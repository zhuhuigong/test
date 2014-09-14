#ifndef __STDAFX_H__
#define __STDAFX_H__

#pragma once

#define WIN32_LEAN_AND_MEAN	
#define _CRT_SECURE_NO_DEPRECATE

// 是否静态链接！
#define STATIC_LINK 0

#include <windows.h>
#include <objbase.h>
#include <zmouse.h>

#if STATIC_LINK == 1
#define UILIB_STATIC
#endif
#include "..\DuiLib\UIlib.h"

using namespace DuiLib;

#if STATIC_LINK == 1
#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "..\\Lib\\DuiLib_Static_ud.lib")
#   else
#       pragma comment(lib, "..\\Lib\\DuiLib_Static_d.lib")
#   endif
#else
#   ifdef _UNICODE
#       pragma comment(lib, "..\\Lib\\DuiLib_Static_u.lib")
#   else
#       pragma comment(lib, "..\\Lib\\DuiLib_Static.lib")
#   endif
#endif
#else
#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "..\\Lib\\DuiLib_ud.lib")
#   else
#       pragma comment(lib, "..\\Lib\\DuiLib_d.lib")
#   endif
#else
#   ifdef _UNICODE
#       pragma comment(lib, "..\\Lib\\DuiLib_u.lib")
#   else
#       pragma comment(lib, "..\\Lib\\DuiLib.lib")
#   endif
#endif
#endif


#endif
