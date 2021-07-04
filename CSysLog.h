#pragma once


#define SETSYSLOG_LEVEL(LOGLEVEL) SetLogLevel(LOGLEVEL)
#define LOG(TYPE, LEVEL, FORMAT, ...) SystemLogPrint(TYPE, LEVEL, FORMAT, ##__VA_ARGS__)
#define CONSOLE_LOG(TYPE, LEVEL, FORMAT, ...) SystemLogConsolePrint(TYPE, LEVEL, FORMAT, ##__VA_ARGS__)

enum class e_LOGLEVEL
{
	e_DEBUG = 1,
	e_WARNING,
	e_ERROR
};


void SetLogLevel(e_LOGLEVEL log);

void SystemLogPrint(const WCHAR* szType, e_LOGLEVEL logLevel,const WCHAR* stringFormat, ...);

void SystemLogConsolePrint(const WCHAR* szType, e_LOGLEVEL logLevel, const WCHAR* stringFormat, ...);

void SaveLogFileOut(const WCHAR* szStr, const WCHAR* szType);

void SystemLogPrintHex(const WCHAR* szType, e_LOGLEVEL logLevel, const BYTE * byPointer, DWORD dwlen);