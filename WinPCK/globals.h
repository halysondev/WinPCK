//////////////////////////////////////////////////////////////////////
// globals.h: WinPCK global header file
// Global header file of interface class
//
// This program is written by Li Qiufeng/stsm/liqf
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2012.4.10
//////////////////////////////////////////////////////////////////////

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <windows.h>
#include <assert.h>
#include <vector>
using namespace std;

#ifndef UNICODE
#error("please use Unicode charset") 
#endif

//****** some defines *******
#define _USE_CUSTOMDRAW_

#define	THIS_NAME			"WinPCK "
#define	THIS_DESC			L"Perfect World series game PCK file package comprehensive viewer" 
#define	THIS_AUTHOR			L"Programming: stsm/liqf/Li Qiufeng\r\n" \
							"E-Mail: stsm85@126.com "
#define THIS_UESDLIB        L"This software is open source \n\n" \
							"Updated by Haly \n\n" \
							"updated source code address : https://github.com/halysondev/WinPCK\r\n\r\n" \
							"stsm source code address\r\nhttps://github.com/stsm85\r\n\r\n" \
                            "Use open source libraries\r\n" \
                            "VC LTL(Copyright (C) Chuyu-Team)\r\n" \
                            "libdeflate(Copyright (C) 2016 Eric Biggers)\r\n" \
                            "zlib(Copyright (C) 1995-2011 Mark Adler)\r\n" \
                            "tlib(Copyright (C) 1996-2010 H.Shirouzu) " \
                            

#ifdef _WIN64
	#define	THIS_MAIN_CAPTION	THIS_NAME \
								"64bit v" \
								WINPCK_VERSION
#else
	#define	THIS_MAIN_CAPTION	THIS_NAME \
								"v"WINPCK_VERSION
#endif


//#define	SHELL_LISTVIEW_PARENT_CLASS		TEXT("SHELLDLL_DefView")
#define	SHELL_LISTVIEW_ROOT_CLASS1		TEXT("ExploreWClass")
#define	SHELL_LISTVIEW_ROOT_CLASS2		TEXT("CabinetWClass")
//#define	SHELL_EXENAME					TEXT("\\explorer.exe")

#define	TEXT_INVALID_PATHCHAR			"\\/:*?\"<>|"

#define	TEXT_FILE_FILTER				TEXT(	"PCK compressed file(*.pck;*.zup;*.cup)\0*.pck;*.zup;*.cup\0")	\
										TEXT(	"All files\0*.*\0\0")

#define IMGLIST_FOLDER	0
#define IMGLIST_FILE	1

#define WM_TIMER_PROGRESS_100		(WM_USER + 1)

#define	TIMER_PROGRESS				100

#ifndef SE_CREATE_SYMBOLIC_LINK_NAME
#define SE_CREATE_SYMBOLIC_LINK_NAME  TEXT("SeCreateSymbolicLinkPrivilege")
#endif

#endif