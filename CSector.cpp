
#include <WinSock2.h>
#include "Common.h"
#include "stSESSION.h"
#include "CSector.h"


void CSector::SectorAddCharacter(CCharacter* pCharacter)
{
	short shX = pCharacter->shX / dfSECTOR_WIDTH;
	short shY = pCharacter->shY / dfSECTOR_HEIGHT;

	sectors[shY][shX].push_back(pCharacter);
}

void CSector::SectorRemoveCharacter(CCharacter* pCharacter)
{
	//short shX = pCharacter->OldSectorPos.iX;// dfSECTOR_WIDTH;
	//short shY = pCharacter->OldSectorPos.iY; // dfSECTOR_HEIGHT;
	short curshX = pCharacter->CurSectorPos.iX;
	short curshY = pCharacter->CurSectorPos.iY;

//	sectors[shY][shX].remove(pCharacter);
	sectors[curshY][curshX].remove(pCharacter);

}

void CSector::GetSectorArounds(int iX, int iY, st_SECTOR_AROUND* around)
{
	//�迭�̶� -1�� ���� ���� �ƴ�.
	//����� ���ϰ� �ϱ� ������. 3*3�� ���Ҷ� ���� ���ڷ� ���޹��� ��ǥ�� ��� ��ǥ.
	//������ 1�� ���� �»�� ���� ����� �� �־� �������� �������.
	iX--;
	iY--;

	//around->iCnt = 0;
	for (int iCntY = 0; iCntY < 3; iCntY++)
	{
		if (iY + iCntY < 0 || iY + iCntY >= dfSECTOR_MAX_Y)
			continue;
		for (int iCntX = 0; iCntX < 3; iCntX++)
		{
			if (iX + iCntX < 0 || iX + iCntX >= dfSECTOR_MAX_X)
				continue;
			else
			{
				around->arounds[around->iCnt].iX = iX + iCntX;
				around->arounds[around->iCnt].iY = iY + iCntY;
				around->iCnt++;
			}
		}
	}
}

void CSector::GetSectorIterBeginEnd(int iX, int iY, std::list<CCharacter*>::iterator& begin, std::list<CCharacter*>::iterator& end)
{
	begin = sectors[iY][iX].begin();
	end = sectors[iY][iX].end();
}

bool CSector::SectorUpdateCheck(CCharacter* pCharacter)
{
	short sectorX = pCharacter->shX / dfSECTOR_WIDTH;
	short sectorY = pCharacter->shY / dfSECTOR_HEIGHT;

	pCharacter->OldSectorPos.iX = pCharacter->CurSectorPos.iX;
	pCharacter->OldSectorPos.iY = pCharacter->CurSectorPos.iY;

	if (sectorX == pCharacter->CurSectorPos.iX && sectorY == pCharacter->CurSectorPos.iY)
		return false;

	SectorRemoveCharacter(pCharacter);

	pCharacter->CurSectorPos.iX = sectorX;
	pCharacter->CurSectorPos.iY = sectorY;

	SectorAddCharacter(pCharacter);

	return true;
}

void CSector::GetSectorUpdateAround(CCharacter* pCharacter, st_SECTOR_AROUND* removeAround, st_SECTOR_AROUND* addAround)
{
	st_SECTOR_AROUND oldSectorAround, curSectorAround;

	bool isCheck;

	GetSectorArounds(pCharacter->CurSectorPos.iX, pCharacter->CurSectorPos.iY, &curSectorAround);
	GetSectorArounds(pCharacter->OldSectorPos.iX, pCharacter->OldSectorPos.iY, &oldSectorAround);

	//���� ���� ���� �� ���� ���Ϳ� ���� ���͸� ã�� �� ĳ���͸� �����ַ��� ��
	for (int iCntOld = 0; iCntOld < oldSectorAround.iCnt; iCntOld++)
	{
		isCheck = false;
		for (int iCntCur = 0; iCntCur < curSectorAround.iCnt; iCntCur++)
		{
			if (oldSectorAround.arounds[iCntOld].iX == curSectorAround.arounds[iCntCur].iX &&
				oldSectorAround.arounds[iCntOld].iY == curSectorAround.arounds[iCntCur].iY)
			{
				isCheck = true;
				break;
			}
		}
		if (!isCheck)
		{
			removeAround->arounds[removeAround->iCnt].iX = oldSectorAround.arounds[iCntOld].iX;
			removeAround->arounds[removeAround->iCnt].iY = oldSectorAround.arounds[iCntOld].iY;
			removeAround->iCnt++;
		}
	}

	for (int iCntCur = 0; iCntCur < curSectorAround.iCnt; iCntCur++)
	{
		isCheck = false;
		for (int iCntOld = 0; iCntOld < oldSectorAround.iCnt; iCntOld++)
		{
			if (oldSectorAround.arounds[iCntOld].iX == curSectorAround.arounds[iCntCur].iX &&
				oldSectorAround.arounds[iCntOld].iY == curSectorAround.arounds[iCntCur].iY)
			{
				isCheck = true;
				break;
			}
		}
		if (!isCheck)
		{
			addAround->arounds[addAround->iCnt].iX = curSectorAround.arounds[iCntCur].iX;
			addAround->arounds[addAround->iCnt].iY = curSectorAround.arounds[iCntCur].iY;
			addAround->iCnt++;
		}
	}
}