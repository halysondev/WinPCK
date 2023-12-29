//////////////////////////////////////////////////////////////////////
// PckClassFileDisk.cpp: used to parse the data in the pck file of Perfect World Company and display it in the List
// Mainly calculate the initial size of the written file, disk space, etc.
//
// This program is written by Li Qiufeng/stsm/liqf
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2018.5.31
//////////////////////////////////////////////////////////////////////

//#include "imagehlp.h"
//#pragma comment(lib,"imagehlp.lib")

#include <windows.h>
#include "PckClassFileDisk.h"


//When creating, if the amount of remaining file space is not enough, add the amount
#define	PCK_STEP_ADD_SIZE				(64*1024*1024)
//When the amount of space remaining in the file is less than this value when created, the data is expanded.
#define	PCK_SPACE_DETECT_SIZE			(4*1024*1024)
//The value of the extended data when the amount of remaining file space is less than this value (PCK_SPACE_DETECT_SIZE) when created
//#define PCK_RENAME_EXPAND_ADD			(16*1024*1024)

#define ZLIB_AVG_RATIO					0.6

CPckClassFileDisk::CPckClassFileDisk()
{}

CPckClassFileDisk::~CPckClassFileDisk()
{
	Logger.OutputVsIde(__FUNCTION__"\r\n");
}

#pragma region ¥≈≈Ãø’º‰¥Û–°

template <typename T>
_inline T * __fastcall mystrcpy(T * dest, const T *src)
{
	while((*dest = *src))
		++dest, ++src;
	return dest;
}


template <typename T>
void GetDriverNameFromFilename(const T* lpszFilename, T lpszDiskName[])
{
	lpszDiskName[0] = lpszFilename[0];
	lpszDiskName[1] = lpszFilename[1];
	lpszDiskName[2] = lpszFilename[2];
	lpszDiskName[3] = '\0';
}

//qwCurrentPckFilesize is the size of the existing file, qwToAddSpace is the size that needs to be expanded, and the return value is (qwCurrentPckFilesize + the maximum size that can be expanded)
QWORD CPckClassFileDisk::GetPckFilesizeByCompressed(QWORD qwDiskFreeSpace, QWORD qwToAddSpace, QWORD qwCurrentPckFilesize)
{
	//Calculate how much space is required qwTotalFileSize
	QWORD	qwTotalFileSizeTemp = qwToAddSpace;

	if(-1 != qwDiskFreeSpace) {

		//If the requested space is less than the remaining disk space, the requested file space size is equal to the remaining disk space.
		if(qwDiskFreeSpace < qwTotalFileSizeTemp)
			qwTotalFileSizeTemp = qwDiskFreeSpace;
	}

	return (qwTotalFileSizeTemp + qwCurrentPckFilesize);

}

//The size of the file required when renaming
QWORD CPckClassFileDisk::GetPckFilesizeRename(const wchar_t * lpszFilename, QWORD qwCurrentPckFilesize)
{
	//Check disk space
	TCHAR szDiskName[4];
	ULARGE_INTEGER lpfree;
	GetDriverNameFromFilename(lpszFilename, szDiskName);


	if(GetDiskFreeSpaceEx(szDiskName, NULL, NULL, &lpfree)) {

		return GetPckFilesizeByCompressed(lpfree.QuadPart, PCK_SPACE_DETECT_SIZE, qwCurrentPckFilesize);
	} else {
		return GetPckFilesizeByCompressed(-1, PCK_SPACE_DETECT_SIZE, qwCurrentPckFilesize);
	}

}

QWORD CPckClassFileDisk::GetPckFilesizeRebuild(const wchar_t * lpszFilename, QWORD qwPckFilesize)
{
	//Check disk space
	TCHAR szDiskName[4];
	ULARGE_INTEGER lpfree;
	GetDriverNameFromFilename(lpszFilename, szDiskName);

	QWORD qwToAddSpace = qwPckFilesize + PCK_SPACE_DETECT_SIZE;

	if(GetDiskFreeSpaceEx(szDiskName, NULL, NULL, &lpfree)) {

		return GetPckFilesizeByCompressed(lpfree.QuadPart, qwToAddSpace, 0);
	} else {
		return GetPckFilesizeByCompressed(-1, qwToAddSpace, 0);
	}
}

QWORD CPckClassFileDisk::GetPckFilesizeByCompressed(LPCSTR lpszFilename, QWORD qwToCompressFilesize, QWORD qwCurrentPckFilesize)
{
	//Check disk space
	char szDiskName[4];
	ULARGE_INTEGER lpfree;
	GetDriverNameFromFilename<char>(lpszFilename, szDiskName);

	QWORD qwToAddSpace = qwToCompressFilesize * ZLIB_AVG_RATIO + PCK_SPACE_DETECT_SIZE;

	if(GetDiskFreeSpaceExA(szDiskName, NULL, NULL, &lpfree)) {

		return GetPckFilesizeByCompressed(lpfree.QuadPart, qwToAddSpace, qwCurrentPckFilesize);
	} else {
		return GetPckFilesizeByCompressed(-1, qwToAddSpace, qwCurrentPckFilesize);
	}
}
#pragma endregion
#pragma region Open file,enum
//File opening, traversal operations, etc.
QWORD CPckClassFileDisk::GetPckFilesizeByCompressed(LPCWSTR lpszFilename, QWORD qwToCompressFilesize, QWORD qwCurrentPckFilesize)
{
	//Check disk space
	wchar_t szDiskName[4];
	ULARGE_INTEGER lpfree;
	GetDriverNameFromFilename<wchar_t>(lpszFilename, szDiskName);

	QWORD qwToAddSpace = qwToCompressFilesize * ZLIB_AVG_RATIO + PCK_SPACE_DETECT_SIZE;

	if(GetDiskFreeSpaceExW(szDiskName, NULL, NULL, &lpfree)) {

		return GetPckFilesizeByCompressed(lpfree.QuadPart, qwToAddSpace, qwCurrentPckFilesize);
	} else {
		return GetPckFilesizeByCompressed(-1, qwToAddSpace, qwCurrentPckFilesize);
	}

}

//////////////////////////The following are the procedures that need to be called during the program///////////////////////////////
VOID CPckClassFileDisk::EnumFile(LPWSTR szFilename, BOOL IsPatition, DWORD &dwFileCount, vector<FILES_TO_COMPRESS> *lpFileLinkList, QWORD &qwTotalFileSize, size_t nLen)
{

	wchar_t		szPath[MAX_PATH], szFile[MAX_PATH];

	size_t nLenBytePath = mystrcpy(szPath, szFilename) - szPath + 1;
	wcscat(szFilename, L"\\*.*");

	HANDLE					hFile;
	WIN32_FIND_DATAW		WFD;

	if((hFile = FindFirstFileW(szFilename, &WFD)) != INVALID_HANDLE_VALUE) {
		if(!IsPatition) {
			FindNextFileW(hFile, &WFD);
			if(!FindNextFileW(hFile, &WFD)) {
				return;
			}
		}

		do {
			if((WFD.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {

				if((MAX_PATH_PCK_260 - 13) >= mystrcpy(mystrcpy(mystrcpy(szFile, szPath), L"\\"), WFD.cFileName) - szFile) {
					EnumFile(szFile, FALSE, dwFileCount, lpFileLinkList, qwTotalFileSize, nLen);
				}

			} else {

				//if(NULL == pFileinfo)return;
				if(0 != WFD.nFileSizeHigh)continue;

				++dwFileCount;

				lpFileLinkList->push_back(FILES_TO_COMPRESS{ 0 });
				LPFILES_TO_COMPRESS	pFileinfo = &lpFileLinkList->back();

				if(MAX_PATH_PCK_260 < nLenBytePath + wcslen(WFD.cFileName)) {
					mystrcpy(mystrcpy(mystrcpy(pFileinfo->szwFilename, szPath), L"\\"), WFD.cAlternateFileName);
				} else {
					mystrcpy(mystrcpy(mystrcpy(pFileinfo->szwFilename, szPath), L"\\"), WFD.cFileName);
				}

#if PCK_DEBUG_OUTPUT
				pFileinfo->id = lpFileLinkList->size();
#endif

				pFileinfo->nFileTitleLen = nLen;
				pFileinfo->nBytesToCopy = MAX_PATH - nLen;

				qwTotalFileSize += (pFileinfo->dwFileSize = WFD.nFileSizeLow);

			}

		} while(FindNextFileW(hFile, &WFD));

		FindClose(hFile);
	}

}

BOOL CPckClassFileDisk::EnumAllFilesByPathList(const vector<wstring> &lpszFilePath, DWORD &_out_FileCount, QWORD &_out_TotalFileSize, vector<FILES_TO_COMPRESS> *lpFileLinkList)
{
	wchar_t		szPathMbsc[MAX_PATH];
	DWORD		dwAppendCount = lpszFilePath.size();

	for(DWORD i = 0; i < dwAppendCount; i++) {

		wcscpy_s(szPathMbsc, lpszFilePath[i].c_str());
		size_t nLen = (size_t)(wcsrchr(szPathMbsc, L'\\') - szPathMbsc) + 1;

		if(FILE_ATTRIBUTE_DIRECTORY == (FILE_ATTRIBUTE_DIRECTORY & GetFileAttributesW(szPathMbsc))) {
			//folder
			EnumFile(szPathMbsc, FALSE, _out_FileCount, lpFileLinkList, _out_TotalFileSize, nLen);
		} else {

			CMapViewFileRead cFileRead;

			if(!cFileRead.Open(szPathMbsc)) {
				//m_PckLog.PrintLogEL(TEXT_OPENNAME_FAIL, szPathMbsc, __FILE__, __FUNCTION__, __LINE__);

				assert(FALSE);
				return FALSE;
			}

			lpFileLinkList->push_back(FILES_TO_COMPRESS{ 0 });
			LPFILES_TO_COMPRESS	lpfirstFile = &lpFileLinkList->back();

			wcscpy(lpfirstFile->szwFilename, szPathMbsc);

			lpfirstFile->nFileTitleLen = nLen;
			lpfirstFile->nBytesToCopy = MAX_PATH - nLen;

#if PCK_DEBUG_OUTPUT
			lpfirstFile->id = lpFileLinkList->size();
#endif
			_out_TotalFileSize += (lpfirstFile->dwFileSize = cFileRead.GetFileSize());
			_out_FileCount++;
		}
	}

	assert(0 != _out_FileCount);
	return TRUE;

}


#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

BOOL CPckClassFileDisk::MakeFolderExistInternal(const wchar_t* lpszDirectory)
{
	wchar_t szUpwardPath[MAX_PATH];
	wcscpy_s(szUpwardPath, lpszDirectory);


	wchar_t *lpLastDir = wcsrchr(szUpwardPath, '\\');

	if (PathFileExistsW(szUpwardPath)) {

		if (PathIsDirectoryW(szUpwardPath)) {
			return TRUE;
		}
		else {
			return FALSE;
		}
	}

	*lpLastDir = 0;

	if (MakeFolderExistInternal(szUpwardPath)) {
		*lpLastDir = '\\';
		if (CreateDirectoryW(szUpwardPath, NULL))
			return TRUE;
	}
	return FALSE;
}

BOOL CPckClassFileDisk::MakeFolderExist(const wchar_t* lpszDirectory)
{

	if ((NULL == lpszDirectory) || (0 == *lpszDirectory))
		return FALSE;

	wchar_t szUpwardPath[MAX_PATH];

	GetFullPathNameW(lpszDirectory, MAX_PATH, szUpwardPath, NULL);
	//wcscpy_s(szUpwardPath, lpszDirectory);
	size_t len = wcslen(szUpwardPath);

	if ('\\' == szUpwardPath[len - 1])
		szUpwardPath[len - 1] = 0;

	return MakeFolderExistInternal(szUpwardPath);
}

#pragma endregion
