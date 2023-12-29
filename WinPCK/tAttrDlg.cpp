//////////////////////////////////////////////////////////////////////
// tAttrDlg.cpp: WinPCK interface thread part
// Dialog code
//
// This program is written by Li Qiufeng/stsm/liqf
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2017.12.26
//////////////////////////////////////////////////////////////////////

#include "miscdlg.h"
#include <shlwapi.h>

TAttrDlg::TAttrDlg(const PCK_UNIFIED_FILE_ENTRY *_lpPckInfo, wchar_t *_lpszPath, TWin *_win) :
	TDlg(IDD_DIALOG_ATTR, _win),
	lpPckInfo(_lpPckInfo),
	lpszPath(_lpszPath)
{
}

//TAttrDlg::~TAttrDlg()
//{
//}

BOOL TAttrDlg::EvCreate(LPARAM lParam)
{
#define PRINTF_BYTESIZE	32
#define PRINTF_SIZE		80

	wchar_t	*lpszFilename;
	const wchar_t	*lpszwFilename;
	wchar_t	szPrintf[PRINTF_SIZE];
	wchar_t	szPrintfBytesize[PRINTF_BYTESIZE];
	LPCWSTR	lpszFormat = L"%s (%llu byte)";
	LPCWSTR	lpszFileFormat = L"%s (%u byte)";

	if (PCK_ENTRY_TYPE_INDEX == lpPckInfo->entryType) {
		wchar_t	szFilename[MAX_PATH_PCK_260];

		wcscpy(szFilename, lpPckInfo->szName);

		SetDlgItemTextW(IDC_EDIT_ATTR_TYPE, L"document");

		if (NULL != (lpszFilename = wcsrchr(szFilename, L'\\'))) {

			*lpszFilename++ = 0;
			SetDlgItemTextW(IDC_EDIT_ATTR_PATH, szFilename);

			lpszwFilename = lpszFilename;

		}
		else if (NULL != (lpszFilename = wcsrchr(szFilename, L'/'))) {

			*lpszFilename++ = 0;
			SetDlgItemTextW(IDC_EDIT_ATTR_PATH, szFilename);

			lpszwFilename = lpszFilename;

		}
		else {

			lpszwFilename = lpPckInfo->szName;
			SetDlgItemTextA(IDC_EDIT_ATTR_PATH, "");
		}

		SetDlgItemTextW(IDC_EDIT_ATTR_NAME, lpszwFilename);
	}
	else {
		SetDlgItemTextW(IDC_EDIT_ATTR_NAME, lpszwFilename = lpPckInfo->szName);
	}

	SetDlgItemTextW(IDC_EDIT_ATTR_PATH, lpszPath);

	uint64_t qdwDirClearTextSize = pck_getFileSizeInEntry(lpPckInfo);
	uint64_t qdwDirCipherTextSize = pck_getCompressedSizeInEntry(lpPckInfo);

	//Compressed size
	swprintf_s(szPrintf, PRINTF_SIZE, lpszFileFormat,
		StrFormatByteSizeW(qdwDirCipherTextSize, szPrintfBytesize, PRINTF_BYTESIZE),
		qdwDirCipherTextSize);

	SetDlgItemTextW(IDC_EDIT_ATTR_CIPHER, szPrintf);


	//actual size
	swprintf_s(szPrintf, PRINTF_SIZE, lpszFileFormat,
		StrFormatByteSizeW(qdwDirClearTextSize, szPrintfBytesize, PRINTF_BYTESIZE),
		qdwDirClearTextSize);

	SetDlgItemTextW(IDC_EDIT_ATTR_SIZE, szPrintf);

	//Compression ratio
	if (0 == qdwDirClearTextSize)
		SetDlgItemTextA(IDC_EDIT_ATTR_CMPR, "-");
	else {
		swprintf_s(szPrintf, PRINTF_SIZE, L"%.1f%%", qdwDirCipherTextSize / (double)qdwDirClearTextSize * 100);
		SetDlgItemTextW(IDC_EDIT_ATTR_CMPR, szPrintf);
	}

	if (PCK_ENTRY_TYPE_FOLDER != (PCK_ENTRY_TYPE_FOLDER & lpPckInfo->entryType))//document
	{
		SetDlgItemTextW(IDC_EDIT_ATTR_TYPE, L"document");

		//Include
		SetDlgItemTextA(IDC_EDIT_ATTR_FILECOUNT, "-");

		//offset address
		swprintf_s(szPrintf, PRINTF_SIZE, L"%llu (0x%016llX)", pck_getFileOffset(lpPckInfo), pck_getFileOffset(lpPckInfo));
		SetDlgItemTextW(IDC_EDIT_ATTR_ADDR, szPrintf);
	}
	else {

		SetDlgItemTextW(IDC_EDIT_ATTR_TYPE, L"folder");

		//Include
		swprintf_s(szPrintf, PRINTF_SIZE, L"%u files, %u folders", pck_getFilesCountInEntry(lpPckInfo), pck_getFoldersCountInEntry(lpPckInfo));
		SetDlgItemTextW(IDC_EDIT_ATTR_FILECOUNT, szPrintf);

		//offset address
		SetDlgItemTextA(IDC_EDIT_ATTR_ADDR, "-");

	}

	//Package information
	const PCK_UNIFIED_FILE_ENTRY* lpRootNode = pck_getRootNode();

	qdwDirClearTextSize = pck_getFileSizeInEntry(lpRootNode);
	qdwDirCipherTextSize = pck_getCompressedSizeInEntry(lpRootNode);

	//total file size
	swprintf_s(szPrintf, PRINTF_SIZE, lpszFormat,
		StrFormatByteSizeW(qdwDirClearTextSize, szPrintfBytesize, PRINTF_BYTESIZE),
		qdwDirClearTextSize);

	SetDlgItemTextW(IDC_EDIT_ATTR_ALLSIZE, szPrintf);

	//Compressed package size
	swprintf_s(szPrintf, PRINTF_SIZE, lpszFormat,
		StrFormatByteSizeW(qdwDirCipherTextSize, szPrintfBytesize, PRINTF_BYTESIZE),
		qdwDirCipherTextSize);
	SetDlgItemTextW(IDC_EDIT_ATTR_PCKSIZE, szPrintf);

	//Compression ratio
	swprintf_s(szPrintf, PRINTF_SIZE, L"%.1f%%", qdwDirCipherTextSize / (float)qdwDirClearTextSize * 100.0);
	SetDlgItemTextW(IDC_EDIT_ATTR_PCKCMPR, szPrintf);

	//Amount of redundant data
	uint64_t qwPckFileSize = pck_file_redundancy_data_size();
	swprintf_s(szPrintf, PRINTF_SIZE, lpszFormat,
		StrFormatByteSizeW(qwPckFileSize, szPrintfBytesize, PRINTF_BYTESIZE),
		qwPckFileSize);
	SetDlgItemTextW(IDC_EDIT_ATTR_DIRT, szPrintf);


	//window text
	TCHAR	szTitle[MAX_PATH];
	_stprintf_s(szTitle, MAX_PATH, TEXT("%s Attributes"), lpszwFilename);
	SetWindowText(szTitle);

	Show();

	return	FALSE;

#undef PRINTF_SIZE
#undef PRINTF_BYTESIZE
}

BOOL TAttrDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	switch(wID) {
	case IDOK:
	case IDCANCEL:
		EndDialog(wID);
		return	TRUE;
	}
	return	FALSE;
}
