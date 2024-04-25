#pragma once

//섹터 하나 크기 128 * 128
#define dfWIDTH_MAX 6400
#define dfHEIGHT_MAX 6400

#define dfSECTOR_WIDTH 200
#define dfSECTOR_HEIGHT 150

#define dfSECTOR_MAX_Y (6400 / dfSECTOR_HEIGHT) + 1
#define dfSECTOR_MAX_X (6400 / dfSECTOR_WIDTH) + 1

struct st_PACKET_HEADER
{
	BYTE	byCode;			// 패킷코드 0x89 고정.
	BYTE	bySize;			// 패킷 사이즈.
	BYTE	byType;			// 패킷타입.
};

struct st_SECTOR_POS
{
	int iX;
	int iY;
};

struct st_SECTOR_AROUND
{
	st_SECTOR_AROUND()
	{
		iCnt = 0;
	}
	int iCnt;
	st_SECTOR_POS arounds[9];
};

struct OVERLEX
{
	WSAOVERLAPPED overlapped;
	int type;
};

template <typename T>
class CLockFreeQueue;

class CMessageBuffer;


enum enGAMEMODE
{
	MODE_NULL, MODE_GAME, MODE_WAIT_GAME_LOGOUT
};

struct stSESSION
{
	stSESSION()
	{
		dwIOCount = 0;
		sock = INVALID_SOCKET;
		Key = MAXINT64;


		RecvOverlapped.type = 1;
		SendOverlapped.type = 2;

		wSendCount = 0;

		mode = MODE_NULL;
		logoutFlag = false;
	}

	SOCKET sock;


	//CRingBuffer SendRingQ;

	CLockFreeQueue<CMessageBuffer*> SendQ; 

	CLockFreeQueue<CMessageBuffer*> completeRecvQ; //Recv결과를 넣는 큐, GameThread가 처리.

	CRingBuffer RecvRingQ; //Recv RingBuffer

	OVERLEX SendOverlapped;
	OVERLEX RecvOverlapped;

	DWORD dwIOCount;     //send, recv 횟수

	LONG lSendFlag;    //send 1회 제한

	BOOL logoutFlag;   //로그아웃 되야하는 상태 flag

	WORD wSendCount;


	//db인증.. 게임 중인지.. 로그아웃 요청 중인지.. 등등
	//4/22 임시 방편으로 accept 하자마자 GameMode로 변경
	enGAMEMODE mode;

	unsigned __int64 Key; //세션 ID
};


class CCharacter
{
public:
	CCharacter()
	{
		SessionKey = MAXINT64;
	}

	__int64 SessionKey;//키, 혹은 아이디

	DWORD dwAction;//Player 액션
	BYTE byDirection; //좌우 보고 있는 방향
	BYTE byMoveDirection; //움직이는 방향

	short shX;
	short shY;
	
	st_SECTOR_POS CurSectorPos;
	st_SECTOR_POS OldSectorPos;

	char chHP;

public:
	void InitPlayer(__int64 Key, DWORD Action, BYTE dir, BYTE moveDir, short _x, short _y, char hp);

	bool CompareXY(short shX, short shY);
};