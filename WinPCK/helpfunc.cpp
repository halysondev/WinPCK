//////////////////////////////////////////////////////////////////////
// helpfunc.cpp: WinPCK interface thread part
// Drag and drop, open saved file, preview
//
// This program is written by Li Qiufeng/stsm/liqf
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2012.4.10
//////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4995 )
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4005 )

#include "tlib.h"
#include "resource.h"
#include "winmain.h"
#include "miscdlg.h"
#include <shlwapi.h>
#include "tPreviewDlg.h"
#include <process.h>
#include "OpenSaveDlg.h"
#include "ShowLogOnDlgListView.h"


inline LONG RecurseDeleteKey(HKEY hRegKey, LPCTSTR lpszKey);
inline void CreateAndSetDefaultValue(LPCTSTR pszValueName, LPCTSTR pszValue);

void TInstDlg::UnpackAllFiles()
{
	if(pck_IsValidPck()) {
		if(pck_isThreadWorking()) {
			pck_forceBreakThreadWorking();
			EnableButton(ID_MENU_UNPACK_ALL, FALSE);
		} else {
			if(OpenFilesVistaUp(hWnd, m_CurrentPath)) {

				_beginthread(ToExtractAllFiles, 0, this);
			}
		}
	}
}

void TInstDlg::UnpackSelectedFiles()
{
	if(pck_IsValidPck()) {
		if(pck_isThreadWorking()) {
			pck_forceBreakThreadWorking();
			EnableButton(ID_MENU_UNPACK_SELECTED, FALSE);
		} else {
			if(OpenFilesVistaUp(hWnd, m_CurrentPath)) {

				_beginthread(ToExtractSelectedFiles, 0, this);
			}
		}
	}
}

const PCK_UNIFIED_FILE_ENTRY* TInstDlg::GetFileEntryByItem(int itemIndex)
{
	LVITEM item = { 0 };

	item.mask = LVIF_PARAM;
	item.iItem = itemIndex;
	ListView_GetItem(GetDlgItem(IDC_LIST), &item);
	return (LPPCK_UNIFIED_FILE_ENTRY)item.lParam;
}

void TInstDlg::DbClickListView(const int itemIndex)
{

	m_iListHotItem = itemIndex;

	const PCK_UNIFIED_FILE_ENTRY* lpFileEntry = GetFileEntryByItem(itemIndex);
	if (NULL == lpFileEntry)return;

	int entry_type = lpFileEntry->entryType;

	//Whether the list is displayed in search status
	if(PCK_ENTRY_TYPE_INDEX == entry_type) {

		if(0 != itemIndex) {
			ViewFile(lpFileEntry);
			return;
		}
	}

	//In directory browsing, the .. in the root directory cannot be clicked.
	if ((PCK_ENTRY_TYPE_ROOT | PCK_ENTRY_TYPE_DOTDOT) == ((PCK_ENTRY_TYPE_ROOT | PCK_ENTRY_TYPE_DOTDOT) & entry_type)) {
		return;
	}

	//Whether this level is a folder (NULL=folder)
	if (PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & entry_type)) {

		ShowPckFiles(lpFileEntry);
		pck_getNodeRelativePath(m_FolderBrowsed, lpFileEntry);

	}
	else {
		ViewFile(lpFileEntry);
	}
}

void TInstDlg::PopupRightMenu(const int itemIndex)
{

	const PCK_UNIFIED_FILE_ENTRY* lpFileEntry = GetFileEntryByItem(itemIndex);

	m_iListHotItem = itemIndex;

	HMENU hMenuRClick = GetSubMenu(LoadMenu(TApp::GetInstance(), MAKEINTRESOURCE(IDR_MENU_RCLICK)), 0);

	if(PCK_ENTRY_TYPE_INDEX != lpFileEntry->entryType) {

		if(NULL == lpFileEntry || pck_isThreadWorking()) {
			::EnableMenuItem(hMenuRClick, ID_MENU_VIEW, MF_GRAYED);
			::EnableMenuItem(hMenuRClick, ID_MENU_RENAME, MF_GRAYED);
			::EnableMenuItem(hMenuRClick, ID_MENU_DELETE, MF_GRAYED);
			::EnableMenuItem(hMenuRClick, ID_MENU_UNPACK_SELECTED, MF_GRAYED);

		} else {
			::EnableMenuItem(hMenuRClick, ID_MENU_VIEW, PCK_ENTRY_TYPE_FOLDER != (PCK_ENTRY_TYPE_FOLDER & lpFileEntry->entryType) ? MF_ENABLED : MF_GRAYED);
			::EnableMenuItem(hMenuRClick, ID_MENU_RENAME, 0 != itemIndex ? MF_ENABLED : MF_GRAYED);
			::EnableMenuItem(hMenuRClick, ID_MENU_DELETE, 0 != itemIndex ? MF_ENABLED : MF_GRAYED);
			::EnableMenuItem(hMenuRClick, ID_MENU_UNPACK_SELECTED, 0 != itemIndex ? MF_ENABLED : MF_GRAYED);

		}

	} else {
		::EnableMenuItem(hMenuRClick, ID_MENU_VIEW, 0 != itemIndex ? MF_ENABLED : MF_GRAYED);
		::EnableMenuItem(hMenuRClick, ID_MENU_RENAME, 0 != itemIndex ? MF_ENABLED : MF_GRAYED);
		::EnableMenuItem(hMenuRClick, ID_MENU_DELETE, 0 != itemIndex ? MF_ENABLED : MF_GRAYED);
		::EnableMenuItem(hMenuRClick, ID_MENU_UNPACK_SELECTED, 0 != itemIndex ? MF_ENABLED : MF_GRAYED);

	}

	::EnableMenuItem(hMenuRClick, ID_MENU_ATTR, 0 != itemIndex ? MF_ENABLED : MF_GRAYED);

	TrackPopupMenu(hMenuRClick, TPM_LEFTALIGN, LOWORD(GetMessagePos()), HIWORD(GetMessagePos()), 0, hWnd, NULL);
}

VOID TInstDlg::ViewFileAttribute()
{
	if (0 == m_iListHotItem)return;
	if(pck_isThreadWorking())return;

	const PCK_UNIFIED_FILE_ENTRY* lpFileEntry = GetFileEntryByItem(m_iListHotItem);

	if(pck_IsValidPck()) {
		wchar_t	szPath[MAX_PATH_PCK_260];

		pck_getNodeRelativePath(szPath, m_currentNodeOnShow);

		TAttrDlg	dlg(lpFileEntry, szPath, this);
		dlg.Exec();
	}
}


VOID TInstDlg::ViewFile(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry)
{
	if(pck_isThreadWorking())return;

	CPriviewInDlg cPreview;
	cPreview.Show(lpFileEntry, this);

}

BOOL TInstDlg::AddFiles()
{

	if(pck_isThreadWorking())return FALSE;

	if(IDCANCEL == MessageBoxW(L"Are you sure you want to add the file?", L"ask", MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2))return FALSE;

	if(OpenFiles(hWnd, m_lpszFilePath)) {

		DragAcceptFiles(hWnd, FALSE);
		_beginthread(UpdatePckFile, 0, this);

	}

	return FALSE;
}

int TInstDlg::MyFeedbackCallback(void* pTag, int32_t eventId, size_t wParam, ssize_t lParam)
{
	TInstDlg* pThis = (TInstDlg*)pTag;

	wchar_t szTitle[MAX_PATH];

	switch (eventId)
	{
	case PCK_FILE_OPEN_SUCESS:
		swprintf_s(szTitle, L"%s - %s", TEXT(THIS_MAIN_CAPTION), (const wchar_t*)lParam);
		pThis->SetWindowTextW(szTitle);

		break;

	case PCK_FILE_CLOSE:
		pThis->SetWindowTextA(THIS_MAIN_CAPTION);

		break;
	}

	return 0;
}


void TInstDlg::AddSetupReg()
{

	//BOOL	isExistOldReg;
	HKEY	hRegKey;
	LONG	result;
	//DWORD	dwType;

	//TCHAR	szString[MAX_PATH];
	TCHAR	szStringIcon[MAX_PATH];
	TCHAR	szStringExec[MAX_PATH];

	_tcscpy_s(szStringIcon, m_MyFileName);
	_tcscat_s(szStringIcon, TEXT(",0"));


	_tcscpy_s(szStringExec, TEXT("\""));
	_tcscat_s(szStringExec, m_MyFileName);
	_tcscat_s(szStringExec, TEXT("\" \"%1\""));

	//DWORD	dwDataLength;
	//m_MyFileName

	//Check if [HKEY_CLASSES_ROOT\pckfile] exists
	if(ERROR_SUCCESS == (result = RegOpenKeyEx(HKEY_CLASSES_ROOT,
		TEXT("pckfile\\shell\\open\\command"),
		0,
		KEY_READ,
		&hRegKey))) {
		//exist
		//1. If the program contains patcher.exe, add
		//result = RegQueryValueEx(hRegKey, NULL, NULL, &dwType, reinterpret_cast<LPBYTE>(szString), &dwDataLength)
		//2.Otherwise replace
		//3.Modify open with index
		RegCloseKey(hRegKey);

		RecurseDeleteKey(HKEY_CLASSES_ROOT, TEXT(".pck"));
		RecurseDeleteKey(HKEY_CLASSES_ROOT, TEXT("pckfile"));
		RecurseDeleteKey(HKEY_CLASSES_ROOT, TEXT(".zup"));
		RecurseDeleteKey(HKEY_CLASSES_ROOT, TEXT("ZPWUpdatePack"));
		RecurseDeleteKey(HKEY_CLASSES_ROOT, TEXT(".cup"));
		RecurseDeleteKey(HKEY_CLASSES_ROOT, TEXT("CPWUpdatePack"));

	}

	CreateAndSetDefaultValue(TEXT(".pck"), TEXT("pckfile"));
	CreateAndSetDefaultValue(TEXT("pckfile"), TEXT("Angelica File Package"));
	CreateAndSetDefaultValue(TEXT("pckfile\\DefaultIcon"), szStringIcon);
	CreateAndSetDefaultValue(TEXT("pckfile\\shell"), NULL);
	CreateAndSetDefaultValue(TEXT("pckfile\\shell\\open"), TEXT("Open using WinPCK"));
	CreateAndSetDefaultValue(TEXT("pckfile\\shell\\open\\command"), szStringExec);

	CreateAndSetDefaultValue(TEXT(".zup"), TEXT("ZPWUpdatePack"));
	CreateAndSetDefaultValue(TEXT("ZPWUpdatePack"), TEXT("Zhu Xian update package"));
	CreateAndSetDefaultValue(TEXT("ZPWUpdatePack\\DefaultIcon"), szStringIcon);
	CreateAndSetDefaultValue(TEXT("ZPWUpdatePack\\shell"), NULL);
	CreateAndSetDefaultValue(TEXT("ZPWUpdatePack\\shell\\open"), TEXT("Open using WinPCK"));
	CreateAndSetDefaultValue(TEXT("ZPWUpdatePack\\shell\\open\\command"), szStringExec);

	CreateAndSetDefaultValue(TEXT(".cup"), TEXT("CPWUpdatePack"));
	CreateAndSetDefaultValue(TEXT("CPWUpdatePack"), TEXT("Perfect World update package"));
	CreateAndSetDefaultValue(TEXT("CPWUpdatePack\\DefaultIcon"), szStringIcon);
	CreateAndSetDefaultValue(TEXT("CPWUpdatePack\\shell"), NULL);
	CreateAndSetDefaultValue(TEXT("CPWUpdatePack\\shell\\open"), TEXT("Open using WinPCK"));
	CreateAndSetDefaultValue(TEXT("CPWUpdatePack\\shell\\open\\command"), szStringExec);

	MessageBox(TEXT("The installation is complete."), TEXT("information"), MB_OK | MB_ICONASTERISK);
}

void TInstDlg::DeleteSetupReg()
{
	RecurseDeleteKey(HKEY_CLASSES_ROOT, TEXT(".pck"));
	RecurseDeleteKey(HKEY_CLASSES_ROOT, TEXT("pckfile"));
	RecurseDeleteKey(HKEY_CLASSES_ROOT, TEXT(".zup"));
	RecurseDeleteKey(HKEY_CLASSES_ROOT, TEXT("ZPWUpdatePack"));
	RecurseDeleteKey(HKEY_CLASSES_ROOT, TEXT(".cup"));
	RecurseDeleteKey(HKEY_CLASSES_ROOT, TEXT("CPWUpdatePack"));

	MessageBox(TEXT("Uninstallation completed."), TEXT("information"), MB_OK | MB_ICONASTERISK);
}

inline void CreateAndSetDefaultValue(LPCTSTR pszValueName, LPCTSTR pszValue)
{
	HKEY	hRegKey;

	if(ERROR_SUCCESS == RegCreateKeyEx(HKEY_CLASSES_ROOT,
		pszValueName,
		0,
		REG_NONE,
		REG_OPTION_NON_VOLATILE,
		KEY_READ | KEY_WRITE,
		NULL,
		&hRegKey,
		NULL)) {
		if(NULL != pszValue)
			RegSetValueEx(hRegKey, NULL, NULL, REG_SZ, reinterpret_cast<const BYTE*>(pszValue), (lstrlen(pszValue) + 1) * sizeof(TCHAR));

		RegCloseKey(hRegKey);
	}

}

inline LONG RecurseDeleteKey(HKEY hRegKey, LPCTSTR lpszKey)
{
	HKEY hSubRegKey;
	LONG lRes = RegOpenKeyEx(hRegKey, lpszKey, 0, KEY_READ | KEY_WRITE, &hSubRegKey);
	if(lRes != ERROR_SUCCESS) {
		return lRes;
	}

	FILETIME time;
	DWORD dwSize = 256;
	TCHAR szBuffer[256];
	while(RegEnumKeyEx(hSubRegKey, 0, szBuffer, &dwSize, NULL, NULL, NULL,
		&time) == ERROR_SUCCESS) {
		lRes = RecurseDeleteKey(hSubRegKey, szBuffer);
		if(lRes != ERROR_SUCCESS)
			return lRes;
		dwSize = 256;
	}

	RegCloseKey(hSubRegKey);

	return RegDeleteKey(hRegKey, lpszKey);


}

void TInstDlg::InitLogWindow()
{

	//Log windows
	//logdlg = new TLogDlg(this);
	m_logdlg.Create();
	//SetLogListWnd(logdlg->GetListWnd());
	//SetLogMainWnd(hWnd);

	//binding function
	LogUnits.setInsertLogFunc(std::bind(&TLogDlg::InsertLogToList, &m_logdlg, std::placeholders::_1, std::placeholders::_2));
	LogUnits.setSetStatusBarInfoFunc(std::bind(&TInstDlg::SetStatusBarInfo, this, std::placeholders::_1));

	//Log function binding
	log_regShowFunc(PreInsertLogToList);

	//Startup log
	pck_logIA(THIS_MAIN_CAPTION " is started.");

}


void TInstDlg::RefreshProgress()
{
	wchar_t		szString[MAX_PATH];
	INT			iNewPos;
	wchar_t		szMTMemoryUsed[CHAR_NUM_LEN], szMTMaxMemory[CHAR_NUM_LEN];

	DWORD		dwUIProgress = pck_getUIProgress();
	DWORD		dwUIProgressUpper = pck_getUIProgressUpper();
	DWORD		dwMTMemoryUsed = pck_getMTMemoryUsed();
	DWORD		dwMTMaxMemory = pck_getMTMaxMemory();

	if(0 == dwUIProgressUpper)
		dwUIProgressUpper = 1;

	iNewPos = (INT)((dwUIProgress << 10) / dwUIProgressUpper);

	SendDlgItemMessage(IDC_PROGRESS, PBM_SETPOS, (WPARAM)iNewPos, (LPARAM)0);

	if(nullptr != m_pTaskBarlist)
		m_pTaskBarlist->SetProgressValue(hWnd, dwUIProgress, dwUIProgressUpper);

	//if(dwUIProgress == dwUIProgressUpper)
	//	swprintf_s(szString, szTimerProcessedFormatString, dwUIProgress, dwUIProgressUpper);
	//else
	swprintf_s(
		szString, 
		szTimerProcessingFormatString, 
		dwUIProgress, 
		dwUIProgressUpper, 
		dwUIProgress * 100.0 / dwUIProgressUpper,
		StrFormatByteSizeW(dwMTMemoryUsed, szMTMemoryUsed, CHAR_NUM_LEN),
		StrFormatByteSizeW(dwMTMaxMemory, szMTMaxMemory, CHAR_NUM_LEN),
		(dwMTMemoryUsed >> 10) * 100.0 / (dwMTMaxMemory >> 10));

	//SetStatusBarText(3, szString);
	SetStatusBarProgress(szString);
}

TCHAR*	TInstDlg::BuildSaveDlgFilterString()
{
	static int nPckVersionCount = 0;
	static TCHAR szSaveDlgFilterString[1024] = { 0 };

	if (pck_getVersionCount() != nPckVersionCount) {

		nPckVersionCount = pck_getVersionCount();

		*szSaveDlgFilterString = 0;
		TCHAR szPrintf[256];

		for (int i = 0; i < nPckVersionCount; i++) {

			_stprintf_s(szPrintf, TEXT("%sPCK file(*.pck)|*.pck|"), pck_getVersionNameById(i));
			_tcscat_s(szSaveDlgFilterString, szPrintf);

		}

		TCHAR *lpszStr = szSaveDlgFilterString;
		while (*lpszStr) {

			if (TEXT('|') == *lpszStr)
				*lpszStr = 0;
			++lpszStr;
		}

		*lpszStr = 0;
	}
	return szSaveDlgFilterString;
}