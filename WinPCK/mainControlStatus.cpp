//////////////////////////////////////////////////////////////////////
// mainControlStatus.cpp: WinPCK interface thread part
// Interface control function
//
// This program is written by Li Qiufeng/stsm/liqf
//
// This code is open source. Please retain the original author information for any modified release based on this code.
//
// 2019.9.10
//////////////////////////////////////////////////////////////////////

#include "tlib.h"
#include "resource.h"
//#include "globals.h"
#include "winmain.h"


void TInstDlg::SetStatusBarText(int	iPart, LPCSTR	lpszText)
{
	SendDlgItemMessageA(IDC_STATUS, SB_SETTEXTA, iPart, (LPARAM)lpszText);
	SendDlgItemMessageA(IDC_STATUS, SB_SETTIPTEXTA, iPart, (LPARAM)lpszText);
}

void TInstDlg::SetStatusBarText(int	iPart, LPCWSTR	lpszText)
{
	SendDlgItemMessageW(IDC_STATUS, SB_SETTEXTW, iPart, (LPARAM)lpszText);
	SendDlgItemMessageW(IDC_STATUS, SB_SETTIPTEXTW, iPart, (LPARAM)lpszText);
}

void TInstDlg::SetStatusBarTitle(LPCWSTR lpszText)
{
	SetStatusBarText(0, lpszText);
}

void TInstDlg::SetStatusBarFileSize(uint64_t size)
{
	wchar_t	szString[64];
	swprintf_s(szString, 64, GetLoadStrW(IDS_STRING_OPENFILESIZE), size);
	SetStatusBarText(1, szString);
}

void TInstDlg::SetStatusBarFileCount(uint32_t size)
{
	wchar_t	szString[64];
	swprintf_s(szString, 64, GetLoadStrW(IDS_STRING_OPENFILECOUNT), size);
	SetStatusBarText(2, szString);
}

void TInstDlg::ClearStatusBarProgress()
{
	SetStatusBarText(3, "");
}

void TInstDlg::SetStatusBarProgress(LPCWSTR lpszText)
{
	SetStatusBarText(3, lpszText);
}

void TInstDlg::SetStatusBarInfo(LPCWSTR lpszText)
{
	SetStatusBarText(4, lpszText);
}