// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <stdio.h>
#include <tchar.h>

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include <WinSock2.h>
#include <Ws2tcpip.h>

#include <list>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <unordered_map>
#include <string>

#include <functional>

#include <exception>

#include <sstream>

#include <memory>

#include <condition_variable>
#include <mutex>
#include <thread>
#include <future>

#define UWM_STARTED                WM_APP + 1
#define UWM_DOWNLOADING            WM_APP + 2
#define UWM_FINISHED               WM_APP + 3
#define UWM_CONNECT_TASK_FINISHED  WM_APP + 4
