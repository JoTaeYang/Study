#pragma once
#pragma once

#define MAX_SAMPLE 100
#define MAX_PROFILE 10

struct PROFILE_SAMPLE
{
    long lFlag;  //���������� ��� ����. (�迭 �ÿ���)
    WCHAR szName[64]; //�������� ���� �̸�

    LARGE_INTEGER lStartTime; // �������� ���� ���� �ð�

    double iTotalTime;  // ��ü ��� �ð� ī����
    double iMin[2];  //�ּ� ��� �ð� ī����
    double iMax[2];  //�ִ� ��� �ð� ī����

    _int64 iCall;  //���� ȣ��
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