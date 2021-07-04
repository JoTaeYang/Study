#pragma once

template <typename Type>
class CLockFreeStack
{
private:
	struct stNODE
	{
		stNODE()
		{
			pNext = NULL;

		}

		stNODE* pNext;
		Type Data;
	};
	struct TopNode
	{
		TopNode()
		{
			pNext = NULL;
			unique = 0;
		}

		stNODE* pNext;
		__int64 unique;
	};
public:
	CLockFreeStack();

	~CLockFreeStack();

	void Push(Type data);

	bool Pop(Type& out);

	int GetCount();
	int GetAllocCount();
	
private:
	CMemoryPool<stNODE>* m_pool;

	TopNode* _pTop;
	long _Count;

	__int64 uniqueValue;

private:
	bool CountDecrement();

	void Clear();
};

template<typename Type>
CLockFreeStack<Type>::CLockFreeStack()
{
	_pTop = new TopNode;
	m_pool = new CMemoryPool<stNODE>(false);
	_Count = 0;
}

template<typename Type>
CLockFreeStack<Type>::~CLockFreeStack()
{
	Clear();
	delete _pTop;
	delete m_pool;
}

template<typename Type>
void CLockFreeStack<Type>::Push(Type data)
{
	stNODE* pNode = m_pool->Alloc();
	TopNode tmpTop;

	__int64 tmpUniq = InterlockedIncrement64(&uniqueValue);

	pNode->Data = data;
	
	do
	{
		tmpTop.unique = _pTop->unique;
		tmpTop.pNext = _pTop->pNext;
		pNode->pNext = tmpTop.pNext;

	} while (InterlockedCompareExchange128((long long*)_pTop, tmpUniq, (long long)pNode, (long long*)&tmpTop) == 0 );
	
	InterlockedIncrement(&_Count);
}

template<typename Type>
bool CLockFreeStack<Type>::Pop(Type& out)
{
	stNODE* tmpNext = NULL;
	stNODE* freeNode = NULL;
	TopNode tmpTop;
	__int64 tmpUniq = InterlockedIncrement64(&uniqueValue);
	if (!CountDecrement()) // 스택이 비어서 실패.
	{		
		out = NULL;
		return false;
	}
	else
	{
		do
		{
			tmpTop.unique = _pTop->unique;
			tmpTop.pNext = _pTop->pNext;
			
			if (!tmpTop.pNext)
			{				
				out = NULL;
				return false;
			}
		} while (InterlockedCompareExchange128((long long*)_pTop, tmpUniq, (long long)tmpTop.pNext->pNext, (long long *)&tmpTop) == 0);
		out = tmpTop.pNext->Data;
		freeNode = tmpTop.pNext;
		m_pool->Free(&freeNode);
	}
	return true;
}

template <typename Type>
int CLockFreeStack<Type>::GetCount()
{
	return _Count;
}

template <typename Type>
int CLockFreeStack<Type>::GetAllocCount()
{
	return m_pool->GetAllocCount();
}

template <typename Type>
bool CLockFreeStack<Type>::CountDecrement()
{
	if (InterlockedDecrement(&_Count) < 0) // 스택이 비어서 실패.
	{
		InterlockedIncrement(&_Count);
		return false;
	}
	return true;
}

template <typename Type>
void CLockFreeStack<Type>::Clear()
{
	stNODE* tmp = _pTop->pNext;
	stNODE* next = NULL;
	while (tmp)
	{
		next = tmp->pNext;
		m_pool->Free(&tmp);
		tmp = next;

		InterlockedDecrement(&_Count);
	}
}