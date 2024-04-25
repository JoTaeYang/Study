#pragma once
class CCrashDump
{
public:
    static long _DumpCount;

    CCrashDump()
    {
        _DumpCount = 0;

        _invalid_parameter_handler oldHandler, newHandler;
        newHandler = myInvalidParameterHandler;

        oldHandler = _set_invalid_parameter_handler(newHandler); //crt함수에 null 포인터 등 넣을 때.
        _CrtSetReportMode(_CRT_WARN, 0); // CRT 오류 메시지 중단, 덤프가 바로 남게.
        _CrtSetReportMode(_CRT_ASSERT, 0); // CRT 오류 메시지 중단, 덤프가 바로 남게.
        _CrtSetReportMode(_CRT_ERROR, 0);// CRT 오류 메시지 중단, 덤프가 바로 남게.

        _CrtSetReportHook(_custom_Report_hook);


        //pure virtual function called 에러 핸들러를 사용자 정의 함수로 우회
        _set_purecall_handler(myPurecallHandler);

        SetHandlerDump();

    }

    static void Crash(void)
    {
        int* p = nullptr;
        *p = 0;

    }

    static LONG WINAPI MyExceptionFilter(__in PEXCEPTION_POINTERS pExceptionPointer)
    {
        int iWorkingMemory = 0;
        SYSTEMTIME stNowTime;

        long DumpCount = InterlockedIncrement(&_DumpCount);

        //현 프로세스의 메모리 사용량 얻기
        HANDLE hProcess = 0;
        PROCESS_MEMORY_COUNTERS pmc;

        hProcess = GetCurrentProcess();

        if (NULL == hProcess)
        {
            printf("Process NULL\n");
            return 0;
        }

        if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
        {
            iWorkingMemory = (int)(pmc.WorkingSetSize / 1024 / 1024);
        }
        CloseHandle(hProcess);

        WCHAR filename[MAX_PATH];
        GetLocalTime(&stNowTime);
        wsprintf(filename, L"Dump_%d%02d%02d_%02d.%02d.%02d_%d_%dMB.dmp",
            stNowTime.wYear, stNowTime.wMonth, stNowTime.wDay, stNowTime.wHour, stNowTime.wMinute, stNowTime.wSecond, DumpCount, iWorkingMemory);

        wprintf(L"\n\n\n!!! Crash Error !!! %d.%d.%d / %d:%d:%d \n",
            stNowTime.wYear, stNowTime.wMonth, stNowTime.wDay, stNowTime.wHour, stNowTime.wMinute, stNowTime.wSecond);
        wprintf(L"Now Save Dump File...\n");


        HANDLE hDumpFile = CreateFile(filename,
            GENERIC_WRITE,
            FILE_SHARE_WRITE,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, NULL);

        if (hDumpFile != INVALID_HANDLE_VALUE)
        {
            _MINIDUMP_EXCEPTION_INFORMATION MinidumpExceptionInformation;

            MinidumpExceptionInformation.ThreadId = GetCurrentThreadId();
            MinidumpExceptionInformation.ExceptionPointers = pExceptionPointer;
            MinidumpExceptionInformation.ClientPointers = TRUE;

            MiniDumpWriteDump(GetCurrentProcess(),
                GetCurrentProcessId(),
                hDumpFile,
                MiniDumpWithFullMemory,
                &MinidumpExceptionInformation,
                NULL,
                NULL);
            CloseHandle(hDumpFile);

            wprintf(L"Dump Save Finish");
            int c;
            scanf_s("%d", &c);
        }
        return EXCEPTION_EXECUTE_HANDLER;
    }

    static void myInvalidParameterHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved)
    {
        //RaiseException(STATUS_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
        Crash();
    }

    static int _custom_Report_hook(int ireposttype, char* message, int* returnvalue)
    {
        Crash();
        return true;
    }

    static void SetHandlerDump()
    {
        if (SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)MyExceptionFilter))
        {
            //printf("Null Handler\n");
        }
    }

    static void myPurecallHandler()
    {
        Crash();
    }
};
