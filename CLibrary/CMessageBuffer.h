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

	//직렬화 버퍼 파괴
	void Release(void);

	//직렬화 버퍼 비우기
	void Clear(void);

	//직렬화 버퍼 사이즈 얻기
	int GetBufferSize(void);

	//직렬화 버퍼 현 사용 중 사이즈 얻기
	int GetDataSize(void);

	//직렬화 버퍼 시작 포인터 얻기
	unsigned char* GetBufferPtr(void);

	unsigned char* GetReadBufferPtr(void);

	unsigned char* GetWriteBufferPtr(void);

	/*
	직렬화 버퍼 위치 이동, 음수 이동 제외
	큐의 형태임. WritePos가 맨 뒤에 데이터를 추가하는 위치
	*/
	int MoveWritePos(int iSize);

	/*
	직렬화 버퍼 위치 이동, 음수 이동 제외
	큐의 형태. ReadPos는 맨 앞에서 데이터를 읽는 위치
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
	//넣기
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


	//빼기
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
	int m_iBufferSize;// 버퍼의 크기

	//메시지 버퍼가 사용하는 헤더 크기.
	int headerSize;

	int m_iUseDataSize;

	unsigned char* m_chBuffer;	

	DWORD* dwrefCnt;

public:
	static CMemoryPool<CMessageBuffer>* pool; //메모리 풀을 사용하면서 Send Recv TPS가 약 15만씩 증가했음.

	static CMessageBuffer* Alloc();

	static int GetAllocCount();
};

