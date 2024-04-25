#pragma once

#define TCPFIGHTER
//#define USE_MESSAGEPTR

#define dfATTACK_RANGE_X 80
#define dfATTACK_RANGE_Y 25

#pragma comment(lib, "DbgHelp.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")

#include <process.h>
#include <time.h>
#include <Windows.h>
#include <iostream>
#include <list>
#include <time.h>
#include <strsafe.h>

#include "C:\ProLib\CMemoryPool.h"
#include "c:\ProLib\CMessageBuffer.h"
#include "C:\ProLib\CLockFreeQueue.h"
#include "C:\ProLib\CLockFreeStack.h"
#include "C:\ProLib\CRingBuffer.h"
#include "C:\ProLib\CSysLog.h"

#include <DbgHelp.h>
#include <Psapi.h>

#define GET_SESSIONINDEX(KEY, INDEX) INDEX = ((KEY) >> 47)