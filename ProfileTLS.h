#pragma once
#pragma once

#define MAX_SAMPLE 100
#define MAX_PROFILE 10

struct PROFILE_SAMPLE
{
    long lFlag;  //프로파일의 사용 여부. (배열 시에만)
    WCHAR szName[64]; //프로파일 샘플 이름

    LARGE_INTEGER lStartTime; // 프로파일 샘플 실행 시간

    double iTotalTime;  // 전체 사용 시간 카운터
    double iMin[2];  //최소 사용 시간 카운터
    double iMax[2];  //최대 사용 시간 카운터

    _int64 iCall;  //누적 호출
};

struct THREAD_SAMPLE
{
    THREAD_SAMPLE()
    {
        id = 0;
        flag = 0;
    }
    int id;

    int flag;

    PROFILE_SAMPLE samples[MAX_SAMPLE];

};


void SetTLSIndex();

int IsCheckName(const WCHAR* szName);
int IsCheckThread(int id);

int AddThread(int id);
void AddProfile(int index, const WCHAR* szName);



void StartProfile(int index);
void ProfileBegin(const WCHAR* szName);
void ProfileEnd(const WCHAR* szName);
void ProfileDataOutText(const WCHAR* szFileName);

void CheckMinMax(int index, double time);
#define PRO_BEGIN(TagName) ProfileBegin(TagName)
#define PRO_END(TagName) ProfileEnd(TagName)
#define PRO_SET() SetTLSIndex();