
#pragma once

#ifndef	__StdAfx_h__
#define	__StdAfx_h__

#define WIN32_LEAN_AND_MEAN			//	Exclude rarely-used stuff from Windows headers

#ifndef	WINVER						//	Specify the minimum required feature set.
//#define	WINVER			0x0501		//	Minimum required feature set is Windows XP.
#define	WINVER			0x0600		//	Minimum required feature set is Windows Vista.
#endif

#ifndef	_WIN32_WINNT				//	Specify the minimum required platform.
//#define	_WIN32_WINNT	0x0501		//	Minimum required platform is Windows XP.
#define	_WIN32_WINNT	0x0600		//	Minimum required platform is Windows Vista.
#endif

#ifndef _WIN32_WINDOWS				//	Specify the minimum required platform.
#define _WIN32_WINDOWS	0x0410		//	Minimum required platform is Windows 98.
#endif

#ifndef _WIN32_IE					//	Specify the minimum required Internet Explorer version.
#define _WIN32_IE		0x0600		//	Minimum required Internet Explorer version is IE 6.0
#endif

#ifndef DIRECTINPUT_VERSION			//	Specify the minimum required DirectInput version.
#define	DIRECTINPUT_VERSION	0x0800	//	Minimum required DirectInput version is DI 8.0
#endif

// Windows:
#include <windows.h>

// C run-time includes:
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <math.h>

//	User includes:

#endif	//	#ifndef	__StdAfx_h__
