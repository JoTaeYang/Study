#include <Windows.h>
#include <iostream>
#include "CMemoryPool.h"
#include "CMessageBuffer.h"

CMemoryPool<CMessageBuffer>* CMessageBuffer::pool = new CMemoryPool<CMessageBuffer>(false);
/*
memcpy�� ������? �ƴϸ� ��Ʈ �����ڷ� �����ϴ°� ������?
�������Ϸ� �����ؼ� �ð� ��ŭ �������� ����� �� �� ������?
*/
CMessageBuffer::CMessageBuffer()
{
	m_chBuffer = new unsigned char[eBUFFER_DEFAULT] {0, };

	headerSize = eDEFAULT_HEADER;
	m_iReadPos = eDEFAULT_HEADER;
	m_iWritePos = eDEFAULT_HEADER;

	m_iBufferSize = eBUFFER_DEFAULT;

	m_iUseDataSize = 0;

	dwrefCnt = new DWORD;

	*dwrefCnt = 0;
}

CMessageBuffer::CMessageBuffer(int iBufferSize)
{
	m_chBuffer = new unsigned char[iBufferSize];

	headerSize = eDEFAULT_HEADER;
	m_iReadPos = eDEFAULT_HEADER;
	m_iWritePos = eDEFAULT_HEADER;

	m_iBufferSize = iBufferSize;

	m_iUseDataSize = 0;

	dwrefCnt = new DWORD;
	*dwrefCnt = 0;
}

void CMessageBuffer::LanInit()
{
	headerSize = eLAN_HEADER;
	
	m_iReadPos = headerSize;
	m_iWritePos = headerSize;

	m_iUseDataSize = 0;
}

void CMessageBuffer::Init()
{
	headerSize = eDEFAULT_HEADER;

	m_iReadPos = headerSize;
	m_iWritePos = headerSize;

	m_iUseDataSize = 0;
}

CMessageBuffer::~CMessageBuffer()
{
	m_iReadPos = 0;
	m_iWritePos = 0;

	m_iUseDataSize = 0;
	
	if(m_chBuffer != NULL)
		delete[] m_chBuffer;
	if (dwrefCnt != NULL)
		delete dwrefCnt;

	m_chBuffer = NULL;
}

void CMessageBuffer::Release()
{
	delete[] m_chBuffer;
	m_chBuffer = NULL;
}

void CMessageBuffer::Clear()
{
	m_iReadPos = 0;
	m_iWritePos = 0;

	m_iUseDataSize = 0;
}

int CMessageBuffer::GetBufferSize()
{
	return m_iBufferSize;
}

int CMessageBuffer::GetDataSize()
{
	return m_iWritePos - m_iReadPos;
}

/*
� �뵵�� ������ �𸣱� ������ �׳� m_chBuffer�� return
*/
unsigned char* CMessageBuffer::GetBufferPtr(void)
{
	return m_chBuffer;
}

unsigned char* CMessageBuffer::GetReadBufferPtr(void)
{
	return m_chBuffer + m_iReadPos;
}

unsigned char* CMessageBuffer::GetWriteBufferPtr(void)
{
	return m_chBuffer + m_iWritePos;
}

int CMessageBuffer::GetPacketDataSize()
{
	//�߰��� ������ + ��� ũ�Ⱑ �츮�� ���� ������
	return m_iUseDataSize + headerSize;
}

int CMessageBuffer::MoveWritePos(int iSize)
{
	m_iWritePos += iSize;
	return m_iWritePos;
}

int CMessageBuffer::MoveReadPos(int iSize)
{
	m_iReadPos += iSize;
	return m_iReadPos;
}

int CMessageBuffer::GetReadPos()
{
	return m_iReadPos;
}
int CMessageBuffer::GetWritePos()
{
	return m_iWritePos;
}

//Lan ���� ���
void CMessageBuffer::SetLanHeader()
{
	/*
	LanHeader�� 2byte ���.
	Default 5byte �����ֱ� ������ �̷��� �ᵵ ���� x
	*/
	m_chBuffer[0] = m_iUseDataSize;
	m_chBuffer[1] = m_iUseDataSize >> 8;
}

//5byte ����ϴ� ���
void CMessageBuffer::SetHeader(char* src)
{
	memcpy(m_chBuffer, src, headerSize);
}

//Ŀ���� ���
void CMessageBuffer::SetCustomHeader(char* src, int size)
{
	headerSize = size;
	if (src)
	{
		memcpy(m_chBuffer, src, headerSize);
	}
	m_iReadPos = headerSize;
	m_iWritePos = headerSize;
}

int CMessageBuffer::AddRef()
{
	return InterlockedIncrement(dwrefCnt);
}

/*
���� �� ���� Ƣ��´�
*/
void CMessageBuffer::DecRef()
{
	CMessageBuffer* p = this;

	if (0 == InterlockedDecrement(dwrefCnt))
	{
		pool->Free(&p);
	}		
}

CMessageBuffer* CMessageBuffer::Alloc()
{
	//pool->Alloc();
	CMessageBuffer* tmp = pool->Alloc();

	tmp->Clear();
	tmp->AddRef();

	return tmp;
}

int CMessageBuffer::GetAllocCount()
{
	return pool->GetAllocCount();
}

/*
�ֱ�
*/
CMessageBuffer& CMessageBuffer::operator <<(char chValue)
{
	*(m_chBuffer + m_iWritePos) = chValue;
	m_iWritePos += 1;
	m_iUseDataSize += 1;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator <<(unsigned char uchValue)
{
	*(m_chBuffer + m_iWritePos) = uchValue;
	m_iWritePos += 1;
	m_iUseDataSize += 1;
	return *this;
}


CMessageBuffer& CMessageBuffer::operator <<(short shValue)
{
	memcpy_s((m_chBuffer + m_iWritePos), m_iBufferSize, &shValue, 2);
	m_iWritePos += 2;
	m_iUseDataSize += 2;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator <<(unsigned short ushValue)
{
	memcpy_s((m_chBuffer + m_iWritePos), m_iBufferSize, &ushValue, 2);
	m_iWritePos += 2;
	m_iUseDataSize += 2;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator <<(int iValue)
{
	//memcpy_s((m_chBuffer + m_iWritePos), m_iBufferSize,  &iValue, 4 );
	m_chBuffer[m_iWritePos++] = iValue;
	m_chBuffer[m_iWritePos++] = iValue >> 8;
	m_chBuffer[m_iWritePos++] = iValue >> 16;
	m_chBuffer[m_iWritePos++] = iValue >> 24;
	m_iUseDataSize += 4;
	//m_iWritePos += 4;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator <<(unsigned int uiValue)
{
	memcpy_s((m_chBuffer + m_iWritePos), m_iBufferSize,  &uiValue, 4 );
	m_iWritePos += 4;
	m_iUseDataSize += 4;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator <<(float fValue)
{
	memcpy_s((m_chBuffer + m_iWritePos), m_iBufferSize, &fValue, 4);
	m_iWritePos += 4;
	m_iUseDataSize += 4;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator <<(long ilValue)
{
	memcpy_s((m_chBuffer + m_iWritePos), m_iBufferSize,  &ilValue, 4 );
	m_iWritePos += 4;
	m_iUseDataSize += 4;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator <<(unsigned long uilValue)
{
	memcpy_s((m_chBuffer + m_iWritePos), m_iBufferSize,  &uilValue, 4 );
	m_iWritePos += 4;
	m_iUseDataSize += 4;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator <<(double dValue)
{
	memcpy_s((m_chBuffer + m_iWritePos), m_iBufferSize,  &dValue, 8 );
	m_iWritePos += 8;
	m_iUseDataSize += 4;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator <<(INT64 iValue)
{
	memcpy_s((m_chBuffer + m_iWritePos), m_iBufferSize,  &iValue, 8 );
	m_iWritePos += 8;
	m_iUseDataSize += 8;
	return *this;
}

/*
����
*/


CMessageBuffer& CMessageBuffer::operator>>(char& chValue)
{
	chValue = *(m_chBuffer + m_iReadPos);
	m_iReadPos += 1;
	m_iUseDataSize -= 1;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator >>(unsigned char &uchValue)
{
	uchValue = *(m_chBuffer + m_iReadPos);
	m_iReadPos += 1;
	m_iUseDataSize -= 1;
	return *this;
}


CMessageBuffer& CMessageBuffer::operator >>(short& shValue)
{
	shValue = *(m_chBuffer + m_iReadPos) + (*(m_chBuffer + (m_iReadPos + 1)) << 8);
	m_iReadPos += 2;
	m_iUseDataSize -= 2;
	return *this;
}
CMessageBuffer& CMessageBuffer::operator >>(unsigned short& ushValue)
{
	ushValue = *(m_chBuffer + m_iReadPos) + (*(m_chBuffer + (m_iReadPos + 1)) << 8);
	m_iReadPos += 2;
	m_iUseDataSize -= 2;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator >>(int& iValue)
{
	iValue = *(m_chBuffer + m_iReadPos) + (*(m_chBuffer + (m_iReadPos + 1)) << 8) + 
		(*(m_chBuffer + (m_iReadPos + 2)) << 16) + (*(m_chBuffer + (m_iReadPos + 3)) << 24);
	m_iReadPos += 4;
	m_iUseDataSize -= 4;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator >>(unsigned int& uiValue)
{
	uiValue = *(m_chBuffer + m_iReadPos) + (*(m_chBuffer + (m_iReadPos + 1)) << 8) +
		(*(m_chBuffer + (m_iReadPos + 2)) << 16) + (*(m_chBuffer + (m_iReadPos + 3)) << 24);
	m_iReadPos += 4;
	m_iUseDataSize -= 4;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator >>(float& fValue)
{
	memcpy(&(fValue),  (m_chBuffer + m_iReadPos), 4);
	m_iReadPos += 4;
	m_iUseDataSize -= 4;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator >>(long& ilValue)
{
	ilValue = *(m_chBuffer + m_iReadPos) + (*(m_chBuffer + (m_iReadPos + 1)) << 8) +
		(*(m_chBuffer + (m_iReadPos + 2)) << 16) + (*(m_chBuffer + (m_iReadPos + 3)) << 24);
	m_iReadPos += 4;
	m_iUseDataSize -= 4;
	
	return *this;
}

CMessageBuffer& CMessageBuffer::operator >>(unsigned long& uilValue)
{
	uilValue = *(m_chBuffer + m_iReadPos) + (*(m_chBuffer + (m_iReadPos + 1)) << 8) +
		(*(m_chBuffer + (m_iReadPos + 2)) << 16) + (*(m_chBuffer + (m_iReadPos + 3)) << 24);
	m_iReadPos += 4;
	m_iUseDataSize -= 4;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator >>(double& dValue)
{
	memcpy(&(dValue), (m_chBuffer + m_iReadPos), 8);
	m_iReadPos += 8;
	m_iUseDataSize -= 8;
	return *this;
}
CMessageBuffer& CMessageBuffer::operator >>(INT64& iValue)
{
	memcpy(&(iValue), (m_chBuffer + m_iReadPos), 8);
	m_iReadPos += 8;
	m_iUseDataSize -= 8;
	return *this;
}


int CMessageBuffer::GetData(char* chpDest, int iSize)
{
	memcpy(chpDest, (m_chBuffer + m_iReadPos), iSize);
	m_iReadPos += iSize;
	m_iUseDataSize -= iSize;
	return iSize;
}

int CMessageBuffer::PutData(char* chpSrc, int iSrcSize)
{
	memcpy(m_chBuffer + m_iWritePos, chpSrc, iSrcSize);
	m_iWritePos += iSrcSize;
	m_iUseDataSize += iSrcSize;
	return iSrcSize;
}