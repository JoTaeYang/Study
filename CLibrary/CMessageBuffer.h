#pragma once

template<typename T>
class CMemoryPool;

class  CMessageBuffer
{
public:
	enum en_MESSAGEBUFFER
	{
		eBUFFER_DEFAULT = 5000,
		eDEFAULT_HEADER = 5,
		eLAN_HEADER = 2
	};

	CMessageBuffer();
	CMessageBuffer(int iBufferSize);

	virtual ~CMessageBuffer();

	//����ȭ ���� �ı�
	void Release(void);

	//����ȭ ���� ����
	void Clear(void);

	//����ȭ ���� ������ ���
	int GetBufferSize(void);

	//����ȭ ���� �� ��� �� ������ ���
	int GetDataSize(void);

	//����ȭ ���� ���� ������ ���
	unsigned char* GetBufferPtr(void);

	unsigned char* GetReadBufferPtr(void);

	unsigned char* GetWriteBufferPtr(void);

	/*
	����ȭ ���� ��ġ �̵�, ���� �̵� ����
	ť�� ������. WritePos�� �� �ڿ� �����͸� �߰��ϴ� ��ġ
	*/
	int MoveWritePos(int iSize);

	/*
	����ȭ ���� ��ġ �̵�, ���� �̵� ����
	ť�� ����. ReadPos�� �� �տ��� �����͸� �д� ��ġ
	*/
	int MoveReadPos(int iSize);

	int GetReadPos();
	int GetWritePos();

public:

	int GetPacketDataSize();

	void SetLanHeader();
	void SetHeader(char* src);
	void SetCustomHeader(char* src, int size);
	
	void LanInit();
	void Init();
public:
	//�ֱ�
	CMessageBuffer& operator <<(char chValue);
	CMessageBuffer& operator <<(unsigned char uchValue);

	

	CMessageBuffer& operator <<(short shValue);
	CMessageBuffer& operator <<(unsigned short ushValue);
	
	CMessageBuffer& operator <<(int iValue);
	CMessageBuffer& operator <<(unsigned int uiValue);

	CMessageBuffer& operator <<(float fValue);

	CMessageBuffer& operator <<(long ilValue);
	CMessageBuffer& operator <<(unsigned long uilValue);

	CMessageBuffer& operator <<(INT64 iValue);
	CMessageBuffer& operator <<(double dValue);


	//����
	CMessageBuffer& operator >>(char& chValue);
	CMessageBuffer& operator >>(unsigned char& uchValue);


	CMessageBuffer& operator >>(short& shValue);
	CMessageBuffer& operator >>(unsigned short& ushValue);

	CMessageBuffer& operator >>(int& iValue);
	CMessageBuffer& operator >>(unsigned int& uiValue);

	CMessageBuffer& operator >>(float& fValue);

	CMessageBuffer& operator >>(long& ilValue);
	CMessageBuffer& operator >>(unsigned long& uilValue);

	CMessageBuffer& operator >>(double& dValue);
	CMessageBuffer& operator >>(INT64& iValue);

	int GetData(char* chpDest, int iSize);

	int PutData(char* chpSrc, int iSrcSize);

	int AddRef();
	void DecRef();

protected:
	int m_iReadPos;
	int m_iWritePos;
	int m_iBufferSize;// ������ ũ��

	//�޽��� ���۰� ����ϴ� ��� ũ��.
	int headerSize;

	int m_iUseDataSize;

	unsigned char* m_chBuffer;	

	DWORD* dwrefCnt;

public:
	static CMemoryPool<CMessageBuffer>* pool; //�޸� Ǯ�� ����ϸ鼭 Send Recv TPS�� �� 15���� ��������.

	static CMessageBuffer* Alloc();

	static int GetAllocCount();
};

