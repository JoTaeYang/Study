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
	넘어온 좌표를 기준으로 최대 9개의 섹터를 인자로 전달한 st_SECTOR_AROUND 구조체 변수로 받음
	*/
	void GetSectorArounds(int iX, int iY, st_SECTOR_AROUND* around);

	void GetSectorIterBeginEnd(int iX, int iY, std::list<CCharacter*>::iterator &begin, std::list<CCharacter*>::iterator& end);
	
	bool SectorUpdateCheck(CCharacter* pCharacter);

	void GetSectorUpdateAround(CCharacter* pCharacter, st_SECTOR_AROUND* removeAround, st_SECTOR_AROUND* addAround);
};