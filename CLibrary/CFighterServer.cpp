
#include <WinSock2.h>
#include <random>
#include "Protocol.h"
#include "Common.h"
#include "stSESSION.h"
#include "CSector.h"
#include "C:\ProLib\CCrashDump.h"
#include "CServer.h"
#include "CFighterServer.h"

#define dfFRAME_TIME 40

CFighterServer::CFighterServer() 
{

}

CFighterServer::~CFighterServer()
{
	delete[] players;
	delete sectors;
}

bool CFighterServer::Start(const WCHAR* cwIP, unsigned short sPort, unsigned char chThreadCount, unsigned int uiMaxSession) 
{
	CServer::Start(cwIP, sPort, chThreadCount, uiMaxSession);

	players = new CCharacter[maxSession];

	sectors = new CSector;

	dwSecTime = timeGetTime();
	dwOldTime = timeGetTime();
	dwFrameTimeTotal = 0;


	return true;
}

void CFighterServer::SendAround(DWORD index, CMessageBuffer* buffer, bool bSendMe)
{
	st_SECTOR_AROUND around;
	int iSectorX;
	int iSectorY;


	sectors->GetSectorArounds(players[index].CurSectorPos.iX, players[index].CurSectorPos.iY, &around);

	for (int iCnt = 0; iCnt < around.iCnt; iCnt++)
	{
		iSectorX = around.arounds[iCnt].iX;
		iSectorY = around.arounds[iCnt].iY;

		std::list<CCharacter*>::iterator iterPos;
		std::list<CCharacter*>::iterator iterEnd;
		sectors->GetSectorIterBeginEnd(iSectorX, iSectorY, iterPos, iterEnd);
		for (; iterPos != iterEnd; iterPos++)
		{
			if ((*iterPos)->SessionKey == players[index].SessionKey)
			{
				if (bSendMe)
				{
					SendPacket((*iterPos)->SessionKey, buffer);
				}
			}
			else
			{
				SendPacket((*iterPos)->SessionKey, buffer);
			}
		}
	}
}

bool CFighterServer::OnConnectionRequest(const WCHAR* cwIP, unsigned short sPort)
{
	return true;
}

void CFighterServer::OnClientJoin(unsigned __int64  SessionID)
{
	DWORD index;
	GET_SESSIONINDEX(SessionID, index);

	st_SECTOR_AROUND around;
	
	CMessageBuffer* pBuffer = CMessageBuffer::Alloc();

	pBuffer->Init();

	CreatePlayer(SessionID,index, pBuffer);
	

	////SendPacket���� CreateChar�� ����.
	SendPacket(SessionID, pBuffer);

	sectors->GetSectorArounds(players[index].CurSectorPos.iX, players[index].CurSectorPos.iY, &around);

	CMessageBuffer* otherBuffer = CMessageBuffer::Alloc();

	MakePacket_Create_OtherUser(index, players[index].byDirection, players[index].shX, players[index].shY, players[index].chHP, otherBuffer);

	SendAround(index, otherBuffer, false);

	CMessageBuffer* addotherBuffer;
	CMessageBuffer* otherMoveBuffer;
	int otherIndex;
	std::list<CCharacter*>::iterator iterPos;
	std::list<CCharacter*>::iterator iterEnd;
	for (int iCnt = 0; iCnt < around.iCnt; iCnt++)
	{
		sectors->GetSectorIterBeginEnd(around.arounds[iCnt].iX, around.arounds[iCnt].iY, iterPos, iterEnd);
		for (; iterPos != iterEnd; iterPos++)
		{
			if ((*iterPos)->SessionKey != SessionID)
			{
				addotherBuffer = CMessageBuffer::Alloc();

				GET_SESSIONINDEX((*iterPos)->SessionKey, otherIndex);

				MakePacket_Create_OtherUser(otherIndex, (*iterPos)->byDirection, (*iterPos)->shX, (*iterPos)->shY, (*iterPos)->chHP, addotherBuffer);

				SendPacket(SessionID, addotherBuffer);

				switch ((*iterPos)->dwAction)
				{
				case dfACTION_MOVE_DD:
				case dfACTION_MOVE_LD:
				case dfACTION_MOVE_LL:
				case dfACTION_MOVE_LU:
				case dfACTION_MOVE_UU:
				case dfACTION_MOVE_RU:
				case dfACTION_MOVE_RR:
				case dfACTION_MOVE_RD:
					otherMoveBuffer = CMessageBuffer::Alloc();
					MakePacket_MoveStart(otherIndex, (*iterPos)->dwAction, (*iterPos)->shX, (*iterPos)->shY, otherMoveBuffer);

					SendPacket(players[index].SessionKey, otherMoveBuffer);
					otherMoveBuffer->DecRef();
					break;
				}

				addotherBuffer->DecRef();
			}
		}
	}
		
	otherBuffer->DecRef();
	pBuffer->DecRef();
}

void CFighterServer::OnGameUpdate()
{
	if (FrameCheck())
	{
		CharacterMove();
	}
}

bool CFighterServer::FrameCheck()
{
	dwCurrentTime = timeGetTime();
	
	if (dwCurrentTime - dwSecTime > 1000)
	{
		dwSecTime = dwCurrentTime;
		if (dwOnGameFPS != 25)
		{
			wprintf(L"Frame : %d \n", dwOnGameFPS);
		}
		dwOnGameFPS = 0;
	}

	if (dwFrameTimeTotal > 40)
	{
		dwFrameTimeTotal -= 40;
		CharacterMove();
		dwOnGameFPS++;
	}

	int deltaTime = dwCurrentTime - dwOldTime;

	if (deltaTime < dfFRAME_TIME)
	{
		return false;
	}

	if (deltaTime > 40)
	{
		dwFrameTimeTotal += deltaTime - 40;		
	}
	dwOldTime = timeGetTime();
	dwOnGameFPS++;

	return true;
}

void CFighterServer::CharacterMove()
{
	short playerX;
	short playerY;
	for (int i = 0; i < maxSession; i++)
	{
		if (players[i].SessionKey != MAXINT64)
		{
			switch (players[i].dwAction)
			{
			case dfACTION_MOVE_LL:
				playerX = players[i].shX - dfSPEED_PLAYER_X;
				playerY = players[i].shY;
				if (playerX >= 0 && playerX < dfWIDTH_MAX && playerY >= 0 && playerY < dfHEIGHT_MAX)
				{
					players[i].shX = playerX;
				}
				break;
			case dfACTION_MOVE_LU:
				playerX = players[i].shX - dfSPEED_PLAYER_X;
				playerY = players[i].shY - dfSPEED_PLAYER_Y;
				if (playerX >= 0 && playerX < dfWIDTH_MAX && playerY >= 0 && playerY < dfHEIGHT_MAX)
				{
					players[i].shX = playerX;
					players[i].shY = playerY;
				}
				break;
			case dfACTION_MOVE_UU:
				playerX = players[i].shX;
				playerY = players[i].shY - dfSPEED_PLAYER_Y;
				if (playerX >= 0 && playerX < dfWIDTH_MAX && playerY >= 0 && playerY < dfHEIGHT_MAX)
				{
					players[i].shY = playerY;
				}
				break;
			case dfACTION_MOVE_RU:
				playerX = players[i].shX + dfSPEED_PLAYER_X;
				playerY = players[i].shY - dfSPEED_PLAYER_Y;
				if (playerX >= 0 && playerX < dfWIDTH_MAX && playerY >= 0 && playerY < dfHEIGHT_MAX)
				{
					players[i].shX = playerX;
					players[i].shY = playerY;
				}
				break;
			case dfACTION_MOVE_RR:
				playerX = players[i].shX + dfSPEED_PLAYER_X;
				playerY = players[i].shY;
				if (playerX >= 0 && playerX < dfWIDTH_MAX && playerY >= 0 && playerY < dfHEIGHT_MAX)
				{
					players[i].shX = playerX;
				}
				break;
			case dfACTION_MOVE_RD:
				playerX = players[i].shX + dfSPEED_PLAYER_X;
				playerY = players[i].shY + dfSPEED_PLAYER_Y;
				if (playerX >= 0 && playerX < dfWIDTH_MAX && playerY >= 0 && playerY < dfHEIGHT_MAX)
				{
					players[i].shX = playerX;
					players[i].shY = playerY;
				}
				break;
			case dfACTION_MOVE_DD:
				playerX = players[i].shX;
				playerY = players[i].shY + dfSPEED_PLAYER_Y;
				if (playerX >= 0 && playerX < dfWIDTH_MAX && playerY >= 0 && playerY < dfHEIGHT_MAX)
				{
					players[i].shY = playerY;
				}
				break;
			case dfACTION_MOVE_LD:
				playerX = players[i].shX - dfSPEED_PLAYER_X;
				playerY = players[i].shY + dfSPEED_PLAYER_Y;
				if (playerX >= 0 && playerX < dfWIDTH_MAX && playerY >= 0 && playerY < dfHEIGHT_MAX)
				{
					players[i].shX = playerX;
					players[i].shY = playerY;
				}
				break;
			}

			if (players[i].dwAction >= dfACTION_MOVE_LL && players[i].dwAction <= dfACTION_MOVE_LD)
			{
				if (sectors->SectorUpdateCheck(&players[i]))
				{
					SectorUpdate(i);
				}
			}
		}
	}
}

void CFighterServer::CreatePlayer(__int64 SessionID,DWORD SessionIndex, CMessageBuffer * buffer)
{
	st_PACKET_HEADER header;
	DWORD action;
	short _x;
	short _y;
	char hp;
	BYTE dir;

	std::random_device t;

	std::mt19937 gen(t());

	std::uniform_int_distribution<int> dis(100, 6300);

	header.byCode = 0x89;
	header.byType = dfPACKET_SC_CREATE_MY_CHARACTER;
	header.bySize = 10;

	_x = dis(gen);
	_y = dis(gen);

	hp = 100;

	action = dfACTION_STAND;

	dir = dfPACKET_MOVE_DIR_RR;

	players[SessionIndex].InitPlayer(SessionID, action, dir, 0, _x, _y, hp);

	sectors->SectorAddCharacter(&players[SessionIndex]);

	buffer->SetCustomHeader((char*)&header, sizeof(st_PACKET_HEADER));

	//������ �迭 ��ȣ, player ������ SessionID
	*buffer << SessionIndex << dir << _x << _y << hp;


}

void CFighterServer::OnClientLeave(unsigned __int64 SessionID)
{
	//���Ϳ��� ������� �Ѵ���
	//�׷� ������� �޼����� �� ������ �Ѵ���
	int index;

	GET_SESSIONINDEX(SessionID, index);

	CMessageBuffer* deleteBuffer = CMessageBuffer::Alloc();

	MakePacket_DeleteCharacter(index, deleteBuffer);

	SendAround(index, deleteBuffer, false);

	st_SECTOR_AROUND arounds;
	
	sectors->SectorRemoveCharacter(&players[index]);

	players[index].dwAction = -1;

	players[index].SessionKey = MAXINT64;

	deleteBuffer->DecRef();

}

void CFighterServer::OnRecv(unsigned __int64 SessionID, CMessageBuffer* pBuffer)
{
	//���̺귯���� Ȱ���� ���������� ������ �κп� ��Ŷ�� Ÿ���� ����.
	//�׷��� ȿ���� ���������� ��Ģ�� ��Ű�� ���� ���⼭ �ٽ� packet�� �����.
	st_PACKET_HEADER header;

	pBuffer->GetData((char*)&header, sizeof(st_PACKET_HEADER));

	if (header.byCode != dfPACKET_CODE)
	{
		//���� ���� ����.
		//CCrashDump::Crash();		
	}

	switch (header.byType)
	{
	case dfPACKET_CS_MOVE_START:
		PacketProc_MoveStart(SessionID, pBuffer);
		break;
	case dfPACKET_CS_MOVE_STOP:
		PacketProc_MoveStop(SessionID, pBuffer);
		break;
	case dfPACKET_CS_ATTACK1:
		PakcetProc_Attack1(SessionID, pBuffer);
		break;
	case dfPACKET_CS_ATTACK2:
		PakcetProc_Attack2(SessionID, pBuffer);
		break;
	case dfPACKET_CS_ATTACK3:
		PakcetProc_Attack3(SessionID, pBuffer);
	case dfPACKET_CS_ECHO:
		PacketProc_Echo(SessionID, pBuffer);
		break;
	}
}

void CFighterServer::PacketProc_MoveStart(unsigned __int64 SessionID, CMessageBuffer* pBuffer)
{
	BYTE byDir;
	short shX;
	short shY;
	DWORD index;

	GET_SESSIONINDEX(SessionID, index);

	*pBuffer >> byDir >> shX >> shY;

	if (!players[index].CompareXY(shX, shY))
	{
		CONSOLE_LOG(L"T", e_LOGLEVEL::e_ERROR, L"Move Start Packet - SessionIndex : %d : Action : %d PlayerX : %d PlayerY : %d ServerX : %d ServerY : %d\n",
			index, players[index].dwAction, shX, shY, players[index].shX, players[index].shY);
		//Sync �޽��� ����
		CMessageBuffer* syncBuffer = CMessageBuffer::Alloc();

		syncBuffer->Init();

		MakePacket_Sync(index, shX, shY, syncBuffer);

		///SendAround�� �ٸ� ������ SendQ�� �� ������ AddRef ����� ��. ������ ��.
		SendAround(index, syncBuffer, true);

		shX = players[index].shX;
		shY = players[index].shY;		
		syncBuffer->DecRef();
	}


	switch(byDir)
	{
	case dfPACKET_MOVE_DIR_RR:
	case dfPACKET_MOVE_DIR_RU:
	case dfPACKET_MOVE_DIR_RD:
		players[index].byDirection = dfPACKET_MOVE_DIR_RR;
		break;
	case dfPACKET_MOVE_DIR_LU:
	case dfPACKET_MOVE_DIR_LL:
	case dfPACKET_MOVE_DIR_LD:
		players[index].byDirection = dfPACKET_MOVE_DIR_LL;
		break;		
	}

	players[index].dwAction = byDir;
	players[index].byMoveDirection = byDir;
	players[index].shX = shX;
	players[index].shY = shY;

	CMessageBuffer* moveBuffer = CMessageBuffer::Alloc();

	MakePacket_MoveStart(index, byDir ,players[index].shX, players[index].shY, moveBuffer);

	if (sectors->SectorUpdateCheck(&players[index]))
	{
		SectorUpdate(index);
	}
	//SendAround�� �ٲ�������� ������ SendUniCast�� ����. �ϴ� ��ũ�� �´��� Ȯ���ϱ� ����.
	//��ũ�� ���ؼ� MoveStart�� MoveStop�� �޾ƾ� ��
	SendAround(index, moveBuffer, false);

	moveBuffer->DecRef();
}

void CFighterServer::PacketProc_MoveStop(unsigned __int64 SessionID, CMessageBuffer* pBuffer)
{
	BYTE byDir;
	short shX;
	short shY;
	DWORD index;

	GET_SESSIONINDEX(SessionID, index);

	*pBuffer >> byDir >> shX >> shY;

	if (!players[index].CompareXY(shX, shY))
	{
		//Sync �޽��� ����		
		CONSOLE_LOG(L"Move Stop Sync : ", e_LOGLEVEL::e_ERROR, L"Move Stop Packet - SessionIndex : %d : Action : %d PlayerX : %d PlayerY : %d ServerX : %d ServerY : %d\n",
			index, players[index].dwAction, shX, shY, players[index].shX, players[index].shY);
		CMessageBuffer* syncBuffer = CMessageBuffer::Alloc();

		syncBuffer->Init();

		MakePacket_Sync(index, shX, shY, syncBuffer);

		///SendAround�� �ٸ� ������ SendQ�� �� ������ AddRef ����� ��. ������ ��.
		SendAround(index, syncBuffer, true);

		shX = players[index].shX;
		shY = players[index].shY;
		syncBuffer->DecRef();
	}

	players[index].dwAction = dfACTION_STAND;
	players[index].byDirection = byDir;

	players[index].shX = shX;
	players[index].shY = shY;

	if (sectors->SectorUpdateCheck(&players[index]))
	{
		SectorUpdate(index);
	}

	CMessageBuffer* stopBuffer = CMessageBuffer::Alloc();

	MakePacket_MoveStop(index, players[index].byDirection, players[index].shX, players[index].shY, stopBuffer);

	SendAround(index, stopBuffer, false);

	stopBuffer->DecRef();
}

void CFighterServer::PakcetProc_Attack1(unsigned __int64 SessionID, CMessageBuffer* pBuffer)
{
	BYTE byDir;
	short shX;
	short shY;
	DWORD index;

	GET_SESSIONINDEX(SessionID, index);

	*pBuffer >> byDir >> shX >> shY;

	if (!players[index].CompareXY(shX, shY))
	{
		CONSOLE_LOG(L"T", e_LOGLEVEL::e_ERROR, L"Move Start Packet - SessionIndex : %d : Action : %d PlayerX : %d PlayerY : %d ServerX : %d ServerY : %d\n",
			index, players[index].dwAction, shX, shY, players[index].shX, players[index].shY);
		//Sync �޽��� ����
		CMessageBuffer* syncBuffer = CMessageBuffer::Alloc();

		syncBuffer->Init();

		MakePacket_Sync(index, shX, shY, syncBuffer);

		///SendAround�� �ٸ� ������ SendQ�� �� ������ AddRef ����� ��. ������ ��.
		SendAround(index, syncBuffer, true);

		shX = players[index].shX;
		shY = players[index].shY;
		syncBuffer->DecRef();
	}

	players[index].shX = shX;
	players[index].shY = shY;

	CMessageBuffer* attackBuffer = CMessageBuffer::Alloc();

	MakePacket_Attack1(index, byDir, players[index].shX, players[index].shY, attackBuffer);

	SendAround(index, attackBuffer, false);

	HitPlayerCheck(index);

	players[index].dwAction = dfACTION_STAND;

	attackBuffer->DecRef();
}

void CFighterServer::PakcetProc_Attack2(unsigned __int64 SessionID, CMessageBuffer* pBuffer)
{
	BYTE byDir;
	short shX;
	short shY;
	DWORD index;

	GET_SESSIONINDEX(SessionID, index);

	*pBuffer >> byDir >> shX >> shY;

	if (!players[index].CompareXY(shX, shY))
	{
		CONSOLE_LOG(L"T", e_LOGLEVEL::e_ERROR, L"Move Start Packet - SessionIndex : %d : Action : %d PlayerX : %d PlayerY : %d ServerX : %d ServerY : %d\n",
			index, players[index].dwAction, shX, shY, players[index].shX, players[index].shY);
		//Sync �޽��� ����
		CMessageBuffer* syncBuffer = CMessageBuffer::Alloc();

		syncBuffer->Init();

		MakePacket_Sync(index, shX, shY, syncBuffer);

		///SendAround�� �ٸ� ������ SendQ�� �� ������ AddRef ����� ��. ������ ��.
		SendAround(index, syncBuffer, true);

		shX = players[index].shX;
		shY = players[index].shY;
		syncBuffer->DecRef();
	}

	players[index].shX = shX;
	players[index].shY = shY;

	CMessageBuffer* attackBuffer = CMessageBuffer::Alloc();

	MakePacket_Attack2(index, byDir, players[index].shX, players[index].shY, attackBuffer);

	SendAround(index, attackBuffer, false);

	HitPlayerCheck(index);

	players[index].dwAction = dfACTION_STAND;

	attackBuffer->DecRef();
}

void CFighterServer::PakcetProc_Attack3(unsigned __int64 SessionID, CMessageBuffer* pBuffer)
{
	BYTE byDir;
	short shX;
	short shY;
	DWORD index;

	GET_SESSIONINDEX(SessionID, index);

	*pBuffer >> byDir >> shX >> shY;

	if (!players[index].CompareXY(shX, shY))
	{
		CONSOLE_LOG(L"T", e_LOGLEVEL::e_ERROR, L"Move Start Packet - SessionIndex : %d : Action : %d PlayerX : %d PlayerY : %d ServerX : %d ServerY : %d\n",
			index, players[index].dwAction, shX, shY, players[index].shX, players[index].shY);
		//Sync �޽��� ����
		CMessageBuffer* syncBuffer = CMessageBuffer::Alloc();

		syncBuffer->Init();

		MakePacket_Sync(index, shX, shY, syncBuffer);

		///SendAround�� �ٸ� ������ SendQ�� �� ������ AddRef ����� ��. ������ ��.
		SendAround(index, syncBuffer, true);

		shX = players[index].shX;
		shY = players[index].shY;
		syncBuffer->DecRef();
	}

	players[index].shX = shX;
	players[index].shY = shY;

	CMessageBuffer* attackBuffer = CMessageBuffer::Alloc();

	MakePacket_Attack3(index, byDir, players[index].shX, players[index].shY, attackBuffer);

	SendAround(index, attackBuffer, false);

	HitPlayerCheck(index);

	players[index].dwAction = dfACTION_STAND;

	attackBuffer->DecRef();
}

void CFighterServer::PacketProc_Echo(unsigned __int64 SessionID, CMessageBuffer* pBuffer)
{
	DWORD dwTime;
	CMessageBuffer* buffer = CMessageBuffer::Alloc();
	DWORD index;
	GET_SESSIONINDEX(SessionID, index);
	
	*pBuffer >> dwTime;

	dwTime = timeGetTime();

	MakePacket_ECHO(dwTime, buffer);

	SendPacket(SessionID, buffer);

	buffer->DecRef();
}

void CFighterServer::SectorUpdate(DWORD index)
{
	st_SECTOR_AROUND removeSector, addSector;

	std::list<CCharacter*>::iterator iterPos;
	std::list<CCharacter*>::iterator iterEnd;

	sectors->GetSectorUpdateAround(&players[index], &removeSector, &addSector);

	CMessageBuffer* deleteBuffer = CMessageBuffer::Alloc();
	CMessageBuffer* removeBuffer;

	MakePacket_DeleteCharacter(index, deleteBuffer);

	//RemoveSector�� �ִ� �÷��̾�鿡�� ���� �����̴� �÷��̾ ���ֶ�� ����
	for (int iCnt = 0; iCnt < removeSector.iCnt; iCnt++)
	{
		SendOneSector(removeSector.arounds[iCnt].iX, removeSector.arounds[iCnt].iY, deleteBuffer, &players[index]);
	}

	for (int iCnt = 0; iCnt < removeSector.iCnt; iCnt++)
	{		
		//�ݴ�� ���� �����̴� ������ removeSector�� �ִ� �÷��̾ ������ ����
		sectors->GetSectorIterBeginEnd(removeSector.arounds[iCnt].iX, removeSector.arounds[iCnt].iY, iterPos, iterEnd);
		for (; iterPos != iterEnd; iterPos++)
		{
			DWORD removeIndex;
			GET_SESSIONINDEX((*iterPos)->SessionKey, removeIndex);
			if (removeIndex != index)
			{
				removeBuffer = CMessageBuffer::Alloc();
				MakePacket_DeleteCharacter(removeIndex, removeBuffer);
				SendPacket(players[index].SessionKey, removeBuffer);
				removeBuffer->DecRef();
			}
		}
	}

	CMessageBuffer* createBuffer = CMessageBuffer::Alloc(); //AddSector�� �����̴� �÷��̾ ����
	CMessageBuffer* moveBuffer = CMessageBuffer::Alloc(); //AddSector�� �߰��� �÷��̾ �����̰� �ִٰ� ����. ���߿� ���� ���� ���� ���� �� �� �޽����� �и��� �ʿ䰡 ���µ�

	MakePacket_Create_OtherUser(index, players[index].byDirection,
		players[index].shX, players[index].shY, players[index].chHP, createBuffer);


	MakePacket_MoveStart(index, players[index].dwAction, players[index].shX, players[index].shY, moveBuffer);
	
	CMessageBuffer* otherBuffer;
	CMessageBuffer* otherMoveBuffer;
	int createIndex;

	for (int iCnt = 0; iCnt < addSector.iCnt; iCnt++)
	{
		SendOneSector(addSector.arounds[iCnt].iX, addSector.arounds[iCnt].iY, createBuffer, &players[index]);
	}

	for (int iCnt = 0; iCnt < addSector.iCnt; iCnt++)
	{				
		SendOneSector(addSector.arounds[iCnt].iX, addSector.arounds[iCnt].iY, moveBuffer, &players[index]);
	}
	
	for (int iCnt = 0; iCnt < addSector.iCnt; iCnt++)
	{
		//AddSector�� �����ϴ� �������� �����϶�� ���� �����̴� �÷��̾�� �����ؾ� �Ѵ�.
		//���� �����Ƿ��� �ϴ� �������� Action�� �����̰� �ִٸ� �����̰� ����
		//������ ���ڿ������ٸ� ���� for������ ���� ��
		sectors->GetSectorIterBeginEnd(addSector.arounds[iCnt].iX, addSector.arounds[iCnt].iY, iterPos, iterEnd);
		for (; iterPos != iterEnd; iterPos++)
		{
			if ((*iterPos)->SessionKey != players[index].SessionKey)
			{
				otherBuffer = CMessageBuffer::Alloc();

				GET_SESSIONINDEX((*iterPos)->SessionKey, createIndex);

				MakePacket_Create_OtherUser(createIndex, (*iterPos)->byDirection, (*iterPos)->shX, (*iterPos)->shY, (*iterPos)->chHP,otherBuffer);

				SendPacket(players[index].SessionKey, otherBuffer);
								

				switch ((*iterPos)->dwAction)
				{
				case dfACTION_MOVE_DD:
				case dfACTION_MOVE_LD:
				case dfACTION_MOVE_LL:
				case dfACTION_MOVE_LU:
				case dfACTION_MOVE_UU:
				case dfACTION_MOVE_RU:
				case dfACTION_MOVE_RR:
				case dfACTION_MOVE_RD:
					otherMoveBuffer = CMessageBuffer::Alloc();
					MakePacket_MoveStart(createIndex, (*iterPos)->dwAction, (*iterPos)->shX, (*iterPos)->shY, otherMoveBuffer);

					SendPacket(players[index].SessionKey, otherMoveBuffer);
					otherMoveBuffer->DecRef();
					break;
				}
				otherBuffer->DecRef();
			}
		}
	}
	moveBuffer->DecRef();
	createBuffer->DecRef();
	deleteBuffer->DecRef();
}

void CFighterServer::SendOneSector(int iSectorX, int iSectorY, CMessageBuffer* pBuffer, CCharacter* pCharacter)
{
	std::list<CCharacter*>::iterator iterPos;
	std::list<CCharacter*>::iterator iterEnd;
	sectors->GetSectorIterBeginEnd(iSectorX, iSectorY, iterPos, iterEnd);
	if (!pCharacter)
	{
		for (; iterPos != iterEnd; iterPos++)
		{
			SendPacket((*iterPos)->SessionKey, pBuffer);
		}
	}
	else
	{
		for (; iterPos != iterEnd; iterPos++)
		{
			if ((*iterPos)->SessionKey != pCharacter->SessionKey)
			{
				SendPacket((*iterPos)->SessionKey, pBuffer);
			}
		}
	}
}

void CFighterServer::MakePacket_Sync(DWORD index, short shX, short shY, CMessageBuffer* pBuffer)
{
	st_PACKET_HEADER header;

	header.byCode = dfPACKET_CODE;
	header.byType = dfPACKET_SC_SYNC;
	header.bySize = 6;

	pBuffer->SetCustomHeader((char*)&header, sizeof(st_PACKET_HEADER));

	*pBuffer << index << shX << shY;
}

void CFighterServer::MakePacket_MoveStart(DWORD index, BYTE dir, short shX, short shY, CMessageBuffer* pBuffer)
{
	st_PACKET_HEADER header;

	header.byCode = dfPACKET_CODE;
	header.byType = dfPACKET_SC_MOVE_START;
	header.bySize = 9;
	
	pBuffer->SetCustomHeader((char*)&header, sizeof(st_PACKET_HEADER));

	*pBuffer << index << dir << shX << shY;
}

void CFighterServer::MakePacket_MoveStop(DWORD index, BYTE dir, short shX, short shY, CMessageBuffer* pBuffer)
{
	st_PACKET_HEADER header;

	header.byCode = dfPACKET_CODE;
	header.byType = dfPACKET_SC_MOVE_STOP;
	header.bySize = 9;

	pBuffer->SetCustomHeader((char*)&header, sizeof(st_PACKET_HEADER));

	*pBuffer << index << dir << shX << shY;
}

void CFighterServer::MakePacket_DeleteCharacter(DWORD index, CMessageBuffer* pBuffer)
{
	st_PACKET_HEADER header;

	header.byCode = dfPACKET_CODE;
	header.byType = dfPACKET_SC_DELETE_CHARACTER;
	header.bySize = 4;

	pBuffer->SetCustomHeader((char*)&header, sizeof(st_PACKET_HEADER));

	*pBuffer << index;
}

void CFighterServer::MakePacket_Create_OtherUser(DWORD id, BYTE byDir, short shX, short shY, char chHP, CMessageBuffer* pBuffer)
{
	st_PACKET_HEADER header;

	header.byCode = dfPACKET_CODE;
	header.byType = dfPACKET_SC_OTHER_CREATE_CHARACTER;
	header.bySize = 10;

	pBuffer->SetCustomHeader((char*)&header, sizeof(st_PACKET_HEADER));

	*pBuffer << id << byDir << shX << shY << chHP;
}

void CFighterServer::MakePacket_Attack1(DWORD id, BYTE byDir, short shX, short shY,CMessageBuffer* pBuffer)
{
	st_PACKET_HEADER header;

	header.byCode = dfPACKET_CODE;
	header.byType = dfPACKET_SC_ATTACK1;
	header.bySize = 9;

	pBuffer->SetCustomHeader((char*)&header, sizeof(st_PACKET_HEADER));

	*pBuffer << id << byDir << shX << shY;
}

void CFighterServer::MakePacket_Attack2(DWORD id, BYTE byDir, short shX, short shY,CMessageBuffer* pBuffer)
{
	st_PACKET_HEADER header;

	header.byCode = dfPACKET_CODE;
	header.byType = dfPACKET_SC_ATTACK2;
	header.bySize = 9;

	pBuffer->SetCustomHeader((char*)&header, sizeof(st_PACKET_HEADER));

	*pBuffer << id << byDir << shX << shY;
}

void CFighterServer::MakePacket_Attack3(DWORD id, BYTE byDir, short shX, short shY,CMessageBuffer* pBuffer)
{
	st_PACKET_HEADER header;

	header.byCode = dfPACKET_CODE;
	header.byType = dfPACKET_SC_ATTACK3;
	header.bySize = 9;

	pBuffer->SetCustomHeader((char*)&header, sizeof(st_PACKET_HEADER));

	*pBuffer << id << byDir << shX << shY;
}

void CFighterServer::MakePacket_Damage(DWORD attackIndex, DWORD hitIndex, char hitHP,CMessageBuffer* pBuffer)
{
	st_PACKET_HEADER header;

	header.byCode = dfPACKET_CODE;
	header.byType = dfPACKET_SC_DAMAGE;
	header.bySize = 9;

	pBuffer->SetCustomHeader((char*)&header, sizeof(st_PACKET_HEADER));

	*pBuffer << attackIndex << hitIndex << hitHP;
}

void CFighterServer::MakePacket_ECHO(DWORD time, CMessageBuffer* pBuffer)
{
	st_PACKET_HEADER header;

	header.byCode = dfPACKET_CODE;
	header.byType = dfPACKET_SC_ECHO;
	header.bySize = 4;

	pBuffer->SetCustomHeader((char*)&header, sizeof(st_PACKET_HEADER));

	*pBuffer << time;
}

void CFighterServer::HitPlayerCheck(DWORD index)
{
	st_SECTOR_AROUND checkArounds;

	std::list<CCharacter*>::iterator iterPos;
	std::list<CCharacter*>::iterator iterEnd;

	int iAttackX, iAttackY, hitIndex;

	CMessageBuffer* attackBuffer;

	sectors->GetSectorArounds(players[index].CurSectorPos.iX, players[index].CurSectorPos.iY, &checkArounds);

	if (players[index].byDirection == dfPACKET_MOVE_DIR_RR)
	{
		iAttackX = players[index].shX + dfATTACK_RANGE_X;
		iAttackY = players[index].shY + dfATTACK_RANGE_Y;
		for (int iCnt = 0; iCnt < checkArounds.iCnt; iCnt++)
		{
			sectors->GetSectorIterBeginEnd(checkArounds.arounds[iCnt].iX, checkArounds.arounds[iCnt].iY, iterPos, iterEnd);
			for (; iterPos != iterEnd; iterPos++)
			{
				if ((*iterPos)->shX > players[index].shX)
				{
					if ((*iterPos)->shX < iAttackX &&
						((*iterPos)->shY < iAttackY &&  (*iterPos)->shY > players[index].shY - 25))
					{
						attackBuffer = CMessageBuffer::Alloc();
						GET_SESSIONINDEX((*iterPos)->SessionKey, hitIndex);
						
						(*iterPos)->chHP -= 5;
						
						MakePacket_Damage(index, hitIndex, (*iterPos)->chHP, attackBuffer);
						
						SendAround(hitIndex, attackBuffer, true);

						attackBuffer->DecRef();
					}
				}
			}
		}
	}
	else if (players[index].byDirection == dfPACKET_MOVE_DIR_LL)
	{
		iAttackX = players[index].shX - dfATTACK_RANGE_X;
		iAttackY = players[index].shY + dfATTACK_RANGE_Y;
		for (int iCnt = 0; iCnt < checkArounds.iCnt; iCnt++)
		{
			sectors->GetSectorIterBeginEnd(checkArounds.arounds[iCnt].iX, checkArounds.arounds[iCnt].iY, iterPos, iterEnd);
			for (; iterPos != iterEnd; iterPos++)
			{
				if ((*iterPos)->shX < players[index].shX)
				{
					if ((*iterPos)->shX > iAttackX &&
						((*iterPos)->shY < iAttackY && (*iterPos)->shY > players[index].shY - 25))
					{
						attackBuffer = CMessageBuffer::Alloc();
						GET_SESSIONINDEX((*iterPos)->SessionKey, hitIndex);

						(*iterPos)->chHP -= 5;

						MakePacket_Damage(index, hitIndex, (*iterPos)->chHP, attackBuffer);

						SendAround(hitIndex, attackBuffer, true);

						attackBuffer->DecRef();
					}
				}
			}
		}
	}

	
}