//////////////////////////////////////////////////////////////////////
// tSearchDlg.cpp: WinPCK interface thread part
// Dialog code
//
// This program is written by Li Qiufeng/stsm/liqf
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2017.12.26
//////////////////////////////////////////////////////////////////////

#include "miscdlg.h"

/*
≤È’“∂‘ª∞øÚ
*/
BOOL TSearchDlg::EvCreate(LPARAM lParam)
{
	SendDlgItemMessage(IDC_EDIT_SEARCH, EM_LIMITTEXT, 255, 0);
	SetDlgItemTextW(IDC_EDIT_SEARCH, dirBuf);
	//SendDlgItemMessage(IDC_EDIT_SEARCH, EM_SETSEL, 0, -1);
	//::SetFocus(GetDlgItem(IDC_EDIT_SEARCH));
	return	TRUE;
}

BOOL TSearchDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	switch(wID) {
	case IDOK:
		GetDlgItemTextW(IDC_EDIT_SEARCH, dirBuf, 256);
		EndDialog(wID);
		return	TRUE;

	case IDCANCEL:
		EndDialog(wID);
		return	TRUE;
	}
	return	FALSE;
}
