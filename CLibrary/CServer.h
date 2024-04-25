#pragma once

#pragma pack(push,1)
struct stHEADER
{
	WORD len;
};
#pragma pack(pop)

template<typename T>
class CLockFreeStack;

class CServer
{
public:
	virtual bool Start(const WCHAR* cwIP, unsigned short sPort, unsigned char chThreadCount, unsigned int uiMaxSession);

	bool Stop();


protected:
	//세션 릴리즈
	bool Disconnect(unsigned __int64  SessionID);

	bool SendPacket(unsigned __int64 SessionID, CMessageBuffer* packet);

	virtual bool OnConnectionRequest(const WCHAR* cwIP, unsigned short sPort) = 0;
	virtual void OnClientJoin(unsigned __int64  SessionID) = 0;
	virtual void OnClientLeave(unsigned __int64 SessionID) = 0;
	virtual void OnGameUpdate() = 0;
	virtual void OnRecv(unsigned __int64 SessionID, CMessageBuffer* pBuffer) = 0;

public:
	//int GetSessionCount();
	unsigned int GetAcceptTPS();
	unsigned int GetRecvTPS();
	unsigned int GetSendTPS();
	unsigned int GetProcessCnt();
	unsigned int GetSendThreadFrame();
	unsigned int GetGameThreadFrame();


	unsigned __int64 GetAcceptTotal();

	void SetTPSInitial();

private:
	unsigned int acceptTPS;
	unsigned int recvTPS;
	unsigned int sendTPS;

	unsigned int packetProcessCnt;
	unsigned int sendThreadFrame;
	unsigned int gameThreadFrame;

	unsigned __int64 acceptTotal;

private:
	stSESSION* m_sessionArray;

	unsigned int workerCount;
	HANDLE hIocp; //iocp 핸들
	HANDLE* hThreadHandles; // 워커 스레드 배열
	SOCKET m_listen_sock;

	CLockFreeStack<unsigned __int64>* sessionIndexStack;
private:
	static unsigned int WINAPI AcceptThread(LPVOID lpParam);
	static unsigned int WINAPI WorkerThread(LPVOID lpParam);
	static unsigned int WINAPI GameThread(LPVOID lpParam);
	static unsigned int WINAPI SendThread(LPVOID lpParam);

	bool ThreadStop;
private:
	bool DomainToIP(const WCHAR* cwIP, IN_ADDR* pAddr); //IP주소 찾기

	stSESSION* CreateSession(SOCKET sock);

	//소켓 연결 끊기.
	void SocketClose(stSESSION* pSession);
	void SessionRelease(unsigned __int64 SessionKey);

private:
	void RecvMsgDiv(stSESSION* pSession, DWORD dwTransferred);
	void SendUpdate(stSESSION* pSession, DWORD dwTransferred);

	bool RecvPost(stSESSION* pSession);
	bool SendPost(stSESSION* pSession);

	void IOCountUpdate(stSESSION* pSession);

private:
	void PacketProcess(void);
	void SessionReleaseProcess(void);

protected:
	unsigned int maxSession;

	int testReleaseCnt;
};