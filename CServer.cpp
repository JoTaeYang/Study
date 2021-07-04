
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "Common.h"
#include "stSESSION.h"
#include "CSector.h"
#include "C:\ProLib\CCrashDump.h"
#include "CServer.h"

int session_key = 0;

#define SET_SESSION(INDEX, KEY) KEY = ((INDEX << 47) | (++session_key))
#define GET_SESSIONINDEX(KEY, INDEX) INDEX = ((KEY) >> 47)
#define dfMAXBUF 100

bool CServer::Start(const WCHAR* cwIP, unsigned short sPort, unsigned char chThreadCount, unsigned int uiMaxSession)
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return false;

	hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	m_listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_listen_sock == INVALID_SOCKET)
		return false;

	SOCKADDR_IN serveraddr;
	IN_ADDR addr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	DomainToIP(cwIP, &addr);
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr = addr;
	serveraddr.sin_port = htons(sPort);
	int ret_bind = bind(m_listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (ret_bind == SOCKET_ERROR)
	{
		printf("WSAGetLastError : %d", WSAGetLastError());
		return false;
	}

	int ret_listen = listen(m_listen_sock, SOMAXCONN);
	if (ret_listen == SOCKET_ERROR)
		return 1;

	ThreadStop = false;

	maxSession = uiMaxSession;

	m_sessionArray = new stSESSION[uiMaxSession];
	

	sessionIndexStack = new CLockFreeStack<unsigned __int64>;

	for (int i = maxSession - 1; i >= 0; i--)
	{
		sessionIndexStack->Push(i);
	}



	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, AcceptThread, (LPVOID)this, 0, NULL);
	CloseHandle(hThread); //Accept Thread의 return은 listen socket close;
	hThread = (HANDLE)_beginthreadex(NULL, 0, GameThread, (LPVOID)this, 0, NULL);
	CloseHandle(hThread);
	hThread = (HANDLE)_beginthreadex(NULL, 0, SendThread, (LPVOID)this, 0, NULL);
	CloseHandle(hThread);

	workerCount = chThreadCount;



	//this 넘겨주는 이유 : static 함수라 클래스 멤버 변수에 접근을 하지 못하므로 넘겨준다.
	hThreadHandles = new HANDLE[workerCount]; // 워커 스레드 개수 만큼 생성
	for (unsigned int iCnt = 0; iCnt < workerCount; iCnt++)
	{
		hThreadHandles[iCnt] = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, (LPVOID)this, 0, NULL);
	}

	printf("Server Open ..\n");

	testReleaseCnt = 0;
	return true;
}

bool CServer::DomainToIP(const WCHAR* cwIP, IN_ADDR* pAddr)
{
	ADDRINFOW* pAddrInfoW;
	SOCKADDR_IN* pSockAddr;
	if (GetAddrInfo(cwIP, L"0", NULL, &pAddrInfoW) != 0)
	{
		return false;
	}
	pSockAddr = (SOCKADDR_IN*)pAddrInfoW->ai_addr;
	*pAddr = pSockAddr->sin_addr;
	FreeAddrInfoW(pAddrInfoW);
	return true;
}

unsigned int WINAPI CServer::AcceptThread(LPVOID lpParam)
{
	CServer* pLanServer = (CServer*)lpParam;
	SOCKADDR_IN clientaddr;
	SOCKET client_sock;
	stSESSION* pSession = NULL;
	int addrlen = sizeof(clientaddr);

	while (1)
	{
		client_sock = accept(pLanServer->m_listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			break;
		}

		WCHAR IP[32];
		InetNtop(AF_INET, &clientaddr.sin_addr, IP, 32);
		if (pLanServer->OnConnectionRequest(IP, clientaddr.sin_port))
		{
			pSession = pLanServer->CreateSession(client_sock);
			if (!pSession)
			{
				CCrashDump::Crash();
			}
			CreateIoCompletionPort((HANDLE)pSession->sock, pLanServer->hIocp, (ULONG_PTR)pSession, 0);
			if (!pLanServer->RecvPost(pSession))
			{
				//Accept에서 Recv 실패
				if (InterlockedDecrement(&pSession->dwIOCount) == 0)
				{
					if (pSession->dwIOCount < 0)
					{
						//혹시 음수로 떨어지게 되면 로그남기기.
					}
					//세션 릴리즈
					pSession->logoutFlag = true;
					//pLanServer->SessionRelease(pSession->Key);
					//pLanServer->OnClientLeave(pSession->Key);
				}
			}
			pLanServer->OnClientJoin(pSession->Key);
			InterlockedIncrement(&pLanServer->acceptTotal);
			InterlockedIncrement(&pLanServer->acceptTPS);
			//InterlockedIncrement(&pLanServer->sessionCount);
		}

	}
	return 0;
}

unsigned int WINAPI CServer::WorkerThread(LPVOID lpParam)
{
	CServer* pLanServer = (CServer*)lpParam;
	stSESSION* pSession = NULL;
	DWORD dwTransferred = 0;
	OVERLEX* wsOverlapped = NULL;
	bool isGQCS;
	while (1)
	{
		dwTransferred = 0;
		wsOverlapped = NULL;
		pSession = NULL;

		isGQCS = GetQueuedCompletionStatus(pLanServer->hIocp, &dwTransferred, (PULONG_PTR)&pSession, (LPOVERLAPPED*)&wsOverlapped, INFINITE);

		if (dwTransferred == 0)
		{
			if (pSession == NULL && wsOverlapped == NULL)
			{
				//스레드 종료 절차
				CCrashDump::Crash();
				break;
			}
			else if (pSession != NULL) // 길이가 0 일 때 IOCount 1 감소, 0 이되면 세션 Release 
			{
				if (InterlockedDecrement(&pSession->dwIOCount) == 0)
				{
					if (pSession->dwIOCount < 0)
						printf("IOCount Minus \n");
					pSession->logoutFlag = true;
				}
			}
			else
				CCrashDump::Crash(); // 그 외는 오류 상황
		}
		else
		{
			if (wsOverlapped->type == 1)
			{
				pLanServer->RecvMsgDiv(pSession, dwTransferred);
			}
			else if (wsOverlapped->type == 2)
			{
				pLanServer->SendUpdate(pSession, dwTransferred);
			}

			if (InterlockedDecrement(&pSession->dwIOCount) == 0)
			{
				if (pSession->dwIOCount < 0)
					printf("IOCount Minus \n");
				pSession->logoutFlag = true;
			}
		}

	}
	return 0;
}

stSESSION* CServer::CreateSession(SOCKET sock)
{
	unsigned __int64 findIndex = 0;

	stSESSION* pSession = NULL;
	if (!sessionIndexStack->Pop(findIndex))
	{
		return NULL;
	}

	pSession = &m_sessionArray[findIndex];
	pSession->sock = sock;

	pSession->RecvRingQ.Clear();

	//pSession->byCloseFlag = 1;
	pSession->dwIOCount = 0;
	pSession->lSendFlag = 0;
	pSession->wSendCount = 0;

	pSession->mode = MODE_GAME;

	SET_SESSION(findIndex, pSession->Key);

	return pSession;
}

bool CServer::RecvPost(stSESSION* pSession)
{
	WSABUF wsaBuf[2] = { 0, };
	DWORD flags = 0;
	int ret_Recv;
	int bufCount = 2;

	InterlockedIncrement(&pSession->dwIOCount);

	wsaBuf[0].buf = pSession->RecvRingQ.GetRearBufferPtr();
	wsaBuf[0].len = pSession->RecvRingQ.DirectEnqueueSize();

	if (pSession->RecvRingQ.GetFrontBufferPtr() - pSession->RecvRingQ.GetStartBufferPtr() >= 10)
	{
		wsaBuf[1].buf = pSession->RecvRingQ.GetStartBufferPtr();
		wsaBuf[1].len = pSession->RecvRingQ.GetFreeSize() - pSession->RecvRingQ.DirectEnqueueSize();
	}
	else
		bufCount = 1;

	ZeroMemory(&pSession->RecvOverlapped, sizeof(WSAOVERLAPPED));
	ret_Recv = WSARecv(pSession->sock, wsaBuf, bufCount, NULL, &flags, (WSAOVERLAPPED*)&pSession->RecvOverlapped, NULL);
	if (ret_Recv == SOCKET_ERROR)
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			//소켓 끊기 단계 돌입.
			//SocketClose(pSession);
			//pSession->logoutFlag = true;
			return false;
		}
		//else if (WSAGetLastError() == WSA_IO_PENDING)
		//{
		//	printf("hello\n");
		//}

	}
	return true;
}

bool CServer::SendPost(stSESSION* pSession)
{
	WSABUF wsaBuf[dfMAXBUF] = { 0, };
	DWORD flags = 0;
	int ret_Send;
	int ret_Peek;
	int bufCount = 0;
	int size1 = pSession->SendQ.GetCount();
	if (size1 <= 0)
	{
		//InterlockedDecrement(&pSession->lSendFlag);
		return true;
	}

	if (InterlockedExchange(&pSession->lSendFlag, 1) == 0)
	{
		//CRingBuffer* SendRingQ = &pSession->SendRingQ;
		CLockFreeQueue<CMessageBuffer*>* pSend = &pSession->SendQ;

		CMessageBuffer* pBuffer = NULL;

		if (pSend->GetCount() <= 0)
		{
			InterlockedExchange(&pSession->lSendFlag, 0);
			return true;
		}

		while (pSend->GetCount() > bufCount)
		{
			if (bufCount >= dfMAXBUF)
				break;

			pBuffer = NULL;

			ret_Peek = pSend->Peek(&pBuffer, bufCount);

			if (ret_Peek == -1 || pBuffer == NULL) break;


			wsaBuf[bufCount].buf = (char*)pBuffer->GetBufferPtr();
			wsaBuf[bufCount].len = pBuffer->GetPacketDataSize();

			bufCount++;
		}

		if (bufCount == 0)
		{
			InterlockedExchange(&pSession->lSendFlag, 0);
			return true;
		}

		pSession->wSendCount = bufCount;
		InterlockedIncrement(&pSession->dwIOCount);

		ZeroMemory(&pSession->SendOverlapped, sizeof(WSAOVERLAPPED));

		ret_Send = WSASend(pSession->sock, wsaBuf, bufCount, NULL, flags, (WSAOVERLAPPED*)&pSession->SendOverlapped, NULL);
		if (ret_Send == SOCKET_ERROR)
		{
			if (WSAGetLastError() != ERROR_IO_PENDING)
			{
				//SocketClose(pSession);
				//pSession->logoutFlag = true;
				InterlockedExchange(&pSession->lSendFlag, 0);
				pSession->wSendCount = 0;
				return false;
			}
		}
	}

	return true;
}

void CServer::SocketClose(stSESSION* pSession)
{
	//if ((BOOL)_InterlockedCompareExchange8((char*)&pSession->byCloseFlag, 0, 1) == TRUE)
	//{
	//	//closesocket(pSession->sock);
	//	//closesocket(pSession->sock);
	//}
}

/*
추후 수정이 생길 가능성이 농후한 함수
*/
bool CServer::Disconnect(unsigned __int64 SessionKey)
{
	WORD index;

	GET_SESSIONINDEX(SessionKey, index);

	m_sessionArray[index].sock = INVALID_SOCKET;

	return true;
}

//int CServer::GetSessionCount()
//{
//	return sessionCount;
//}

void CServer::RecvMsgDiv(stSESSION* pSession, DWORD dwTransferred)
{
	int ret_Deque = 0;
	CMessageBuffer* pBuffer;
	CRingBuffer* RecvRingQ = &pSession->RecvRingQ;
	RecvRingQ->MoveRear(dwTransferred);
	stHEADER header;
	while (dwTransferred > 0)
	{
		pBuffer = CMessageBuffer::Alloc();

		st_PACKET_HEADER header;
		ret_Deque = RecvRingQ->Peek((char*)&header, sizeof(st_PACKET_HEADER));

		if (header.byCode != 0x89)
		{
			printf("header Code : %x\n", header.byCode);
			pBuffer->DecRef();
			break;
		}

		if (RecvRingQ->GetUseSize() < header.bySize + sizeof(st_PACKET_HEADER))
		{
			pBuffer->DecRef();
			break;
		}
		//printf("Header Type : %d Size : %d Code : %d\n", header.byType, header.bySize, header.byCode);
		ret_Deque = RecvRingQ->Dequeue((char*)pBuffer->GetWriteBufferPtr(), sizeof(st_PACKET_HEADER) + header.bySize);

		dwTransferred -= ret_Deque;

		pBuffer->MoveWritePos(sizeof(st_PACKET_HEADER) + header.bySize);
#ifndef TCPFIGHTER
		ret_Deque = RecvRingQ->Dequeue((char*)&header, sizeof(stHEADER));

		dwTransferred -= ret_Deque;

		//header 말고 data 부분, 즉 CookieServer에서 Proc을 할 때 쓰는 Type은 data 부분에 있다.
		//그래서 Header는 Dequeue로 날려버리고 Data 부분에서 추출해서 쓸 것.
		ret_Deque = RecvRingQ->Dequeue((char*)pBuffer->GetWriteBufferPtr(), header.len);

		dwTransferred -= ret_Deque;

		pBuffer->MoveWritePos(header.len);
#endif
		pBuffer->AddRef();
		pSession->completeRecvQ.Enqueue(pBuffer);

		//OnRecv(pSession->Key, &pBuffer);
		pBuffer->DecRef();
		InterlockedIncrement(&recvTPS);
	}

	if (!RecvPost(pSession))
	{
		IOCountUpdate(pSession);
	}

}

void CServer::SendUpdate(stSESSION* pSession, DWORD dwTransferred)
{
	int iCnt = pSession->wSendCount;

	CMessageBuffer* pBuffer = NULL;
	CLockFreeQueue<CMessageBuffer*>* pSend = &pSession->SendQ;
	while (iCnt > 0)
	{
		pBuffer = NULL;

		pSend->Dequeue(pBuffer);

		if (!pBuffer)
		{
			printf("pBuffer Is Null\n");
			continue;
		}

		pBuffer->DecRef();

		iCnt--;

		InterlockedIncrement(&sendTPS);
	}

	InterlockedDecrement(&pSession->lSendFlag);

	//if (!SendPost(pSession))
	//{
	//	return false;
	//}
}

bool CServer::SendPacket(unsigned __int64 SessionID, CMessageBuffer* packet)
{
	WORD dwLen = 8;
	LONG flag;
	WORD index = 0;

	GET_SESSIONINDEX(SessionID, index);

	stSESSION* pSession = &m_sessionArray[index];
	//CRingBuffer* SendRingQ = &pSession->SendRingQ;
	flag = pSession->lSendFlag;

	packet->AddRef();

	pSession->SendQ.Enqueue(packet);

#ifndef TCPFIGHTER
	packet->MoveReadPos(dwLen);
#endif
	return true;
}

void CServer::IOCountUpdate(stSESSION* pSession)
{
	if (InterlockedDecrement(&pSession->dwIOCount) == 0)
	{
		if (pSession->dwIOCount < 0)
			printf("IOCount Minus \n");
		pSession->logoutFlag = true;
		//SessionRelease(pSession->Key);//세션 릴리즈
		//OnClientLeave(pSession->Key);
	}
}

bool CServer::Stop()
{
	closesocket(m_listen_sock);

	ThreadStop = true;

	for (unsigned int iCnt = 0; iCnt < maxSession; iCnt++)
	{
		if (m_sessionArray[iCnt].sock != INVALID_SOCKET)
		{
			closesocket(m_sessionArray[iCnt].sock);
		}
	}

	for (unsigned int iCnt = 0; iCnt < workerCount; iCnt++)
		PostQueuedCompletionStatus(hIocp, 0, 0, 0);

	WaitForMultipleObjects(workerCount, hThreadHandles, TRUE, INFINITE);

	for (unsigned int iCnt = 0; iCnt < workerCount; iCnt++)
		CloseHandle(hThreadHandles[iCnt]);

	delete[] hThreadHandles;
	delete[] m_sessionArray;

	WSACleanup();
	return true;
}

unsigned int CServer::GetAcceptTPS()
{
	return acceptTPS;
}

unsigned int CServer::GetRecvTPS()
{
	return recvTPS;
}

unsigned int CServer::GetSendTPS()
{
	return sendTPS;
}

unsigned int CServer::GetProcessCnt()
{
	return packetProcessCnt;
}

unsigned int CServer::GetSendThreadFrame()
{
	return sendThreadFrame;
}

unsigned int CServer::GetGameThreadFrame()
{
	return gameThreadFrame;
}

unsigned __int64 CServer::GetAcceptTotal()
{
	return acceptTotal;
}

void CServer::SetTPSInitial()
{
	acceptTPS = 0;
	recvTPS = 0;
	sendTPS = 0;
	packetProcessCnt = 0;
	sendThreadFrame = 0;
	gameThreadFrame = 0;
}

void CServer::SessionRelease(unsigned __int64 SessionKey)
{
	WORD index;

	GET_SESSIONINDEX(SessionKey, index);

	CMessageBuffer* pBuffer = NULL;

	while (1)
	{
		pBuffer = NULL;

		m_sessionArray[index].SendQ.Dequeue(pBuffer);

		if (!pBuffer) break;

		pBuffer->DecRef();
	}

	while (1)
	{
		pBuffer = NULL;

		m_sessionArray[index].completeRecvQ.Dequeue(pBuffer);

		if (!pBuffer) break;

		pBuffer->DecRef();
	}

	// 인덱스의 Push 바로 위에서 Closesocket을 해주니까 소켓이 제대로 정리되지 않았다. 이유가 뭘까..? Invalid_socket 하고 closesocket을 하고 있었네.. 등신
	closesocket(m_sessionArray[index].sock);

	m_sessionArray[index].wSendCount = 0;

	m_sessionArray[index].sock = INVALID_SOCKET;

	m_sessionArray[index].Key = MAXINT64;

	m_sessionArray[index].mode = MODE_NULL;

	m_sessionArray[index].logoutFlag = false;



	sessionIndexStack->Push(index);
}

unsigned int WINAPI CServer::GameThread(LPVOID lpParam)
{
	CServer* pServer = (CServer*)lpParam;

	while (!pServer->ThreadStop)
	{
		pServer->PacketProcess();
		pServer->OnGameUpdate();
		pServer->SessionReleaseProcess();
		pServer->gameThreadFrame++;
		Sleep(5);
	}
	return 0;
}

void CServer::PacketProcess(void)
{
	int count = 0, loop = 0;
	CMessageBuffer* pBuf = NULL;

	for (count; count < maxSession; count++)
	{
		if (m_sessionArray[count].mode == MODE_GAME)
		{
			loop = 0;
			while (loop < 2000)
			{
				pBuf = NULL;
				m_sessionArray[count].completeRecvQ.Dequeue(pBuf);
				if (pBuf == NULL)
					break;
				InterlockedIncrement((long*)&packetProcessCnt);
				OnRecv(m_sessionArray[count].Key, pBuf);
				pBuf->DecRef();
				loop++;
			}
		}
	}
}

void CServer::SessionReleaseProcess(void)
{
	int count = 0, loop = 0;
	CMessageBuffer* pBuf = NULL;
	
	for (count; count < maxSession; count++)
	{
		if (loop > 2000) break;
		if (m_sessionArray[count].logoutFlag == true && m_sessionArray[count].lSendFlag == 0 )
		{
			OnClientLeave(m_sessionArray[count].Key);
			SessionRelease(m_sessionArray[count].Key);
			testReleaseCnt++;
			loop++;
		}
	}
}

unsigned int WINAPI CServer::SendThread(LPVOID lpParam)
{
	CServer* pServer = (CServer*)lpParam;

	stSESSION* session = NULL;
	while (!pServer->ThreadStop)
	{
		session = (pServer->m_sessionArray);
		int iCnt = 0;
		for (; iCnt < pServer->maxSession; iCnt++)
		{
			if (session[iCnt].mode != MODE_GAME)
				continue;
			if (session[iCnt].lSendFlag == 0)
			{
				if (!pServer->SendPost(&session[iCnt]))
				{
					pServer->IOCountUpdate(&session[iCnt]);
				}
			}
		}
		pServer->sendThreadFrame++;
		Sleep(2);
	}
	return 0;
}