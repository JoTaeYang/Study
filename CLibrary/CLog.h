#pragma once

class CLog
{
public:
	CLog(const WCHAR* LogFile = L"LogFile");
	~CLog();

	bool PrintLog(const WCHAR* log, int Line,int err_num);



	void SetLogLevel(int levelNo);
private:
	WCHAR szFileName[128];
	WCHAR szLogBuff[1024];

	int _ilogLevel;
};