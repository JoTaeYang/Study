#pragma once

struct stMEMORYLOG
{
	__int64 addr;	
	long id;
	BYTE jobNum;
	BYTE _count;
	__int64 nodeAddr;
};


template <typename T>
class CLockFreeQueue
{
private:
	struct stNODE
	{
		stNODE()
		{
			pNext = NULL;
		}
		stNODE* pNext;
		T data;
	};

	struct stUniqNODE
	{
		stUniqNODE()
		{
			node = NULL;
			unique = 0;
		}

		stNODE* node;
		__int64 unique;
	};

public:
	CLockFreeQueue();

	~CLockFreeQueue();

	void Enqueue(T data);

	void Dequeue(T& data);

	int Peek(T* data, int _pos);

	int GetCount();

	int GetAllocCount();

	void log(int jobNum, long id, __int64 addr, __int64 nodeAddr = 0);
private:

	CMemoryPool<stNODE>* _pool;

	stUniqNODE* _pHead;
	stUniqNODE* _pTail;

	long _count;

	__int64 _uniqueValue;

	//stMEMORYLOG* memLog;
	long memlogCount;

	
};

template<typename T>
void CLockFreeQueue<T>::log(int jobNum, long id, __int64 addr, __int64 nodeAddr)
{
	//int tmp = InterlockedIncrement(&memlogCount);
	//memLog[tmp].addr = addr;
	//memLog[tmp].nodeAddr = nodeAddr;
	//memLog[tmp].id = id;
	//memLog[tmp].jobNum = jobNum;
	//memLog[tmp]._count = _count;
}

template<typename T>
CLockFreeQueue<T>::CLockFreeQueue()
{
	_pHead = new stUniqNODE;

	_pTail = new stUniqNODE;

	_pool = new CMemoryPool<stNODE>(false);

	//memLog = new stMEMORYLOG[50000];
	_count = 0;
	_uniqueValue = 0;

	_pHead->node = _pool->Alloc(); //�Ȱ��� ���� ��带 ����Ű�� �ȴ�.
	_pTail->node = _pHead->node;
}

template<typename T>
CLockFreeQueue<T>::~CLockFreeQueue()
{

	delete _pHead;

	delete _pTail;

	delete _pool;
}

template<typename T>
void CLockFreeQueue<T>::Enqueue(T data)
{
	//log(1, GetCurrentThreadId(), 0); //In Enqueue
	
	stUniqNODE tmpNode;
	stNODE* pNode = _pool->Alloc();
	pNode->data = data;
	
	pNode->pNext = NULL;

	__int64 tmpValue = InterlockedIncrement64(&_uniqueValue);
	//log(11, GetCurrentThreadId(), (long long)pNode);
	while(1)
	{
		tmpNode.node = _pTail->node;
		tmpNode.unique = _pTail->unique;
		
		if (tmpNode.node->pNext == NULL)
		{
			//���濡 �����ؼ� NULL�� ������ �ȴٸ� ���� ����.
			//Next�� �� ���� �Ŵ� ��
			if (InterlockedCompareExchangePointer((PVOID*)&tmpNode.node->pNext, pNode, nullptr) == NULL)
			{		
				//log(3, GetCurrentThreadId(), (long long)pNode);
				
				if (InterlockedCompareExchange128((long long*)_pTail, tmpValue, (long long)tmpNode.node->pNext, (long long*)&tmpNode) == 1)
				{
					//log(4, GetCurrentThreadId(), (long long)pNode, (long long)tmpNode.node);
				}
				break;
			}
		}
		else
		{
			InterlockedCompareExchange128((long long*)_pTail, tmpValue, (long long)tmpNode.node->pNext, (long long*)&tmpNode);
		}			
	}
	InterlockedIncrement(&_count);
}

template<typename T>
void CLockFreeQueue<T>::Dequeue(T& data)
{	
	stUniqNODE tHeadNode;
	stUniqNODE tTailNode;
	stNODE* tmpHead = NULL;
	stNODE* freeNode = NULL;
	
	if (InterlockedDecrement(&_count) < 0)
	{
		InterlockedIncrement(&_count);
		data = NULL;
		//printf("Hello Count zERO\n");
		return;
	}

	volatile __int64 tmpValue = InterlockedIncrement64(&_uniqueValue);

	while (1)
	{
		tHeadNode.node = _pHead->node;
		tHeadNode.unique = _pHead->unique;
		tTailNode.node = _pTail->node;
		tTailNode.unique = _pTail->unique;		

		/*if (tTailNode.node->pNext == tHeadNode.node)
		{
			int* ptr = NULL;
			*ptr = 10;
		}*/
		if (tTailNode.node->pNext == NULL) //���� : Head�� Tail�� ���� ���� �ٶ󺸰� �ֱ� ������ üũ�� ������ ������ Next�� NULL�̿��� ������ �ȴ�
		{			
			
			if (tHeadNode.node->pNext)//LockFreeQ�� ���� ã�� 5-1-1�� �ذ�å.
			{
				data = tHeadNode.node->pNext->data;
				freeNode = tHeadNode.node;
				
				if (InterlockedCompareExchange128((long long*)_pHead, tmpValue, (long long)tHeadNode.node->pNext, (long long*)&tHeadNode) == 1)
				{
					_pool->Free(&freeNode);
					break;
				}
			}
		}
		else
		{			
			//log(12, GetCurrentThreadId(), (long long)tTailNode.node->pNext->data);
			int t = InterlockedCompareExchange128((long long*)_pTail, tmpValue, (long long)tTailNode.node->pNext, (long long*)&tTailNode);
			//log(6, GetCurrentThreadId(),t);
		}
	}
}

template<typename T>
int CLockFreeQueue<T>::Peek(T* data, int _pos)
{
	//�� ���� ������ ��û �� -1 ��ȯ.
	if (_pos > _count || !data)
		return -1;

	stUniqNODE tmpHead;
	tmpHead.unique = _pHead->unique;
	tmpHead.node = _pHead->node;

	stNODE* tail = _pTail->node;
	stNODE* iter = tmpHead.node->pNext;
	for (int i = 0; i < _pos; i++)
	{
		if (iter == NULL || iter == tail->pNext)
			return -1;
		iter = iter->pNext;
	}
	if (iter == NULL)
		return -1;

	if (tmpHead.unique == _pHead->unique)
		*data = iter->data;

	return _pos;
}

template<typename T>
int CLockFreeQueue<T>::GetCount()
{
	return _count;
}

template<typename T>
int CLockFreeQueue<T>::GetAllocCount()
{
	return _pool->GetAllocCount();
}
