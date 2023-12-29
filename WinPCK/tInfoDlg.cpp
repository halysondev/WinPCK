//////////////////////////////////////////////////////////////////////
// tInfoDlg.cpp: WinPCK interface thread part
// Dialog code
//
// This program is written by Li Qiufeng/stsm/liqf
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2017.12.26
//////////////////////////////////////////////////////////////////////

#include "globals.h"
#include "miscdlg.h"
#include "PckHeader.h"

/*
Additional information dialog
*/
BOOL TInfoDlg::EvCreate(LPARAM lParam)
{
	SendDlgItemMessage(IDC_EDIT_INFO, EM_LIMITTEXT, pck_GetAdditionalInfoMaxSize() - 1, 0);
	SetDlgItemTextA(IDC_EDIT_INFO, pck_GetAdditionalInfo());
	return	TRUE;
}

BOOL TInfoDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	char *szAdditionalInfo;
	switch(wID) {
	case IDOK:
		szAdditionalInfo = new char[pck_GetAdditionalInfoMaxSize()];
		memset(szAdditionalInfo, 0, pck_GetAdditionalInfoMaxSize());
		GetDlgItemTextA(IDC_EDIT_INFO, szAdditionalInfo, pck_GetAdditionalInfoMaxSize());

		//Returning 1 means the operation is successful, success=WINPCK_OK
		pck_SetAdditionalInfo(szAdditionalInfo);
		EndDialog(wID);
		return	TRUE;

	case IDCANCEL:
		EndDialog(wID);
		return	TRUE;
	}
	return	FALSE;
}

