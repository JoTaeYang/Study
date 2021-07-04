#include <Windows.h>
#include <stdio.h>
#include <wchar.h>
#include "ProfileTLS.h"

//PROFILE_SAMPLE profile[MAX_PROFILE];

THREAD_SAMPLE g_profile[MAX_PROFILE];

int g_TLSIndex;

void SetTLSIndex()
{
    int index = TlsAlloc();
    if (index == TLS_OUT_OF_INDEXES)
    {
        //TLS 부족
    }

    g_TLSIndex = index;    
}

int IsCheckName(const WCHAR* szName)
{
    THREAD_SAMPLE* pSample = NULL;

    pSample = (THREAD_SAMPLE*)TlsGetValue(g_TLSIndex);

    for (int iCnt = 0; iCnt < MAX_SAMPLE; iCnt++)
    {
        if (pSample->samples[iCnt].lFlag == 1)
        {
            if (wcscmp(pSample->samples[iCnt].szName, szName) == 0)
            {
                return iCnt;
            }
        }
    }
    return -1;
}
int IsCheckThread(int id)
{
    for (int iCnt = 0; iCnt < MAX_PROFILE; iCnt++)
    {
        if (g_profile[iCnt].id == id)
        {
            return iCnt;
        }
    }
    int index = AddThread(id);
    return index;
}

int AddThread(int id)
{
    for (int iCnt = 0; iCnt < MAX_PROFILE; iCnt++)
    {
        if (g_profile[iCnt].flag == 0)
        {
            if (InterlockedIncrement((long*)&g_profile[iCnt].flag) == 1)
            {
                g_profile[iCnt].id = id;
                return iCnt;
            }
        }
    }

    return -1; // 배열이 꽉 찬 것.
}

void AddProfile(int index, const WCHAR* szName)
{
    THREAD_SAMPLE* pSample = (THREAD_SAMPLE*)TlsGetValue(g_TLSIndex);
    if (*szName == L'\0')
        return;
    pSample->samples[index].lFlag = 1;
    wcscpy_s(pSample->samples[index].szName, szName);

    QueryPerformanceCounter(&pSample->samples[index].lStartTime);

    pSample->samples[index].iTotalTime = 0;
    pSample->samples[index].iCall = 1;
    memset(pSample->samples[index].iMin, 0, sizeof(pSample->samples[index].iMin) / sizeof(_int64));
    memset(pSample->samples[index].iMax, 0, sizeof(pSample->samples[index].iMax) / sizeof(_int64));

    return;
}

void StartProfile(int index)
{
    THREAD_SAMPLE* pSample = (THREAD_SAMPLE*)TlsGetValue(g_TLSIndex);
    QueryPerformanceCounter(&pSample->samples[index].lStartTime);
    pSample->samples[index].iCall += 1;
}

void ProfileBegin(const WCHAR* szName)
{
    int index = -1;
    int tIndex;
    THREAD_SAMPLE* pSample = NULL;

    if (TlsGetValue(g_TLSIndex) == NULL)
    {
        int tmp = AddThread(GetCurrentThreadId());
        TlsSetValue(g_TLSIndex, (void *)&g_profile[tmp]);
    }
    else
        tIndex = IsCheckThread(GetCurrentThreadId());


    pSample = (THREAD_SAMPLE*)TlsGetValue(g_TLSIndex);
    index = IsCheckName(szName);


    if (index == -1) // 프로파일 이름이 동일한 것이 없다
    {
        for (int iCnt = 0; iCnt < MAX_SAMPLE; iCnt++)
        {
            if (pSample->samples[iCnt].lFlag == 0)
            {
                AddProfile(iCnt, szName);
                return;
            }
        }
    }
    else
    {
        StartProfile(index);
    }
}
void CheckMinMax(int index, double time)
{
    double tmp = 0;
    THREAD_SAMPLE* pSample = (THREAD_SAMPLE *)TlsGetValue(g_TLSIndex);
    if (pSample->samples[index].iMin[0] == 0)
    {
        pSample->samples[index].iMin[0] = time;
        return;
    }
    for (int iCnt = 0; iCnt < 2; iCnt++)
    {
        if (pSample->samples[index].iMin[iCnt] > time)
        {
            tmp = time;
            time = pSample->samples[index].iMin[iCnt];
            pSample->samples[index].iMin[iCnt] = tmp;
        }
    }

    for (int iCnt = 0; iCnt < 2; iCnt++)
    {
        if (pSample->samples[index].iMax[iCnt] < time)
        {
            tmp = time;
            time = pSample->samples[index].iMax[iCnt];
            pSample->samples[index].iMax[iCnt] = tmp;
        }
    }
}

void ProfileEnd(const WCHAR* szName)
{
    LARGE_INTEGER End;
    LARGE_INTEGER Freq;
    LONGLONG timeDiff;
    double microFreq = 0.0;
    double usingTime;
    int index;
    

    if (!QueryPerformanceFrequency(&Freq))
        return;

    QueryPerformanceCounter(&End);

    THREAD_SAMPLE* pSample = (THREAD_SAMPLE*)TlsGetValue(g_TLSIndex);
    
    index = IsCheckName(szName); // 체크하는 이름의 인덱스 가져오기

    timeDiff = End.QuadPart - pSample->samples[index].lStartTime.QuadPart;//함수 사용시간

    usingTime = (timeDiff) / (Freq.QuadPart / 1000000.0); //함수 사용시간, 마이크로 세컨드 100만단위로 구함

    pSample->samples[index].iTotalTime += usingTime; //총 사용시간 집계를 위해 더하기

    CheckMinMax(index, usingTime); //Min과 Max
}

void ProfileDataOutText(const WCHAR* szFileName)
{
    FILE* fp;

    LPCTSTR columns = L"Id\t|\tName\t| \tAverage\t\t| \tMin\t\t| \tMax\t\t|\tCall\n";
    WCHAR outbuffer[3100];
    char te[1024];
    _wfopen_s(&fp, szFileName, L"w, ccs = UTF-8");
    wmemset(outbuffer, 0, 1024);

    wcsncat_s(outbuffer, columns, wcslen(columns));

    WCHAR tmp[256] = L"\n";
   
    fwrite(outbuffer, 1, wcslen(outbuffer) * 2, fp);
    for (int tiCnt = 0; tiCnt < MAX_PROFILE; tiCnt++)
    {
        if (g_profile[tiCnt].flag)
        {                      
            ZeroMemory(outbuffer, wcslen(outbuffer) * 2);
            
            for (int iCnt = 0; iCnt < MAX_SAMPLE; iCnt++)
            {
                if (g_profile[tiCnt].samples[iCnt].lFlag == 1)
                {
                    WCHAR tmpbuf[250];

                    swprintf_s(tmpbuf, L"%d\t| \t%s\t|\t%10.4lf㎲\t|\t%10.4lf㎲\t|\t%10.4lf㎲\t|\t%7lld\n", g_profile[tiCnt].id, g_profile[tiCnt].samples[iCnt].szName,
                        (g_profile[tiCnt].samples[iCnt].iTotalTime / g_profile[tiCnt].samples[iCnt].iCall) / 1.0,
                        g_profile[tiCnt].samples[iCnt].iMin[0] / 1.0, g_profile[tiCnt].samples[iCnt].iMax[0] / 1.0, g_profile[tiCnt].samples[iCnt].iCall);
                    wcsncat_s(outbuffer, tmpbuf, wcslen(tmpbuf));
                }
            } 
            wcsncat_s(outbuffer, tmp, wcslen(tmp));
            fwrite(outbuffer, 1, wcslen(outbuffer) * 2, fp);
        }

    }

    fclose(fp);
}