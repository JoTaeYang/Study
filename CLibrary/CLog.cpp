#include <Windows.h>
#include <stdio.h>
#include <wchar.h>
#include <time.h>
#include <stdarg.h>
#include "CLog.h"

CLog::CLog(const WCHAR* LogFile)
{
	WCHAR szTime[17] = L"";

	time_t timer;
	struct tm Tm;

	memset(szFileName, 0, 1024);
	
	time(&timer);

	localtime_s(&Tm, &timer);

	wsprintf(szTime, L"%04d%02d%02d+%02d%02d%02d",
		Tm.tm_year + 1900,
		Tm.tm_mon + 1,
		Tm.tm_mday,
		Tm.tm_hour,
		Tm.tm_min,
		Tm.tm_sec);

	wcscat_s(szFileName, 128, LogFile);
	wcscat_s(szFileName, 128, szTime);
	wcscat_s(szFileName, 128, L".txt");	

	FILE* fp;
	errno_t err = _wfopen_s(&fp, szFileName, L"a");

	fclose(fp);
}

CLog::~CLog()
{

}

bool CLog::PrintLog(const WCHAR* log, int Line, int err_num)
{
	FILE* fp;
	errno_t err = _wfopen_s(&fp, szFileName, L"a");
	if (err != 0) return false;

	fwprintf(fp, L"[LOG]  : ");
	fwprintf(fp, L"[%7d] %s  ERROR_NUM : %7d\n",
		Line,
		log,
		err_num);

	fclose(fp);
	return true;
}


void CLog::SetLogLevel(int levelNo)
{
	_ilogLevel = levelNo;
}
