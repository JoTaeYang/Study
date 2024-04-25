#pragma once

class CFighterServer : public CServer
{
private:
	//플레이어 클래스 추가 필요.
	//플레이어와 동일한 인덱스, 배열 사용
	CCharacter* players;

	
	//LockFreeStack으로 Index 필요. -> 없음. OnClientJoin으로 넘어온 ID에 들어있는 index와 같은 index를 사용할 것임.

	void CreatePlayer(__int64 SessionID,DWORD SessionIndex, CMessageBuffer* buffer);

	
	//섹터의 기능을 써야 하므로 Sector 클래스가 필요함.
	CSector* sectors;

	
	void SendAround(DWORD index, CMessageBuffer* buffer, bool bSendMe);
	

	void HitPlayerCheck(DWORD index);
private:
	void CharacterMove();
	bool FrameCheck();

	//현재 시간을 저장
	DWORD dwCurrentTime;

	//이전 프레임의 시간
	DWORD dwOldTime;

	//1프레임에 20ms, 20ms를 초과하는 시간들을 저장하는 변수
	DWORD dwFrameTimeTotal;

	//1초가 지났는지 확인하는 변수
	DWORD dwSecTime;

	//
	DWORD dwOnGameFPS;
protected:

	virtual bool OnConnectionRequest(const WCHAR* cwIP, unsigned short sPort);

	virtual void OnClientJoin(unsigned __int64  SessionID);

	virtual void OnClientLeave(unsigned __int64 SessionID);

	virtual void OnRecv(unsigned __int64 SessionID, CMessageBuffer* pBuffer);

	virtual void OnGameUpdate();

public:
	CFighterServer();

	~CFighterServer();

	bool Start(const WCHAR* cwIP, unsigned short sPort, unsigned char chThreadCount, unsigned int uiMaxSession);
private:
	void SectorUpdate(DWORD index);

	//제외하고 싶은 캐릭터를 인자로 전달.
	void SendOneSector(int iSectorX, int iSectorY, CMessageBuffer* pBuffer, CCharacter* pCharacter = nullptr);

private:
	void PacketProc_MoveStart(unsigned __int64 SessionID, CMessageBuffer* pBuffer);

	void PacketProc_MoveStop(unsigned __int64 SessionID, CMessageBuffer* pBuffer);

	void PakcetProc_Attack1(unsigned __int64 SessionID, CMessageBuffer* pBuffer);

	void PakcetProc_Attack2(unsigned __int64 SessionID, CMessageBuffer* pBuffer);

	void PakcetProc_Attack3(unsigned __int64 SessionID, CMessageBuffer* pBuffer);

	void PacketProc_Echo(unsigned __int64 SessionID, CMessageBuffer* pBuffer);

	void MakePacket_Sync(DWORD index, short shX, short shY, CMessageBuffer* pBuffer);

	void MakePacket_MoveStart(DWORD index, BYTE dir, short shX, short shY, CMessageBuffer* pBuffer);

	void MakePacket_MoveStop(DWORD index, BYTE dir, short shX, short shY, CMessageBuffer* pBuffer);

	void MakePacket_DeleteCharacter(DWORD index, CMessageBuffer* pBuffer);

	void MakePacket_Create_OtherUser(DWORD id, BYTE byDir, short shX, short shY, char chHP, CMessageBuffer* pBuffer);

	void MakePacket_Attack1(DWORD id, BYTE byDir, short shX, short shY, CMessageBuffer* pBuffer);

	void MakePacket_Attack2(DWORD id, BYTE byDir, short shX, short shY, CMessageBuffer* pBuffer);

	void MakePacket_Attack3(DWORD id, BYTE byDir, short shX, short shY, CMessageBuffer* pBuffer);

	void MakePacket_Damage(DWORD attackIndex, DWORD hitIndex, char hitHP,CMessageBuffer* pBuffer);

	void MakePacket_ECHO(DWORD time, CMessageBuffer* pBuffer);

};