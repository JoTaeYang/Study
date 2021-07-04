#include <Windows.h>
#include <strsafe.h>
#include <time.h>
#include "CSysLog.h"



static e_LOGLEVEL g_LogLevel = e_LOGLEVEL::e_ERROR;
static int g_logCount = 0;

void SetLogLevel(e_LOGLEVEL log)
{
	g_LogLevel = log;
}

void SystemLogPrint(const WCHAR* szType, e_LOGLEVEL logLevel, const WCHAR* stringFormat, ...)
{
	WCHAR logBuffer[512] = { 0, };
	WCHAR szTmpBuffer[512] = { 0, };

	//설정된 로그 레벨보다 상위 레벨일 경우에만 파일로 저장을 한다.
	if (logLevel < g_LogLevel)
		return;

	va_list vlist;
	va_start(vlist, stringFormat);
	HRESULT hresult = StringCchVPrintf(logBuffer, 1024, stringFormat, vlist);
	if (hresult != S_OK)
	{
		wprintf(L"VA_LIST ERROR!!\n");
		wprintf(L"Type : %s Level : %d Format : %s\n", szType, logLevel, stringFormat);
		return;
	}
	va_end(vlist);

	g_logCount++;
	wsprintf(szTmpBuffer, L" LEVEL : %d /COUNTER : %8d / DATA : %s", logLevel, g_logCount, logBuffer);
	SaveLogFileOut(szTmpBuffer, szType);
	
}

void SystemLogConsolePrint(const WCHAR* szType, e_LOGLEVEL logLevel, const WCHAR* stringFormat, ...)
{
	WCHAR logBuffer[512] = { 0, };
	WCHAR szTmpBuffer[512] = { 0, };

	//설정된 로그 레벨보다 상위 레벨일 경우에만 파일로 저장을 한다.
	if (logLevel < g_LogLevel)
		return;

	va_list vlist;
	va_start(vlist, stringFormat);
	HRESULT hresult = StringCchVPrintf(logBuffer, 1024, stringFormat, vlist);
	if (hresult != S_OK)
	{
		wprintf(L"VA_LIST ERROR!!\n");
		wprintf(L"Type : %s Level : %d Format : %s\n", szType, logLevel, stringFormat);
		return;
	}
	va_end(vlist);

	g_logCount++;
	wsprintf(szTmpBuffer, L" LEVEL : %d /COUNTER : %8d / DATA : %s", logLevel, g_logCount, logBuffer);
	wprintf(L"%s\n", szTmpBuffer);
}

void SystemLogPrintHex(const WCHAR* szType, e_LOGLEVEL logLevel, const BYTE* byPointer, DWORD dwlen)
{
	WCHAR logBuffer[512] = { 0, };
	WCHAR szTmpBuffer[32] = { 0, };

	if (logLevel < g_LogLevel)
		return;

	g_logCount++;
	wsprintf(szTmpBuffer, L" LEVEL : %d /COUNTER : %8d / DATA : ", logLevel, g_logCount);

	for (int iCnt = 0; iCnt < dwlen; iCnt++)
	{
		wsprintf(szTmpBuffer, L"%x", byPointer);
		wcscat_s(logBuffer, szTmpBuffer);
		byPointer++;
	}

	wcscat_s(logBuffer, L"\n");
	SaveLogFileOut(logBuffer, szType);
	
}

void SaveLogFileOut(const WCHAR* szStr, const WCHAR* szType)
{
	WCHAR szTime[32] = { 0, };
	WCHAR szfileName[32] = { 0, };
	time_t timer;
	FILE* fp = NULL;
	struct tm Tm;

	time(&timer);

	localtime_s(&Tm, &timer);

	wsprintf(szTime, L"[%04d-%02d-%02d %02d:%02d:%02d]",
		Tm.tm_year + 1900,
		Tm.tm_mon + 1,
		Tm.tm_mday,
		Tm.tm_hour,
		Tm.tm_min,
		Tm.tm_sec);

	wsprintf(szfileName, L"%04d%02d_%s.txt", Tm.tm_year + 1900, Tm.tm_mon + 1, szType);

	errno_t t = _wfopen_s(&fp, szfileName, L"a");
	if (fp == NULL)
		return;

	fwprintf(fp, L"[%s][%s] %s", szType, szTime, szStr);

	fclose(fp);
}