#include <iostream>
#include <Windows.h>
#include <strsafe.h>
#include <time.h>
#include "C:\ProLib\CRingBuffer.h"



CRingBuffer::CRingBuffer()
{
	_buffer = new char[e_DEFAULT_SIZE];
	_iFrontPos = 0;
	_iRearPos = 0;
}
CRingBuffer::~CRingBuffer()
{
	_iFrontPos = 0;
	_iRearPos = 0;
	delete[] _buffer;
}

int CRingBuffer::GetBufferSize(void)
{
	return e_DEFAULT_SIZE;
}

int CRingBuffer::GetUseSize(void)
{
	int tmpRear = _iRearPos;
	int tmpFront = _iFrontPos;
	if (tmpRear >= tmpFront)
		return tmpRear - tmpFront;
	else
		return (e_DEFAULT_SIZE - tmpFront) + tmpRear;
}

int CRingBuffer::GetFreeSize(void)
{
	if (_iFrontPos > _iRearPos)
		return _iFrontPos - _iRearPos;
	else
		return e_DEFAULT_SIZE - (_iRearPos - _iFrontPos);

}

int CRingBuffer::DirectEnqueueSize(void)
{
	int tmpRear = _iRearPos;
	int tmpFront = _iFrontPos;
	if (tmpRear >= tmpFront)
		return (e_DEFAULT_SIZE - tmpRear);
	else
		return tmpFront - tmpRear;
}

int CRingBuffer::DirectDequeueSize(void)
{
	int tmpRear = _iRearPos;
	int tmpFront = _iFrontPos;
	if (tmpRear >= tmpFront)
		return _iRearPos - _iFrontPos;
	else
		return e_DEFAULT_SIZE - _iFrontPos;
}

//실제 나중에 네트워크를 생각하면 크기가 크다고 안받으면 안될 듯.
//enqueue를 하는데 공간이 없다? 그러면 바로 로그 찍어서 남겨보기.
int CRingBuffer::Enqueue(char* chpData, int iSize)
{
	int ret_val = 0;
	int tmp_RearPos = _iRearPos;
	int tmp_FrontPos = _iFrontPos;

	while (iSize > 0)
	{
		if ((tmp_RearPos + 1) % e_DEFAULT_SIZE == tmp_FrontPos)
		{
			//로그 찍기, 데이터 넣다가 공간이 부족해진 순간 or 데이터를 넣기 전에 이미 꽉 찬 순간.
			//강사님처럼 ring buffer보다 큰 데이터를 넣으려고 할 때 프로세스 종료.시키기.후에.
			break;
		}
		
		_buffer[tmp_RearPos] = *chpData;
		tmp_RearPos = (tmp_RearPos + 1) % e_DEFAULT_SIZE;

		chpData++;
		iSize--;
		ret_val++;
	}
	_iRearPos = tmp_RearPos;
	return ret_val;
}
//dequeue는 뽑을 수 있는 크기만큼 만 뽑는다.
int CRingBuffer::Dequeue(char* chpDest, int iSize)
{
	int ret_val = iSize;
	int orgSize = iSize;

	bool circleCheck = false;

	int orgFrontPos = _iFrontPos;
	int tmpFrontPos = _iFrontPos;
	int tmpRearPos = _iRearPos;
	//int tmp
	int Count = 0;


	//PRO_BEGIN(L"Dequeue");
	//지금 뽑으려는 데이터가 들어있는 데이터보다 클 때
	//근데 이 상황에서 front가 end를 돌파해 시작점으로 갈 때..
	//현재 들어있는 데이터보다 많이 뽑아내려고 한다면 어떻게 해야하지?
	while (iSize > 0)
	{
		if (tmpFrontPos == tmpRearPos) //front와 rear가 만남. 배열이 비었다.
		{
			//여기서도 한바퀴 돌아서 만난 걸 수도 있네
			if (circleCheck == true)
			{
				//0부터 tmpPos까지.				
				int endSpace = e_DEFAULT_SIZE - orgFrontPos;
				memcpy(chpDest, (_buffer + orgFrontPos), endSpace);//front - end까지
				memcpy((chpDest + endSpace), _buffer, tmpRearPos);//지금 이 조건에 들어왔다는 것은 iSize만큼 데이터를 읽기 전에, iRearPos와 만나서 배열이 텅텅된 것.그래서 _iRearPos까지만 받으면 된다고 현재는 생각.
			}
			else
			{
				memcpy(chpDest, (_buffer + orgFrontPos), Count);//여기선 그냥 넣어주면 될 듯.
			}
			_iFrontPos = tmpFrontPos;
			//PRO_END(L"Dequeue");
			return Count;
		}
		tmpFrontPos = (tmpFrontPos + 1) % e_DEFAULT_SIZE;

		//한바퀴 돈 것을 알려면 tmpPos가 0이되는 순간임.
		if (tmpFrontPos == 0)
			circleCheck = true;

		iSize--;
		Count++;
	}
	if (circleCheck == true)//한바퀴를 돌았는데 데이터를 다 Dequeue할 수 있는 경우
	{
		int endSpace = e_DEFAULT_SIZE - orgFrontPos;
		memcpy(chpDest, (_buffer + orgFrontPos), endSpace);//front - end까지
		memcpy((chpDest + endSpace), _buffer, orgSize - endSpace);
	}
	else //dequeue iSize만큼 할 수 있는데 한 바퀴를 돌지 않은 경우
	{
		memcpy(chpDest, (_buffer + orgFrontPos), Count);
	}

	_iFrontPos = tmpFrontPos;
	//PRO_END(L"Dequeue");
	return Count;

}

int CRingBuffer::Peek(char* chpDest, int iSize)
{
	int ret_val = iSize;
	int orgSize = iSize;

	bool circleCheck = false;
	int endSpace = e_DEFAULT_SIZE - _iFrontPos;
	int tmpPos = _iFrontPos;
	int Count = 0;
	//if (_iFrontPos == _iRearPos) //비었을 때
	//	return 0;

	//지금 뽑으려는 데이터가 들어있는 데이터보다 클 때
	//근데 이 상황에서 front가 end를 돌파해 시작점으로 갈 때..
	//현재 들어있는 데이터보다 많이 뽑아내려고 한다면 어떻게 해야하지?
	while (iSize > 0)
	{
		if (tmpPos == _iRearPos) //front와 rear가 만남. 배열이 비었다.
		{
			//여기서도 한바퀴 돌아서 만난 걸 수도 있네
			if (circleCheck == true)
			{
				//0부터 tmpPos까지.				
				int endSpace = e_DEFAULT_SIZE - _iFrontPos;
				memcpy(chpDest, (_buffer + _iFrontPos), endSpace);//front - end까지
				memcpy((chpDest + endSpace), _buffer, _iRearPos);//지금 이 조건에 들어왔다는 것은 iSize만큼 데이터를 읽기 전에, iRearPos와 만나서 배열이 텅텅된 것.그래서 _iRearPos까지만 받으면 된다고 현재는 생각.
			}
			else
			{
				memcpy(chpDest, (_buffer + _iFrontPos), Count);//여기선 그냥 넣어주면 될 듯.
			}
			return Count; //그리고 넣은 데이터 만큼 return 해주기;			
		}
		tmpPos = (tmpPos + 1) % e_DEFAULT_SIZE;

		//한바퀴 돈 것을 알려면 tmpPos가 0이되는 순간임.
		if (tmpPos == 0)
			circleCheck = true;

		iSize--;
		Count++;
	}

	if (circleCheck == true)//한바퀴를 돌았는데 데이터를 다 Dequeue할 수 있는 경우
	{
		
		memcpy(chpDest, (_buffer + _iFrontPos), endSpace);//front - end까지
		memcpy((chpDest + endSpace), _buffer, orgSize - endSpace);
	}
	else //dequeue iSize만큼 할 수 있는데 한 바퀴를 돌지 않은 경우
	{
		memcpy(chpDest, (_buffer + _iFrontPos), Count);
	}

	return Count;
}

int CRingBuffer::Peek(char* chpDest, int iPosCount, int iSize)
{
	int iret_cnt = 0;
	int orgSize = iSize;
	int tmpFrontPos = _iFrontPos;
	int tmpRearPos = _iRearPos;
	bool circleCheck = false;

	int iCnt = 0;

	//if (((tmpFrontPos) + (iPosCount * 8)) % e_DEFAULT_SIZE > _iRearPos)
	//	return 0;

	tmpFrontPos = _iFrontPos + (iPosCount * iSize);
	int tmpCntFrontPos = tmpFrontPos;
	while (iSize > 0)
	{
		if (tmpCntFrontPos == _iRearPos) //front와 rear가 만남. 배열이 비었다.
		{
			//여기서도 한바퀴 돌아서 만난 걸 수도 있네
			if (circleCheck == true)
			{
				//0부터 tmpPos까지.				
				int endSpace = e_DEFAULT_SIZE - tmpFrontPos;
				memcpy(chpDest, (_buffer + tmpFrontPos), endSpace);//front - end까지
				memcpy((chpDest + endSpace), _buffer, _iRearPos);//지금 이 조건에 들어왔다는 것은 iSize만큼 데이터를 읽기 전에, iRearPos와 만나서 배열이 텅텅된 것.그래서 _iRearPos까지만 받으면 된다고 현재는 생각.
			}
			else
			{
				memcpy(chpDest, (_buffer + tmpFrontPos), iCnt);//여기선 그냥 넣어주면 될 듯.
			}
			return iCnt; //그리고 넣은 데이터 만큼 return 해주기;			
		}
		tmpCntFrontPos = (tmpCntFrontPos + 1) % e_DEFAULT_SIZE;

		//한바퀴 돈 것을 알려면 tmpPos가 0이되는 순간임.
		if (tmpCntFrontPos == 0)
			circleCheck = true;

		iSize--;
		iCnt++;
	}

	if (circleCheck == true)//한바퀴를 돌았는데 데이터를 다 Dequeue할 수 있는 경우
	{
		int endSpace = e_DEFAULT_SIZE - tmpFrontPos;
		memcpy(chpDest, (_buffer + tmpFrontPos), endSpace);//front - end까지
		memcpy((chpDest + endSpace), _buffer, orgSize - endSpace);
	}
	else //dequeue iSize만큼 할 수 있는데 한 바퀴를 돌지 않은 경우
	{
		memcpy(chpDest, (_buffer + tmpFrontPos), iCnt);
	}

	return iCnt;
}

char* CRingBuffer::GetFrontBufferPtr(void)
{
	return &_buffer[_iFrontPos];
}
char* CRingBuffer::GetRearBufferPtr(void)
{
	return &_buffer[_iRearPos];
}

char* CRingBuffer::GetStartBufferPtr(void)
{
	return _buffer;
}

int CRingBuffer::MoveRear(int iSize)
{
	int tmpRear = _iRearPos;
	tmpRear = (tmpRear + iSize) % e_DEFAULT_SIZE;

	_iRearPos = tmpRear;
	return _iRearPos;
}

int CRingBuffer::MoveFront(int iSize)
{
	int tmpFront = _iFrontPos;
	_iFrontPos = (_iFrontPos + iSize) % e_DEFAULT_SIZE;

	//_iFrontPos = tmpFront;
	return _iFrontPos;
}

void CRingBuffer::Clear()
{
	_iFrontPos = 0;
	_iRearPos = 0;
}

int CRingBuffer::GetTestRearPoint()
{
	return _iRearPos;
 }

int CRingBuffer::GetTestFrontPoint()
{
	return _iFrontPos;
}