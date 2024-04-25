
#include <WinSock2.h>
#include "Protocol.h"
#include "Common.h"
#include "stSESSION.h"



void CCharacter::InitPlayer(__int64 Key, DWORD Action, BYTE dir, BYTE moveDir, short _x, short _y, char hp)
{
	this->SessionKey = Key;
	this->dwAction = Action;
	this->byDirection = dir;
	this->byMoveDirection = moveDir;
	this->shX = _x;
	this->shY = _y;
	this->chHP = hp;

	this->CurSectorPos.iX = _x / dfSECTOR_WIDTH;
	this->CurSectorPos.iY = _y / dfSECTOR_HEIGHT;
	this->OldSectorPos.iX = CurSectorPos.iX;
	this->OldSectorPos.iY = CurSectorPos.iY;

}

bool CCharacter::CompareXY(short shX, short shY)
{
	if (abs(this->shX - shX) > dfERROR_RANGE || abs(this->shY - shY) > dfERROR_RANGE)
		return false;
	return true;
}