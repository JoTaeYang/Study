#include <Windows.h>
#include <stdio.h>
#include <wchar.h>
#include "Profile.h"

//PROFILE_SAMPLE profile[MAX_PROFILE];

THREAD_SAMPLE g_profile[MAX_PROFILE];

int IsCheckName(int index, const WCHAR* szName)
{
    for (int iCnt = 0; iCnt < MAX_SAMPLE; iCnt++)
    {
        if (g_profile[index].samples[iCnt].lFlag == 1)
        {
            if (wcscmp(g_profile[index].samples[iCnt].szName, szName) == 0)
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

void AddProfile(int id, int index, const WCHAR* szName)
{
    if (*szName == L'\0')
        return;
    g_profile[id].samples[index].lFlag = 1;
    wcscpy_s(g_profile[id].samples[index].szName, szName);

    QueryPerformanceCounter(&g_profile[id].samples[index].lStartTime);

    g_profile[id].samples[index].iTotalTime = 0;
    g_profile[id].samples[index].iCall = 1;
    memset(g_profile[id].samples[index].iMin, 0, sizeof(g_profile[id].samples[index].iMin) / sizeof(_int64));
    memset(g_profile[id].samples[index].iMax, 0, sizeof(g_profile[id].samples[index].iMax) / sizeof(_int64));

    return;
}

void StartProfile(int id, int index)
{
    QueryPerformanceCounter(&g_profile[id].samples[index].lStartTime);
    g_profile[id].samples[index].iCall += 1;
}

void ProfileBegin(const WCHAR* szName)
{
    int index = -1;
    int tIndex;

    tIndex = IsCheckThread(GetCurrentThreadId());

    index = IsCheckName(tIndex, szName);

    if (index == -1) // 프로파일 이름이 동일한 것이 없다
    {
        for (int iCnt = 0; iCnt < MAX_SAMPLE; iCnt++)
        {
            if (g_profile[tIndex].samples[iCnt].lFlag == 0)
            {
                //AddProfile(iCnt, szName);
                return;
            }
        }
    }
    else
    {
        StartProfile(tIndex, index);
    }
}
void CheckMinMax(int id, int index, double time)
{
    double tmp = 0;
    if (g_profile[id].samples[index].iMin[0] == 0)
    {
        g_profile[id].samples[index].iMin[0] = time;
        return;
    }
    for (int iCnt = 0; iCnt < 2; iCnt++)
    {
        if (g_profile[id].samples[index].iMin[iCnt] > time)
        {
            tmp = time;
            time = g_profile[id].samples[index].iMin[iCnt];
            g_profile[id].samples[index].iMin[iCnt] = tmp;
        }
    }

    for (int iCnt = 0; iCnt < 2; iCnt++)
    {
        if (g_profile[id].samples[index].iMax[iCnt] < time)
        {
            tmp = time;
            time = g_profile[id].samples[index].iMax[iCnt];
            g_profile[id].samples[index].iMax[iCnt] = tmp;
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
    int tIndex;

    if (!QueryPerformanceFrequency(&Freq))
        return;

    QueryPerformanceCounter(&End);

    tIndex = IsCheckThread(GetCurrentThreadId());
    index = IsCheckName(tIndex, szName); // 체크하는 이름의 인덱스 가져오기

    timeDiff = End.QuadPart - g_profile[tIndex].samples[index].lStartTime.QuadPart;//함수 사용시간

    usingTime = (timeDiff) / (Freq.QuadPart / 1000000.0); //함수 사용시간, 마이크로 세컨드 100만단위로 구함

    g_profile[tIndex].samples[index].iTotalTime += usingTime; //총 사용시간 집계를 위해 더하기

    CheckMinMax(tIndex, index, usingTime); //Min과 Max
}

void ProfileDataOutText(const WCHAR* szFileName)
{
    FILE* fp;

    LPCTSTR columns = L"Id  |  Name  |     Average      |      Min      |      Ma x      |     Call   |\n";
    WCHAR outbuffer[1024];
    char te[1024];
    _wfopen_s(&fp, szFileName, L"w, ccs = UTF-8");
    wmemset(outbuffer, 0, 1024);

    wcsncat_s(outbuffer, columns, wcslen(columns));

    for (int tiCnt = 0; tiCnt < MAX_PROFILE; tiCnt++)
    {

        WCHAR tbuf[100];

        swprintf_s(tbuf, L"-----------------------------------------------\n");
        wcsncat_s(outbuffer, tbuf, wcslen(tbuf));
        for (int iCnt = 0; iCnt < MAX_SAMPLE; iCnt++)
        {
            if (g_profile[tiCnt].samples[iCnt].lFlag == 1)
            {
                WCHAR tmpbuf[250];

                swprintf_s(tmpbuf, L"%2d  |  %10s    |     %10.4lf ㎲ | %10.4lf ㎲ |  %10.4lf ㎲ |  %lld\n", g_profile[tiCnt].id, g_profile[tiCnt].samples[iCnt].szName,
                    (g_profile[tiCnt].samples[iCnt].iTotalTime / g_profile[tiCnt].samples[iCnt].iCall) / 1.0,
                    g_profile[tiCnt].samples[iCnt].iMin[0] / 1.0, g_profile[tiCnt].samples[iCnt].iMax[0] / 1.0, g_profile[tiCnt].samples[iCnt].iCall);
                wcsncat_s(outbuffer, tmpbuf, wcslen(tmpbuf));
            }
        }
        fwrite(outbuffer, 1, wcslen(outbuffer) * 2, fp);
    }


    fclose(fp);
}