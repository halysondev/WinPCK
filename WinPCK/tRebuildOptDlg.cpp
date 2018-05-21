//////////////////////////////////////////////////////////////////////
// tRebuildOptDlg.cpp: WinPCK �����̲߳���
// �Ի������
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2018.05.11
//////////////////////////////////////////////////////////////////////

#include "globals.h"
#include "miscdlg.h"
#include "PckControlCenter.h"
#include "OpenSaveDlg.h"


/*
ѹ��ѡ��
*/
BOOL TRebuildOptDlg::EvCreate(LPARAM lParam)
{
	char	szStr[8];

	SendDlgItemMessage(IDC_EDIT_SCRIPT, EM_LIMITTEXT, MAX_PATH, 0);

	SendDlgItemMessage(IDC_SLIDER_LEVEL, TBM_SETRANGE, FALSE, MAKELONG(1, MAX_COMPRESS_LEVEL));
	SendDlgItemMessage(IDC_SLIDER_LEVEL, TBM_SETPOS, TRUE, (LPARAM)lpParams->dwCompressLevel);

	SetDlgItemTextA(IDC_STATIC_LEVEL, ultoa(lpParams->dwCompressLevel, szStr, 10));
#ifdef _DEBUG
	SetDlgItemTextA(IDC_EDIT_SCRIPT, "F:\\!)MyProjects\\VC\\WinPCK\\testpck\\script\\test.txt");
#endif

	return	TRUE;
}

BOOL TRebuildOptDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{

	switch(wID) {
	case IDOK:
	{
		if(isScriptParseSuccess) {

			OnOK();
			EndDialog(wID);
			return	TRUE;
		}
		break;
	}
	case IDCANCEL:
		EndDialog(wID);
		return	TRUE;


	case IDC_BUTTON_OPEN:
		OnOpenClick();
		break;
	}
	return	FALSE;
}

BOOL TRebuildOptDlg::OnOpenClick()
{
	if(OpenSingleFile(hWnd, szScriptFile)) {

		SetDlgItemText(IDC_EDIT_SCRIPT, szScriptFile);
		return ParseScript();
	}
	return FALSE;
}

void TRebuildOptDlg::OnOK()
{
	DWORD dwCompressLevel = lpParams->dwCompressLevel;
	lpParams->dwCompressLevel = SendDlgItemMessage(IDC_SLIDER_LEVEL, TBM_GETPOS, 0, 0);

	if(dwCompressLevel != lpParams->dwCompressLevel) {
		if(lpParams->lpPckControlCenter->IsValidPck())
			lpParams->lpPckControlCenter->ResetCompressor();
	}

	*lpNeedRecompress = IsDlgButtonChecked(IDC_CHECK_RECPMPRESS);
	//GetDlgItemText(IDC_EDIT_SCRIPT, szScriptFile, MAX_PATH);

}

BOOL TRebuildOptDlg::EventScroll(UINT uMsg, int nCode, int nPos, HWND scrollBar)
{
	int		iPos;
	char	szStr[4];

	switch(nCode) {
	case TB_THUMBTRACK:
	case TB_PAGEDOWN:
	case TB_PAGEUP:
		iPos = ::SendMessage(scrollBar, TBM_GETPOS, 0, 0);
		SetDlgItemTextA(IDC_STATIC_LEVEL, ultoa(iPos, szStr, 10));
		CheckDlgButton(IDC_CHECK_RECPMPRESS, BST_CHECKED);

		break;
	}
	return 0;
}

BOOL TRebuildOptDlg::ParseScript()
{
	GetDlgItemText(IDC_EDIT_SCRIPT, szScriptFile, MAX_PATH);
	if(isScriptParseSuccess = lpParams->lpPckControlCenter->ParseScript((LPCTSTR)szScriptFile)) {

		SetDlgItemTextA(IDC_EDIT_RESULT, "�����ű��ɹ�");
		::EnableWindow(GetDlgItem(IDOK), TRUE);
		return TRUE;
	} else {
		SetDlgItemTextA(IDC_EDIT_RESULT, "�����ű�ʧ��");
		::EnableWindow(GetDlgItem(IDOK), FALSE);
		return FALSE;
	}
}

BOOL TRebuildOptDlg::EvDropFiles(HDROP hDrop)
{

	TCHAR	szFirstFile[MAX_PATH];

	DWORD dwDropFileCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);

	if(0 == dwDropFileCount)goto END_DROP;

	if(1 == dwDropFileCount) {
		
		DragQueryFile(hDrop, 0, szFirstFile, MAX_PATH);
		SetDlgItemText(IDC_EDIT_SCRIPT, szFirstFile);
	}


	DragAcceptFiles(hWnd, FALSE);

	ParseScript();

END_DROP:
	DragFinish(hDrop);
	DragAcceptFiles(hWnd, TRUE);
	return	TRUE;
}