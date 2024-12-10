//////////////////////////////////////////////////////////////////////
// threadproc.cpp: WinPCK interface thread part
// Time-consuming processes such as compression, decompression, and updating require multi-threaded calls
//
// This program is written by Li Qiufeng/stsm/liqf
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2012.4.10
// 2012.10.10
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
#include <tchar.h>
#include "OpenSaveDlg.h"
#include "StopWatch.h"

VOID GetPckFileNameBySource(LPWSTR dst, LPCWSTR src, BOOL isDirectory);

VOID TInstDlg::UpdatePckFile(VOID *pParam)
{

	TInstDlg	*pThis = (TInstDlg*)pParam;
	BOOL		bHasPckOpened = !pck_IsValidPck();
	wchar_t		szFilenameToSave[MAX_PATH];
	wchar_t		szPrintf[320];
	CStopWatch	timer;

	*szFilenameToSave = 0;

	if (bHasPckOpened) {
		//There is no file open at this time. The operation at this time is equivalent to creating a new document.
		pThis->m_currentNodeOnShow = NULL;

		if (1 == pThis->m_lpszFilePath.size()) {
			GetPckFileNameBySource(szFilenameToSave, pThis->m_lpszFilePath[0].c_str(), FALSE);
		}

		//Select the saved file name
		int nSelectFilter = SaveFile(pThis->hWnd, szFilenameToSave, L"pck", pThis->BuildSaveDlgFilterString());
		if (0 > nSelectFilter) {
			pck_close();
			return;
		}

		//Set the version of the target pck
		if(WINPCK_OK != pck_setVersion(nSelectFilter))
			return;

		swprintf_s(szPrintf, GetLoadStrW(IDS_STRING_RENEWING), wcsrchr(szFilenameToSave, L'\\') + 1);
	}
	else {
		wcscpy_s(szFilenameToSave, pThis->m_Filename);
		swprintf_s(szPrintf, GetLoadStrW(IDS_STRING_RENEWING), wcsrchr(pThis->m_Filename, L'\\') + 1);
	}

	//start the timer
	timer.start();

	pThis->EnbaleButtons(ID_MENU_ADD, FALSE);

	//pThis->SetStatusBarText(4, szPrintf);
	pThis->SetStatusBarInfo(szPrintf);


	pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);

	pck_StringArrayReset();
	for (int i = 0; i < pThis->m_lpszFilePath.size(); i++) {
		pck_StringArrayAppend(pThis->m_lpszFilePath[i].c_str());
	}

	if (WINPCK_OK == pck_UpdatePckFileSubmit(szFilenameToSave, pThis->m_currentNodeOnShow)) {

		//End of timer
		timer.stop();

		if (pck_isLastOptSuccess()) {

			//pThis->m_PckLog.PrintLogN(GetLoadStr(IDS_STRING_RENEWOK), timer.getElapsedTime());
			pck_logN(GetLoadStr(IDS_STRING_RENEWOK), timer.getElapsedTime());
			//pThis->SetStatusBarText(4, szPrintf);
		}
		else {

			//pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
			pThis->SetStatusBarInfo(GetLoadStr(IDS_STRING_PROCESS_ERROR));
		}


		// Integrated reporting
		// Print report
		// There are %d original files in the pck package\r\n
		// %d new files added, %d of which have duplicate file names\r\n
		// Use original data addresses %d, new data addresses %d\r\n
		// A total of %d new pck files were passed\r\n

		if (0 != pck_getUpdateResult_PrepareToAddFileCount()) {

			swprintf_s(szPrintf,
				L"The data for this update process is as follows\r\n"
				L"Number of original files in the PCK package %d\r\n"
				L"Number of files planned to be updated %d\r\n"
				L"Actual number of updated files %d\r\n"
				L"Number of files with duplicate names %d\r\n"
				L"Number of files not updated %d\r\n"
				L"The number of files in the PCK package after the update %d",
				pck_getUpdateResult_OldFileCount(),
				pck_getUpdateResult_PrepareToAddFileCount(),
				pck_getUpdateResult_ChangedFileCount(),
				pck_getUpdateResult_DuplicateFileCount(),
				pck_getUpdateResult_PrepareToAddFileCount() - pck_getUpdateResult_ChangedFileCount(),
				pck_getUpdateResult_FinalFileCount());

			pThis->MessageBoxW(szPrintf, L"update report", MB_OK | MB_ICONINFORMATION);

			pck_logI(szPrintf);
		}


		if (bHasPckOpened) {

			pThis->OpenPckFile(szFilenameToSave, TRUE);
		}
		else {
			pThis->OpenPckFile(pThis->m_Filename, TRUE);
		}

	}
	else {

		//pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
		pThis->SetStatusBarInfo(GetLoadStr(IDS_STRING_PROCESS_ERROR));
		if (bHasPckOpened) {
			pck_close();
		}
	}

	pThis->EnbaleButtons(ID_MENU_ADD, TRUE);

	pThis->KillTimer(WM_TIMER_PROGRESS_100);
	pThis->RefreshProgress();

	if ((!(pck_isLastOptSuccess())) && pThis->bGoingToExit) {
		pThis->bGoingToExit = FALSE;
		pThis->SendMessage(WM_CLOSE, 0, 0);
	}

	//Restore Drop status
	pThis->m_lpszFilePath.clear();
	DragAcceptFiles(pThis->hWnd, TRUE);

	return;

}

VOID TInstDlg::RenamePckFile(VOID *pParam)
{

	TInstDlg	*pThis = (TInstDlg*)pParam;
	TCHAR		szPrintf[64];
	CStopWatch	timer;

	_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_RENEWING), _tcsrchr(pThis->m_Filename, TEXT('\\')) + 1);

	//start the timer
	timer.start();

	pThis->EnbaleButtons(ID_MENU_RENAME, FALSE);

	//pThis->SetStatusBarText(4, szPrintf);
	pThis->SetStatusBarInfo(szPrintf);

	pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);

	if (WINPCK_OK == pck_RenameSubmit()) {

		//End of timer
		timer.stop();
		_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_RENEWOK), timer.getElapsedTime());

		//pThis->SetStatusBarText(4, szPrintf);
		pThis->SetStatusBarInfo(szPrintf);

		pThis->OpenPckFile(pThis->m_Filename, TRUE);

	}
	else {

		//pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
		pThis->SetStatusBarInfo(GetLoadStr(IDS_STRING_PROCESS_ERROR));
		pck_close();
	}

	pThis->EnbaleButtons(ID_MENU_RENAME, TRUE);

	pThis->KillTimer(WM_TIMER_PROGRESS_100);
	pThis->RefreshProgress();

	if (pThis->bGoingToExit) {
		pThis->bGoingToExit = FALSE;
		pThis->SendMessage(WM_CLOSE, 0, 0);
	}

	return;
}

VOID TInstDlg::RebuildPckFile(VOID	*pParam)
{

	TInstDlg	*pThis = (TInstDlg*)pParam;

	BOOL		bNeedCreatePck = !pck_IsValidPck();

	TCHAR		szFilenameToSave[MAX_PATH];
	TCHAR		szScriptFile[MAX_PATH];
	TCHAR		szPrintf[288];

	CStopWatch	timer;

	if (bNeedCreatePck) {
		if (!pThis->OpenPckFile()) {
			return;
		}
	}

	_tcscpy_s(szFilenameToSave, pThis->m_Filename);

	TCHAR		*lpszFileTitle = _tcsrchr(szFilenameToSave, TEXT('\\')) + 1;
	_tcscpy(lpszFileTitle, TEXT("Rebuild_"));
	_tcscat_s(szFilenameToSave, _tcsrchr(pThis->m_Filename, TEXT('\\')) + 1);

	//Pop up options dialog box
	//call dialog
	BOOL  bNeedRecompress;
	TRebuildOptDlg	dlg(szScriptFile, &bNeedRecompress, pThis);
	if (IDCANCEL == dlg.Exec())
		return;

	//Select the saved file name
	int nSelectFilter = SaveFile(pThis->hWnd, szFilenameToSave, TEXT("pck"), pThis->BuildSaveDlgFilterString(), pck_getVersion());
	if (0 > nSelectFilter) {
		return;
	}

	if (WINPCK_OK != pck_setVersion(nSelectFilter))
		return;

	//start the timer
	timer.start();

	pThis->EnbaleButtons(ID_MENU_REBUILD, FALSE);

	_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_REBUILDING), _tcsrchr(szFilenameToSave, TEXT('\\')) + 1);
	//pThis->SetStatusBarText(4, szPrintf);
	pThis->SetStatusBarInfo(szPrintf);

	pThis->SetTimer(WM_TIMER_PROGRESS_100, 100, NULL);

	if (WINPCK_OK == pck_RebuildPckFileWithScript(szScriptFile, szFilenameToSave, bNeedRecompress)) {

		//End of timer
		timer.stop();

		if (pck_isLastOptSuccess()) {

			pck_logN(GetLoadStr(IDS_STRING_REBUILDOK), timer.getElapsedTime());
		}
		else {
			//pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
			pThis->SetStatusBarInfo(GetLoadStr(IDS_STRING_PROCESS_ERROR));
		}

	}
	else {

		//pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
		pThis->SetStatusBarInfo(GetLoadStr(IDS_STRING_PROCESS_ERROR));
	}

	pThis->EnbaleButtons(ID_MENU_REBUILD, TRUE);

	if (bNeedCreatePck) {
		ListView_DeleteAllItems(pThis->GetDlgItem(IDC_LIST));
		pck_close();
	}

	pThis->KillTimer(WM_TIMER_PROGRESS_100);
	pThis->RefreshProgress();

	if ((!(pck_isLastOptSuccess())) && pThis->bGoingToExit) {
		pThis->bGoingToExit = FALSE;
		pThis->SendMessage(WM_CLOSE, 0, 0);
	}

	return;
}

VOID TInstDlg::StripPckFile(VOID *pParam)
{

	TInstDlg	*pThis = (TInstDlg*)pParam;

	BOOL		bNeedCreatePck = !pck_IsValidPck();

	TCHAR		szFilenameToSave[MAX_PATH];
	TCHAR		szPrintf[288];

	CStopWatch	timer;

	if (bNeedCreatePck) {
		if (!pThis->OpenPckFile()) {
			return;
		}
	}

	_tcscpy_s(szFilenameToSave, pThis->m_Filename);

	TCHAR		*lpszFileTitle = _tcsrchr(szFilenameToSave, TEXT('\\')) + 1;
	_tcscpy(lpszFileTitle, TEXT("Striped_"));
	_tcscat_s(szFilenameToSave, _tcsrchr(pThis->m_Filename, TEXT('\\')) + 1);

	//Pop up options dialog box
	//call dialog
	int stripFlag;
	TStripDlg	dlg(&stripFlag, pThis);
	if (IDCANCEL == dlg.Exec())
		return;

	//Select the saved file name
	int nSelectFilter = SaveFile(pThis->hWnd, szFilenameToSave, TEXT("pck"), pThis->BuildSaveDlgFilterString(), pck_getVersion());
	if (0 > nSelectFilter) {
		return;
	}

	if (WINPCK_OK != pck_setVersion(nSelectFilter))
		return;

	//start the timer
	timer.start();

	pThis->EnbaleButtons(ID_MENU_REBUILD, FALSE);

	_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_REBUILDING), _tcsrchr(szFilenameToSave, TEXT('\\')) + 1);
	//pThis->SetStatusBarText(4, szPrintf);
	pThis->SetStatusBarInfo(szPrintf);

	pThis->SetTimer(WM_TIMER_PROGRESS_100, 100, NULL);

	if (WINPCK_OK == pck_StripPck(szFilenameToSave, stripFlag)) {

		//End of timer
		timer.stop();

		if (pck_isLastOptSuccess()) {

			pck_logN(GetLoadStr(IDS_STRING_REBUILDOK), timer.getElapsedTime());
		}
		else {
			//pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
			pThis->SetStatusBarInfo(GetLoadStr(IDS_STRING_PROCESS_ERROR));
		}

	}
	else {

		//pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
		pThis->SetStatusBarInfo(GetLoadStr(IDS_STRING_PROCESS_ERROR));
	}

	pThis->EnbaleButtons(ID_MENU_REBUILD, TRUE);

	if (bNeedCreatePck) {
		ListView_DeleteAllItems(pThis->GetDlgItem(IDC_LIST));
		pck_close();
	}

	pThis->KillTimer(WM_TIMER_PROGRESS_100);
	pThis->RefreshProgress();

	if ((!(pck_isLastOptSuccess())) && pThis->bGoingToExit) {
		pThis->bGoingToExit = FALSE;
		pThis->SendMessage(WM_CLOSE, 0, 0);
	}
	return;
}

VOID TInstDlg::CreateNewPckFile(VOID	*pParam)
{
	TInstDlg	*pThis = (TInstDlg*)pParam;

	BOOL		isNotOpenedPck = !pck_IsValidPck();
	TCHAR		szFilenameToSave[MAX_PATH];

	TCHAR		szPrintf[64];

	CStopWatch	timer;

	//Select directory
	if (!OpenFilesVistaUp(pThis->hWnd, pThis->m_CurrentPath))
		return;

	GetPckFileNameBySource(szFilenameToSave, pThis->m_CurrentPath, TRUE);

	//Select the saved file name
	int nSelectFilter = SaveFile(pThis->hWnd, szFilenameToSave, TEXT("pck"), pThis->BuildSaveDlgFilterString());
	if (0 > nSelectFilter)
		return;

	//Set the version of the target pck
	if (WINPCK_OK != pck_setVersion(nSelectFilter))
		return;

	//start the timer
	timer.start();

	pThis->EnbaleButtons(ID_MENU_NEW, FALSE);

	_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_COMPING), _tcsrchr(szFilenameToSave, TEXT('\\')) + 1);
	//pThis->SetStatusBarText(4, szPrintf);
	pThis->SetStatusBarInfo(szPrintf);

	//pck_setThreadWorking(pThis->m_PckHandle);

	pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);

	pck_StringArrayReset();
	pck_StringArrayAppend(pThis->m_CurrentPath);

	if (WINPCK_OK == pck_UpdatePckFileSubmit(szFilenameToSave, pThis->m_currentNodeOnShow)) {

		//End of timer
		timer.stop();

		if (pck_isLastOptSuccess()) {
			pck_logN(GetLoadStr(IDS_STRING_REBUILDOK), timer.getElapsedTime());
		}
		else {
			//pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
			pThis->SetStatusBarInfo(GetLoadStr(IDS_STRING_PROCESS_ERROR));
		}

	}
	else {

		//pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
		pThis->SetStatusBarInfo(GetLoadStr(IDS_STRING_PROCESS_ERROR));
	}

	pThis->EnbaleButtons(ID_MENU_NEW, TRUE);

	if (isNotOpenedPck)
		pck_close();

	pThis->KillTimer(WM_TIMER_PROGRESS_100);
	pThis->RefreshProgress();

	if ((!(pck_isLastOptSuccess())) && pThis->bGoingToExit) {

		pThis->bGoingToExit = FALSE;
		pThis->SendMessage(WM_CLOSE, 0, 0);
	}
	return;
}

VOID TInstDlg::ToExtractAllFiles(VOID	*pParam)
{
	TInstDlg	*pThis = (TInstDlg*)pParam;
	TCHAR		szPrintf[64];

	CStopWatch	timer;

	//start the timer
	timer.start();

	pThis->EnbaleButtons(ID_MENU_UNPACK_ALL, FALSE);

	_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_EXPING), _tcsrchr(pThis->m_Filename, TEXT('\\')) + 1);
	//pThis->SetStatusBarText(4, szPrintf);
	pThis->SetStatusBarInfo(szPrintf);

	pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);

	if (WINPCK_OK == pck_ExtractAllFiles(pThis->m_CurrentPath)) {
		//End of timer
		timer.stop();
		_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_EXPOK), timer.getElapsedTime());

		//pThis->SetStatusBarText(4, szPrintf);
		pThis->SetStatusBarInfo(szPrintf);

	}
	else {
		//pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
		pThis->SetStatusBarInfo(GetLoadStr(IDS_STRING_PROCESS_ERROR));
	}

	pThis->EnbaleButtons(ID_MENU_UNPACK_ALL, TRUE);

	pThis->KillTimer(WM_TIMER_PROGRESS_100);
	pThis->RefreshProgress();

	if ((!(pck_isLastOptSuccess())) && pThis->bGoingToExit) {
		pThis->bGoingToExit = FALSE;
		pThis->SendMessage(WM_CLOSE, 0, 0);
	}

	return;
}

VOID TInstDlg::ToExtractSelectedFiles(VOID	*pParam)
{
	TInstDlg	*pThis = (TInstDlg*)pParam;

	HWND	hList = pThis->GetDlgItem(IDC_LIST);

	CStopWatch	timer;

	const PCK_UNIFIED_FILE_ENTRY **lpFileEntryArray, **lpFileEntryArrayPtr;

	UINT uiSelectCount = ListView_GetSelectedCount(hList);

	if (0 != uiSelectCount) {
		if (NULL != (lpFileEntryArray = (const PCK_UNIFIED_FILE_ENTRY **)malloc(sizeof(PCK_UNIFIED_FILE_ENTRY *) * uiSelectCount))) {
			TCHAR		szPrintf[64];

			//Get lpNodeToShow
			int	nCurrentItemCount = ListView_GetItemCount(hList);

			LVITEM item;
			item.mask = LVIF_PARAM | LVIF_STATE;
			item.iSubItem = 0;
			item.stateMask = LVIS_SELECTED;		// get all state flags

			lpFileEntryArrayPtr = lpFileEntryArray;

			uiSelectCount = 0;

			//Start at 1, skip ..directories
			for (item.iItem = 1; item.iItem < nCurrentItemCount; item.iItem++) {
				ListView_GetItem(hList, &item);

				if (item.state & LVIS_SELECTED) {
					*lpFileEntryArrayPtr = (LPPCK_UNIFIED_FILE_ENTRY)item.lParam;
					lpFileEntryArrayPtr++;

					uiSelectCount++;
				}
			}

			if (0 == uiSelectCount)return;

			//start the timer
			timer.start();

			pThis->EnbaleButtons(ID_MENU_UNPACK_SELECTED, FALSE);

			_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_EXPING), _tcsrchr(pThis->m_Filename, TEXT('\\')) + 1);
			//pThis->SetStatusBarText(4, szPrintf);
			pThis->SetStatusBarInfo(szPrintf);

			pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);

			lpFileEntryArrayPtr = lpFileEntryArray;

			if (WINPCK_OK == pck_ExtractFilesByEntrys(lpFileEntryArray, uiSelectCount, pThis->m_CurrentPath)) {
				//End of timer
				timer.stop();
				_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_EXPOK), timer.getElapsedTime());

				//pThis->SetStatusBarText(4, szPrintf);
				pThis->SetStatusBarInfo(szPrintf);
			}
			else {
				//pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
				pThis->SetStatusBarInfo(GetLoadStr(IDS_STRING_PROCESS_ERROR));
			}

			free(lpFileEntryArray);

			pThis->EnbaleButtons(ID_MENU_UNPACK_SELECTED, TRUE);

			pThis->KillTimer(WM_TIMER_PROGRESS_100);
			pThis->RefreshProgress();

			if ((!(pck_isLastOptSuccess())) && pThis->bGoingToExit) {
				pThis->bGoingToExit = FALSE;
				pThis->SendMessage(WM_CLOSE, 0, 0);
			}
		}
	}
	return;
}


VOID TInstDlg::DeleteFileFromPckFile(VOID	*pParam)
{
	TInstDlg	*pThis = (TInstDlg*)pParam;

	HWND	hList = pThis->GetDlgItem(IDC_LIST);

	CStopWatch	timer;

	UINT uiSelectCount = ListView_GetSelectedCount(hList);

	if (0 != uiSelectCount) {

		TCHAR		szPrintf[64];

		int	nCurrentItemCount = ListView_GetItemCount(hList);

		LVITEM item;
		item.mask = LVIF_PARAM | LVIF_STATE;
		item.iSubItem = 0;
		item.stateMask = LVIS_SELECTED;		// get all state flags

		uiSelectCount = 0;

		for (item.iItem = 1; item.iItem < nCurrentItemCount; item.iItem++) {
			ListView_GetItem(hList, &item);

			if (item.state & LVIS_SELECTED) {
				//WINPCK_OK
				pck_DeleteEntry((LPPCK_UNIFIED_FILE_ENTRY)item.lParam);
				uiSelectCount++;
			}
		}

		if (0 == uiSelectCount)return;

		//start the timer
		timer.start();

		pThis->EnbaleButtons(ID_MENU_DELETE, FALSE);

		_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_RENEWING), _tcsrchr(pThis->m_Filename, TEXT('\\')) + 1);
		//pThis->SetStatusBarText(4, szPrintf);
		pThis->SetStatusBarInfo(szPrintf);

		pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);


		if (WINPCK_OK == pck_RenameSubmit()) {
			//End of timer
			timer.stop();
			_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_RENEWOK), timer.getElapsedTime());

			//pThis->SetStatusBarText(4, szPrintf);
			pThis->SetStatusBarInfo(szPrintf);

			pThis->OpenPckFile(pThis->m_Filename, TRUE);

		}
		else {
			//pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
			pThis->SetStatusBarInfo(GetLoadStr(IDS_STRING_PROCESS_ERROR));
		}

		pThis->EnbaleButtons(ID_MENU_DELETE, TRUE);

		pThis->KillTimer(WM_TIMER_PROGRESS_100);
		pThis->RefreshProgress();

		if (pThis->bGoingToExit) {
			pThis->bGoingToExit = FALSE;
			pThis->SendMessage(WM_CLOSE, 0, 0);
		}

	}
	return;
}

//Push the pre-saved pck file name from the dragged source file name
VOID GetPckFileNameBySource(LPWSTR dst, LPCWSTR src, BOOL isDirectory)
{
	int szPathToCompressLen;
	wcscpy(dst, src);

	if (isDirectory) {
		if ((szPathToCompressLen = lstrlenW(dst)) > 13 && 0 == lstrcmpW(dst + szPathToCompressLen - 10, L".pck.files")) {
			*(dst + szPathToCompressLen - 10) = 0;
		}
	}

	wcscat(dst, L".pck");
}