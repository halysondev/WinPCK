//////////////////////////////////////////////////////////////////////
// winmain.cpp: WinPCK interface thread part
// Initialization of interface class, message loop, etc.
//
// This program is written by Li Qiufeng/stsm/liqf
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2012.4.10
//////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4244 )
#pragma warning ( disable : 4311 )
//#pragma warning ( disable : 4018 )
#pragma warning ( disable : 4005 )

#include "tlib.h"
#include "resource.h"
#include "globals.h"
#include "winmain.h"
#include <process.h>
#include <shlobj.h>
#include <tchar.h>
#include "GetDragPath.h"

/*
	WinMain
*/
int WINAPI _tWinMain(HINSTANCE hI, HINSTANCE, LPTSTR cmdLine, int nCmdShow)
{
	return	TInstApp(hI, cmdLine, nCmdShow).Run();
}


TInstApp::TInstApp(HINSTANCE _hI, LPTSTR _cmdLine, int _nCmdShow) : TApp(_hI, _cmdLine, _nCmdShow) {}

TInstApp::~TInstApp() {}

void TInstApp::InitWindow(void)
{
	/*	if (IsWinNT() && TIsWow64()) {
			DWORD	val = 0;
			TWow64DisableWow64FsRedirection(&val);
		}
	*/
	TDlg *maindlg = new TInstDlg(cmdLine);
	mainWnd = maindlg;
	maindlg->InitRichEdit2();
	maindlg->Create();

}

TInstDlg::TInstDlg(LPTSTR cmdLine) :
	TDlg(IDD_MAIN),
	m_logdlg(this)
{}

TInstDlg::~TInstDlg() {}

/*
	Initialize the dialog box when receiving WM_INITDIALOG message
*/
BOOL TInstDlg::EvCreate(LPARAM lParam)
{

	//InstallExceptionFilter(THIS_NAME, "%s\r\nException information has been saved to:\r\n%s\r\n");

	int		cx = ::GetSystemMetrics(SM_CXFULLSCREEN), cy = ::GetSystemMetrics(SM_CYFULLSCREEN);

	//GetWindowRect(&rect);
	//int		xsize = rect.right - rect.left, ysize = rect.bottom - rect.top;

	int		xsize = 1100, ysize = 800;

	::SetClassLong(hWnd, GCL_HICON,
		(LONG_PTR)::LoadIcon(TApp::GetInstance(), (LPCTSTR)IDI_ICON_APP));
	//MoveWindow((cx - xsize) / 2, (cy - ysize) / 2, xsize, ysize, TRUE);

	//Interface and data initialization
	SetWindowTextA(THIS_MAIN_CAPTION);
	//Initialization data
	initParams();

	OleInitialize(0);
	//Initialize common controls
	initCommctrls();
	//Initialize toolbar
	initToolbar();
	//Initialize log window
	InitLogWindow();
	//Initialize browsing path
	initArgument();
	//display window
	Show();

	MoveWindow((cx - xsize) / 2, (cy - ysize) / 2, xsize, ysize, TRUE);

	return	TRUE;
}


BOOL TInstDlg::EvClose()
{

	if(bGoingToExit)return FALSE;

	//SetStatusBarText(0, GetLoadStr(IDS_STRING_EXITING));
	SetStatusBarInfo(GetLoadStr(IDS_STRING_EXITING));

	if(pck_isThreadWorking()) {
		if(IDNO == MessageBox(GetLoadStr(IDS_STRING_ISEXIT), GetLoadStr(IDS_STRING_ISEXITTITLE), MB_YESNO | MB_ICONEXCLAMATION | MB_DEFBUTTON2))return FALSE;

		bGoingToExit = TRUE;
		pck_forceBreakThreadWorking();
		return FALSE;
	}

	ShowWindow(SW_HIDE);

	if (nullptr != m_pTaskBarlist)
		m_pTaskBarlist->Release();

	OleUninitialize();

	ListView_Uninit();

	//delete logdlg;

	::PostQuitMessage(0);
	return FALSE;
}


BOOL TInstDlg::EvTimer(WPARAM timerID, TIMERPROC proc)
{
	switch(timerID) {
	case WM_TIMER_PROGRESS_100:
		RefreshProgress();
		break;
	}
	return	FALSE;
}
/*
	Main dialog WM_COMMAND handler
*/
BOOL TInstDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{

	switch(wID) {
	case IDOK:
		return	TRUE;
	case IDCANCEL:
		return	TRUE;
	case ID_OPEN_PCK:
	case ID_MENU_OPEN:
		OpenPckFile();
		break;
	case ID_MENU_UNPACK_ALL:
		UnpackAllFiles();
		break;
	case ID_MENU_UNPACK_SELECTED:
		UnpackSelectedFiles();
		break;
	case ID_CLOSE_PCK:
	case ID_MENU_CLOSE:
		MenuClose();
		break;
	case ID_MENU_INFO:
		MenuInfo();
		break;
	case ID_MENU_SEARCH:
		MenuSearch();
		break;
	case ID_CREATE_NEWPCK:
	case ID_MENU_NEW:
		MenuNew(wID);
		break;
	case ID_ADD_FILE_TO_PCK:
	case ID_MENU_ADD:
		MenuAdd(wID);
		break;
	case ID_MENU_REBUILD:
		MenuRebuild(wID);
		break;
	case ID_MENU_SIMPLIFY:
		MenuStrip();
		break;
	case ID_MENU_COMPRESS_OPT:
		MenuCompressOpt();
		break;
	case ID_MENU_RENAME:
		MenuRename();
		break;
	case ID_MENU_DELETE:
		MenuDelete();
		break;
	case ID_MENU_SELECTALL:
		MenuSelectAll();
		break;
	case ID_MENU_SELECTORP:
		MenuSelectReverse();
		break;
	case ID_MENU_ATTR:
		MenuAttribute();
		break;
	case ID_MENU_EXIT:
		SendMessage(WM_CLOSE, 0, 0);
		break;
	case ID_MENU_VIEW:
		MenuView();
		break;
	case ID_MENU_SETUP:
		AddSetupReg();
		break;
	case ID_MENU_UNSETUP:
		DeleteSetupReg();
		break;
	case ID_MENU_ABOUT:
		MenuAbout();
		break;
	case ID_MENU_LOG:
		m_logdlg.Show();
		break;
	case ID_LISTVIEW_ENTER:
		ListViewEnter();
		break;
	//case ID_LISTVIEW_BACK:
	//	DbClickListView(0);
	//	break;
	//case ID_LISTVIEW_POPMENU:
	//	PopupRightMenu(m_lpPckCenter->GetListCurrentHotItem());
	//	break;
	}
	return	FALSE;
}


BOOL TInstDlg::EvNotify(UINT ctlID, NMHDR *pNmHdr)
{
	switch(ctlID) {
	case IDC_LIST:
		EvNotifyListView(pNmHdr);
		break;
	}
	return FALSE;
}

BOOL TInstDlg::EventButton(UINT uMsg, int nHitTest, POINTS pos)
{
	switch(uMsg) {
	case WM_LBUTTONUP:

		if(m_isSearchWindow) {
			wchar_t szPath[MAX_PATH];

			//DoMouseUp();
			SetCursor(m_hCursorOld);
			ReleaseCapture();

			m_isSearchWindow = FALSE;

			if(IsValidWndAndGetPath(szPath, TRUE)) {
				if(pck_IsValidPck()) {
					if(!pck_isThreadWorking()) {

						wcscpy_s(m_CurrentPath, szPath);
						pck_setMTMaxMemory(0);

						_beginthread(ToExtractSelectedFiles, 0, this);
					}
				}
			}
		}
		break;
	}
	return TRUE;
}

BOOL TInstDlg::EvMouseMove(UINT fwKeys, POINTS pos)
{
	if(m_isSearchWindow) {

		if(IsValidWndAndGetPath(0, FALSE))
			SetCursor(m_hCursorAllow);
		else
			SetCursor(m_hCursorNo);
	}
	return TRUE;
}

BOOL TInstDlg::EvSize(UINT fwSizeType, WORD nWidth, WORD nHeight)
{
	HWND	hList = GetDlgItem(IDC_LIST);
	HWND	hPrgs = GetDlgItem(IDC_PROGRESS);
	HWND	hToolbar = GetDlgItem(IDC_TOOLBAR);

	RECT rectToolbar;

	GetClientRect(hToolbar, &rectToolbar);

	::SetWindowPos(hToolbar, NULL, 0, 0, nWidth, rectToolbar.bottom, SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW);
	::SetWindowPos(GetDlgItem(IDC_STATUS), NULL, 0, nHeight - 22, nWidth, 22, SWP_NOZORDER);

	::SetWindowPos(hPrgs, NULL, 0, nHeight - 36, nWidth, 13, SWP_NOZORDER);

	::SetWindowPos(hList, NULL, 0, rectToolbar.bottom + 6, nWidth, nHeight - (36 + rectToolbar.bottom + 6), SWP_NOZORDER);
	ListView_SetColumnWidth(hList, 0, nWidth - 257);

	return TRUE;
}

/*
	szPath stores the obtained target window path
	isGetPath Whether it is necessary to obtain the target window path
*/
BOOL TInstDlg::IsValidWndAndGetPath(wchar_t * szPath, BOOL isGetPath)
{

	HWND	hwndFoundWindow = NULL, hWndParent = NULL, hRootHwnd = NULL;
	TCHAR	sRootClass[MAX_PATH];//, sParentClass[MAX_PATH];//, sExeName[MAX_PATH];
	POINT	screenpoint;
	//DWORD	hProcessID;

	GetCursorPos(&screenpoint);

	hwndFoundWindow = WindowFromPoint(screenpoint);

	//hWndParent = GetAncestor(hwndFoundWindow, GA_PARENT);
	hRootHwnd = GetAncestor(hwndFoundWindow, GA_ROOT);

	if(GetShellWindow() == hRootHwnd) {
		if(isGetPath) {
			if(SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_DESKTOP, NULL, 0, szPath))) {
				return TRUE;
			}else{
				return FALSE;
			}
		}else{
			return TRUE;
		}
	}

	GetClassName(hRootHwnd, sRootClass, MAX_PATH);
#ifdef _DEBUG
	Debug(TEXT("sRootClass:%s\r\n"), sRootClass);
#endif
	if((0 == lstrcmp(sRootClass, SHELL_LISTVIEW_ROOT_CLASS2)) || \
		(0 == lstrcmp(sRootClass, SHELL_LISTVIEW_ROOT_CLASS1))/* || \
		(GetClassName(hWndParent, sParentClass, MAX_PATH), 0 == lstrcmp(sParentClass, SHELL_LISTVIEW_PARENT_CLASS))*/) {
		if(isGetPath) {
			return GetWndPath(hRootHwnd, szPath);
		} else {
			return TRUE;
		}

	} else {
		//GetClassName(hWndParent, sParentClass, MAX_PATH);

		//if(0 == lstrcmp(sParentClass, SHELL_LISTVIEW_PARENT_CLASS)) {

		//	if(isGetPath) {
		//		return GetWndPath(hRootHwnd, szPath);
		//	}else{
		//		return TRUE;
		//	}
		//}else{
		return FALSE;
		//}
	}

}

#ifdef _USE_CUSTOMDRAW_
////////////////////////////////////////////DrawItem////////////////////////////////////////////////
BOOL TInstDlg::EvMeasureItem(UINT ctlID, MEASUREITEMSTRUCT *lpMis)
{
	switch(ctlID) {
	case IDC_LIST:

		//lpMI = (LPMEASUREITEMSTRUCT)lParam; 
		if(lpMis->CtlType == ODT_HEADER) {
			//lpMis->itemWidth = 0 ; 
			lpMis->itemHeight = 17;		//icon=16x16
		}
		break;
	}

	return FALSE;
}

BOOL TInstDlg::EvDrawItem(UINT ctlID, DRAWITEMSTRUCT *lpDis)
{
	switch(ctlID) {
	case IDC_LIST:

		return EvDrawItemListView(lpDis);
		break;
	}
	return FALSE;
}
#endif

BOOL TInstDlg::EvDropFiles(HDROP hDrop)
{

	if(pck_isThreadWorking())goto END_DROP;

	wchar_t	szFirstFile[MAX_PATH];

	DWORD dwDropFileCount = DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);

	if(0 == dwDropFileCount)goto END_DROP;

	if(1 == dwDropFileCount) {
		if(!pck_IsValidPck()) {
			size_t	nFirstFileLength;
			DragQueryFileW(hDrop, 0, szFirstFile, MAX_PATH);
			nFirstFileLength = wcsnlen(szFirstFile, MAX_PATH);

			if(7 <= nFirstFileLength) {
				if(0 == lstrcmpiW(szFirstFile + nFirstFileLength - 4, L".pck")) {

					OpenPckFile(szFirstFile);
					goto END_DROP;
				}
				else if (0 == lstrcmpiW(szFirstFile + nFirstFileLength - 4, L".zup") ||
					0 == lstrcmpiW(szFirstFile + nFirstFileLength - 4, L".cup")) {

					OpenPckFile(szFirstFile);
					goto END_DROP;
				}
			}
		}
	}

	if(IDCANCEL == MessageBoxW(L"Are you sure you want to add these files?", L"ask", MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2))
		goto END_DROP;

	DragAcceptFiles(hWnd, FALSE);

	m_lpszFilePath.clear();

	for(DWORD i = 0; i < dwDropFileCount; i++) {

		wchar_t szFile[MAX_PATH];
		DragQueryFileW(hDrop, i, szFile, MAX_PATH);
		m_lpszFilePath.push_back(szFile);
	}

	_beginthread(UpdatePckFile, 0, this);


END_DROP:
	DragFinish(hDrop);
	DragAcceptFiles(hWnd, TRUE);
	return	TRUE;
}
