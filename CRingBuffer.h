#pragma once

class  CRingBuffer
{
private:
	enum {
		e_DEFAULT_SIZE = 4800
	};
public:
	CRingBuffer(void);
	~CRingBuffer();

	//void Resize(int size);
	int GetUseSize(void);

	int GetBufferSize(void); // ��¥ �� �״�� �迭 ũ��

	int GetFreeSize(void);

	int DirectEnqueueSize(void);
	int DirectDequeueSize(void);

	int Enqueue(char* chpData, int iSize);

	int Dequeue(char* chpDest, int iSize);

	int Peek(char* chpDest, int iSize);
	int Peek(char* chpDest, int iPosCount, int iSize);

	char* GetFrontBufferPtr(void);
	char* GetRearBufferPtr(void);
	char* GetStartBufferPtr(void);

	int MoveRear(int iSize);
	int MoveFront(int iSize);

	int GetTestRearPoint();
	int GetTestFrontPoint();

	void Clear();

private:
	char* _buffer;

	int _iFrontPos;
	int _iRearPos;
};

