#pragma once

class CFighterServer : public CServer
{
private:
	//�÷��̾� Ŭ���� �߰� �ʿ�.
	//�÷��̾�� ������ �ε���, �迭 ���
	CCharacter* players;

	
	//LockFreeStack���� Index �ʿ�. -> ����. OnClientJoin���� �Ѿ�� ID�� ����ִ� index�� ���� index�� ����� ����.

	void CreatePlayer(__int64 SessionID,DWORD SessionIndex, CMessageBuffer* buffer);

	
	//������ ����� ��� �ϹǷ� Sector Ŭ������ �ʿ���.
	CSector* sectors;

	
	void SendAround(DWORD index, CMessageBuffer* buffer, bool bSendMe);
	

	void HitPlayerCheck(DWORD index);
private:
	void CharacterMove();
	bool FrameCheck();

	//���� �ð��� ����
	DWORD dwCurrentTime;

	//���� �������� �ð�
	DWORD dwOldTime;

	//1�����ӿ� 20ms, 20ms�� �ʰ��ϴ� �ð����� �����ϴ� ����
	DWORD dwFrameTimeTotal;

	//1�ʰ� �������� Ȯ���ϴ� ����
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

	//�����ϰ� ���� ĳ���͸� ���ڷ� ����.
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