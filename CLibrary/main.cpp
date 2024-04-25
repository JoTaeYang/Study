// 포폴_2DGAME_SERVER.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <stdlib.h>
#include <WS2tcpip.h>
#include <iostream>
#include <process.h>
#include <Windows.h>
#include <conio.h>
#include <list>
#include <time.h>
#include "C:\ProLib\CMemoryPool.h"
#include "c:\ProLib\CMessageBuffer.h"
#include "C:\ProLib\CLockFreeQueue.h"
#include "C:\ProLib\CLockFreeStack.h"
#include "C:\ProLib\CRingBuffer.h"
#include "C:\ProLib\CSysLog.h"
#include "stSESSION.h"
#include "CSector.h"
#include "CServer.h"
#include "CFighterServer.h"
int main()
{
	WCHAR Key;
	CFighterServer server;

	server.Start(L"0.0.0.0", 20000, 2, 9500);

	while (1)
	{
		server.SetTPSInitial();

		Sleep(1000);

		printf("Accept Total : %d\n", server.GetAcceptTotal());
		printf("Accept TPS : %d\n", server.GetAcceptTPS());
		printf("Recv TPS :%d\n", server.GetRecvTPS());
		printf("Send TPS :%d\n", server.GetSendTPS());
		printf("Game Thread Process Count :%d\n", server.GetProcessCnt());
		printf("Game Thread Frame Count :%d\n", server.GetGameThreadFrame());
		printf("Send Thread Frame Count :%d\n", server.GetSendThreadFrame());
		printf("직렬화 버퍼 Memory Pool Alloc Count : %d\n", CMessageBuffer::GetAllocCount());
		
		

		if (_kbhit())
		{
			Key = _getwch();
			if (Key == L'Q' || Key == L'q')
			{
				break;
			}
		}
	}
	server.Stop();
}


