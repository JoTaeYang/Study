#pragma once




class CCharacter;

class CSector
{
private:
	std::list<CCharacter*> sectors[dfSECTOR_MAX_Y][dfSECTOR_MAX_X];
	
public:
	void SectorAddCharacter(CCharacter* pCharacter);
	void SectorRemoveCharacter(CCharacter* pCharacter);


	/*
	�Ѿ�� ��ǥ�� �������� �ִ� 9���� ���͸� ���ڷ� ������ st_SECTOR_AROUND ����ü ������ ����
	*/
	void GetSectorArounds(int iX, int iY, st_SECTOR_AROUND* around);

	void GetSectorIterBeginEnd(int iX, int iY, std::list<CCharacter*>::iterator &begin, std::list<CCharacter*>::iterator& end);
	
	bool SectorUpdateCheck(CCharacter* pCharacter);

	void GetSectorUpdateAround(CCharacter* pCharacter, st_SECTOR_AROUND* removeAround, st_SECTOR_AROUND* addAround);
};