#pragma once

//���� �ϳ� ũ�� 128 * 128
#define dfWIDTH_MAX 6400
#define dfHEIGHT_MAX 6400

#define dfSECTOR_WIDTH 200
#define dfSECTOR_HEIGHT 150

#define dfSECTOR_MAX_Y (6400 / dfSECTOR_HEIGHT) + 1
#define dfSECTOR_MAX_X (6400 / dfSECTOR_WIDTH) + 1

struct st_PACKET_HEADER
{
	BYTE	byCode;			// ��Ŷ�ڵ� 0x89 ����.
	BYTE	bySize;			// ��Ŷ ������.
	BYTE	byType;			// ��ŶŸ��.
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

	CLockFreeQueue<CMessageBuffer*> completeRecvQ; //Recv����� �ִ� ť, GameThread�� ó��.

	CRingBuffer RecvRingQ; //Recv RingBuffer

	OVERLEX SendOverlapped;
	OVERLEX RecvOverlapped;

	DWORD dwIOCount;     //send, recv Ƚ��

	LONG lSendFlag;    //send 1ȸ ����

	BOOL logoutFlag;   //�α׾ƿ� �Ǿ��ϴ� ���� flag

	WORD wSendCount;


	//db����.. ���� ������.. �α׾ƿ� ��û ������.. ���
	//4/22 �ӽ� �������� accept ���ڸ��� GameMode�� ����
	enGAMEMODE mode;

	unsigned __int64 Key; //���� ID
};


class CCharacter
{
public:
	CCharacter()
	{
		SessionKey = MAXINT64;
	}

	__int64 SessionKey;//Ű, Ȥ�� ���̵�

	DWORD dwAction;//Player �׼�
	BYTE byDirection; //�¿� ���� �ִ� ����
	BYTE byMoveDirection; //�����̴� ����

	short shX;
	short shY;
	
	st_SECTOR_POS CurSectorPos;
	st_SECTOR_POS OldSectorPos;

	char chHP;

public:
	void InitPlayer(__int64 Key, DWORD Action, BYTE dir, BYTE moveDir, short _x, short _y, char hp);

	bool CompareXY(short shX, short shY);
};