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

//���� ���߿� ��Ʈ��ũ�� �����ϸ� ũ�Ⱑ ũ�ٰ� �ȹ����� �ȵ� ��.
//enqueue�� �ϴµ� ������ ����? �׷��� �ٷ� �α� �� ���ܺ���.
int CRingBuffer::Enqueue(char* chpData, int iSize)
{
	int ret_val = 0;
	int tmp_RearPos = _iRearPos;
	int tmp_FrontPos = _iFrontPos;

	while (iSize > 0)
	{
		if ((tmp_RearPos + 1) % e_DEFAULT_SIZE == tmp_FrontPos)
		{
			//�α� ���, ������ �ִٰ� ������ �������� ���� or �����͸� �ֱ� ���� �̹� �� �� ����.
			//�����ó�� ring buffer���� ū �����͸� �������� �� �� ���μ��� ����.��Ű��.�Ŀ�.
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
//dequeue�� ���� �� �ִ� ũ�⸸ŭ �� �̴´�.
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
	//���� �������� �����Ͱ� ����ִ� �����ͺ��� Ŭ ��
	//�ٵ� �� ��Ȳ���� front�� end�� ������ ���������� �� ��..
	//���� ����ִ� �����ͺ��� ���� �̾Ƴ����� �Ѵٸ� ��� �ؾ�����?
	while (iSize > 0)
	{
		if (tmpFrontPos == tmpRearPos) //front�� rear�� ����. �迭�� �����.
		{
			//���⼭�� �ѹ��� ���Ƽ� ���� �� ���� �ֳ�
			if (circleCheck == true)
			{
				//0���� tmpPos����.				
				int endSpace = e_DEFAULT_SIZE - orgFrontPos;
				memcpy(chpDest, (_buffer + orgFrontPos), endSpace);//front - end����
				memcpy((chpDest + endSpace), _buffer, tmpRearPos);//���� �� ���ǿ� ���Դٴ� ���� iSize��ŭ �����͸� �б� ����, iRearPos�� ������ �迭�� ���ֵ� ��.�׷��� _iRearPos������ ������ �ȴٰ� ����� ����.
			}
			else
			{
				memcpy(chpDest, (_buffer + orgFrontPos), Count);//���⼱ �׳� �־��ָ� �� ��.
			}
			_iFrontPos = tmpFrontPos;
			//PRO_END(L"Dequeue");
			return Count;
		}
		tmpFrontPos = (tmpFrontPos + 1) % e_DEFAULT_SIZE;

		//�ѹ��� �� ���� �˷��� tmpPos�� 0�̵Ǵ� ������.
		if (tmpFrontPos == 0)
			circleCheck = true;

		iSize--;
		Count++;
	}
	if (circleCheck == true)//�ѹ����� ���Ҵµ� �����͸� �� Dequeue�� �� �ִ� ���
	{
		int endSpace = e_DEFAULT_SIZE - orgFrontPos;
		memcpy(chpDest, (_buffer + orgFrontPos), endSpace);//front - end����
		memcpy((chpDest + endSpace), _buffer, orgSize - endSpace);
	}
	else //dequeue iSize��ŭ �� �� �ִµ� �� ������ ���� ���� ���
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
	//if (_iFrontPos == _iRearPos) //����� ��
	//	return 0;

	//���� �������� �����Ͱ� ����ִ� �����ͺ��� Ŭ ��
	//�ٵ� �� ��Ȳ���� front�� end�� ������ ���������� �� ��..
	//���� ����ִ� �����ͺ��� ���� �̾Ƴ����� �Ѵٸ� ��� �ؾ�����?
	while (iSize > 0)
	{
		if (tmpPos == _iRearPos) //front�� rear�� ����. �迭�� �����.
		{
			//���⼭�� �ѹ��� ���Ƽ� ���� �� ���� �ֳ�
			if (circleCheck == true)
			{
				//0���� tmpPos����.				
				int endSpace = e_DEFAULT_SIZE - _iFrontPos;
				memcpy(chpDest, (_buffer + _iFrontPos), endSpace);//front - end����
				memcpy((chpDest + endSpace), _buffer, _iRearPos);//���� �� ���ǿ� ���Դٴ� ���� iSize��ŭ �����͸� �б� ����, iRearPos�� ������ �迭�� ���ֵ� ��.�׷��� _iRearPos������ ������ �ȴٰ� ����� ����.
			}
			else
			{
				memcpy(chpDest, (_buffer + _iFrontPos), Count);//���⼱ �׳� �־��ָ� �� ��.
			}
			return Count; //�׸��� ���� ������ ��ŭ return ���ֱ�;			
		}
		tmpPos = (tmpPos + 1) % e_DEFAULT_SIZE;

		//�ѹ��� �� ���� �˷��� tmpPos�� 0�̵Ǵ� ������.
		if (tmpPos == 0)
			circleCheck = true;

		iSize--;
		Count++;
	}

	if (circleCheck == true)//�ѹ����� ���Ҵµ� �����͸� �� Dequeue�� �� �ִ� ���
	{
		
		memcpy(chpDest, (_buffer + _iFrontPos), endSpace);//front - end����
		memcpy((chpDest + endSpace), _buffer, orgSize - endSpace);
	}
	else //dequeue iSize��ŭ �� �� �ִµ� �� ������ ���� ���� ���
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
		if (tmpCntFrontPos == _iRearPos) //front�� rear�� ����. �迭�� �����.
		{
			//���⼭�� �ѹ��� ���Ƽ� ���� �� ���� �ֳ�
			if (circleCheck == true)
			{
				//0���� tmpPos����.				
				int endSpace = e_DEFAULT_SIZE - tmpFrontPos;
				memcpy(chpDest, (_buffer + tmpFrontPos), endSpace);//front - end����
				memcpy((chpDest + endSpace), _buffer, _iRearPos);//���� �� ���ǿ� ���Դٴ� ���� iSize��ŭ �����͸� �б� ����, iRearPos�� ������ �迭�� ���ֵ� ��.�׷��� _iRearPos������ ������ �ȴٰ� ����� ����.
			}
			else
			{
				memcpy(chpDest, (_buffer + tmpFrontPos), iCnt);//���⼱ �׳� �־��ָ� �� ��.
			}
			return iCnt; //�׸��� ���� ������ ��ŭ return ���ֱ�;			
		}
		tmpCntFrontPos = (tmpCntFrontPos + 1) % e_DEFAULT_SIZE;

		//�ѹ��� �� ���� �˷��� tmpPos�� 0�̵Ǵ� ������.
		if (tmpCntFrontPos == 0)
			circleCheck = true;

		iSize--;
		iCnt++;
	}

	if (circleCheck == true)//�ѹ����� ���Ҵµ� �����͸� �� Dequeue�� �� �ִ� ���
	{
		int endSpace = e_DEFAULT_SIZE - tmpFrontPos;
		memcpy(chpDest, (_buffer + tmpFrontPos), endSpace);//front - end����
		memcpy((chpDest + endSpace), _buffer, orgSize - endSpace);
	}
	else //dequeue iSize��ŭ �� �� �ִµ� �� ������ ���� ���� ���
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