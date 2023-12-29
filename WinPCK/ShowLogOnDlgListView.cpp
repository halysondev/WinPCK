//////////////////////////////////////////////////////////////////////
// ShowLogOnDlgListView.cpp: Display the log in the listView
//
//
// This program is written by Li Qiufeng/stsm/liqf
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2018.6.4
//////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include "ShowLogOnDlgListView.h"
#include "resource.h"
#include <commctrl.h>
#include <stdio.h>

CLogUnits LogUnits;

void	PreInsertLogToList(const char ch, const wchar_t *str)
{
	LogUnits.InsertLog(ch, str);
}


CLogUnits::CLogUnits()
{
}

CLogUnits::~CLogUnits()
{
}
