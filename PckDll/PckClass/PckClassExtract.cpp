//////////////////////////////////////////////////////////////////////
// PckClassExtract.cpp: used to parse the data in the pck file of Perfect World Company and display it in the List
// decompression function
//
// This program is written by Li Qiufeng/stsm/liqf. The pck structure refers to Ruoshui's pck structure.txt, and
// Refer to the part of its Yi language code and read the pck file list
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2012.4.10
//////////////////////////////////////////////////////////////////////
#pragma warning ( disable : 4267 )
#include "PckClass.h"
using namespace std;
BOOL CPckClass::GetSingleFileData(const PCKINDEXTABLE* const lpPckFileIndexTable, char *buffer, size_t sizeOfBuffer)
{
	CMapViewFileMultiPckRead	cFileRead;

	if (!cFileRead.OpenPckAndMappingRead(m_PckAllInfo.szFilename)) {
		return FALSE;
	}
	return CPckClass::GetSingleFileData(&cFileRead, lpPckFileIndexTable, buffer, sizeOfBuffer);
}

BOOL CPckClass::GetSingleFileData(LPVOID lpvoidFileRead, const PCKINDEXTABLE* const lpPckFileIndexTable, char *buffer, size_t sizeOfBuffer)
{

	CMapViewFileMultiPckRead	*lpFileRead = (CMapViewFileMultiPckRead*)lpvoidFileRead;
	const PCKFILEINDEX* lpPckFileIndex = &lpPckFileIndexTable->cFileIndex;

	BYTE *lpMapAddress;
	if(NULL == (lpMapAddress = lpFileRead->View(lpPckFileIndex->dwAddressOffset, lpPckFileIndex->dwFileCipherTextSize))) {
		Logger_el(UCSTEXT(TEXT_VIEWMAPNAME_FAIL), m_PckAllInfo.szFilename);
		return FALSE;
	}

	DWORD	dwFileLengthToWrite = lpPckFileIndex->dwFileClearTextSize;

	if(0 != sizeOfBuffer && sizeOfBuffer < dwFileLengthToWrite)
		dwFileLengthToWrite = sizeOfBuffer;

	if(m_zlib.check_zlib_header(lpMapAddress)) {

		if(Z_OK != m_zlib.decompress_part((BYTE*)buffer, &dwFileLengthToWrite,
			lpMapAddress, lpPckFileIndex->dwFileCipherTextSize, lpPckFileIndex->dwFileClearTextSize)) {
			if(lpPckFileIndex->dwFileClearTextSize == lpPckFileIndex->dwFileCipherTextSize)
				memcpy(buffer, lpMapAddress, dwFileLengthToWrite);
			else {

				Logger_el(UCSTEXT(TEXT_UNCOMPRESSDATA_FAIL), lpPckFileIndex->szFilename);
				assert(FALSE);
				lpFileRead->UnmapViewAll();

				return FALSE;
			}
		}
	} else {
		if (lpPckFileIndex->dwFileCipherTextSize < dwFileLengthToWrite)
			dwFileLengthToWrite = lpPckFileIndex->dwFileCipherTextSize;

		memcpy(buffer, lpMapAddress, dwFileLengthToWrite);
	}
	lpFileRead->UnmapViewAll();
	return TRUE;
}

BOOL CPckClass::ExtractFiles(const PCKINDEXTABLE **lpIndexToExtract, int nFileCount)
{

	Logger.i(TEXT_LOG_EXTRACT);

	//First set up the progress bar
	SetParams_ProgressUpper(nFileCount, TRUE);

	CMapViewFileMultiPckRead	cFileRead;

	if(!cFileRead.OpenPckAndMappingRead(m_PckAllInfo.szFilename)) 
		return FALSE;

	BOOL	ret = TRUE;

	wchar_t	szFilename[MAX_PATH_PCK_260], *szStrchr;

	const PCKINDEXTABLE **lpIndexToExtractPtr = lpIndexToExtract;

	for(int i = 0;i < nFileCount;i++) {
		if(CheckIfNeedForcedStopWorking()) {
			Logger.w(TEXT_USERCANCLE);

			return FALSE;
		}

		wcscpy(szFilename, (*lpIndexToExtractPtr)->cFileIndex.szwFilename);

		szStrchr = szFilename;
		for(int j = 0;j < MAX_PATH_PCK_260;j++) {
			if(TEXT('\\') == *szStrchr)*szStrchr = TEXT('_');
			else if(TEXT('/') == *szStrchr)*szStrchr = TEXT('_');
			//else if(0 == *szStrchr)break;
			++szStrchr;
		}

		//unzip files
		if(!(DecompressFile(szFilename, *lpIndexToExtractPtr, &cFileRead))) {
			Logger_el(TEXT_UNCOMP_FAIL);
			return FALSE;
		} else {
			//dwCount++;
			SetParams_ProgressInc();
		}

		++lpIndexToExtractPtr;
	}

	Logger.i(TEXT_LOG_WORKING_DONE);
	return	ret;
}

BOOL CPckClass::ExtractFiles(const PCK_PATH_NODE **lpNodeToExtract, int nFileCount)
{

	Logger.i(TEXT_LOG_EXTRACT);

	//First set up the progress bar
	SetParams_ProgressUpper(nFileCount, TRUE);

	CMapViewFileMultiPckRead	cFileRead;

	if(!cFileRead.OpenPckAndMappingRead(m_PckAllInfo.szFilename)) 
		return FALSE;

	BOOL	ret = TRUE;

	const PCK_PATH_NODE **lpNodeToExtractPtr = lpNodeToExtract;

	for(int i = 0;i < nFileCount;i++) {
		if(CheckIfNeedForcedStopWorking()) {
			Logger.w(TEXT_USERCANCLE);
			return FALSE;
		}

		if(PCK_ENTRY_TYPE_FOLDER != (PCK_ENTRY_TYPE_FOLDER & (*lpNodeToExtractPtr)->entryType)) {

			//unzip files
			if(!(DecompressFile((*lpNodeToExtractPtr)->szName, (*lpNodeToExtractPtr)->lpPckIndexTable, &cFileRead))) {
				Logger_el(TEXT_UNCOMP_FAIL);
				return FALSE;
			} else {
				//dwCount++;
				SetParams_ProgressInc();
			}

		} else {
			//Add the number of files in the current directory to the progress
			AddParams_ProgressUpper((*lpNodeToExtractPtr)->child->dwFilesCount - 1);

			CreateDirectoryW((*lpNodeToExtractPtr)->szName, NULL);
			SetCurrentDirectoryW((*lpNodeToExtractPtr)->szName);
			ret = StartExtract((*lpNodeToExtractPtr)->child->next, &cFileRead);
			SetCurrentDirectoryA("..\\");
		}

		lpNodeToExtractPtr++;
	}

	Logger.i(TEXT_LOG_WORKING_DONE);
	return	ret;
}

BOOL CPckClass::ExtractFiles(const PCK_UNIFIED_FILE_ENTRY **lpFileEntryArray, int nEntryCount, const wchar_t *lpszDestDirectory)
{
	BOOL rtn = FALSE;

	if (!MakeFolderExist(lpszDestDirectory))
		return FALSE;

	SetCurrentDirectoryW(lpszDestDirectory);

	SetThreadFlag(TRUE);

	if (PCK_ENTRY_TYPE_INDEX == (*lpFileEntryArray)->entryType) {

		rtn = ExtractFiles((const PCKINDEXTABLE **)lpFileEntryArray, nEntryCount);
	}
	else {
		rtn = ExtractFiles((const PCK_PATH_NODE **)lpFileEntryArray, nEntryCount);
	}

	SetThreadFlag(FALSE);
	return rtn;
}

BOOL CPckClass::ExtractAllFiles(const wchar_t *lpszDestDirectory)
{
	BOOL rtn = FALSE;

	if (!MakeFolderExist(lpszDestDirectory))
		return FALSE;

	SetCurrentDirectoryW(lpszDestDirectory);

	SetThreadFlag(TRUE);

	const PCK_PATH_NODE *lpRootNode = &m_PckAllInfo.cRootNode;
	rtn = ExtractFiles(&lpRootNode, 1);

	SetThreadFlag(FALSE);
	return rtn;
}

BOOL CPckClass::StartExtract(LPPCK_PATH_NODE lpNodeToExtract, LPVOID lpvoidFileRead)
{

	CMapViewFileMultiPckRead	*lpFileRead = (CMapViewFileMultiPckRead*)lpvoidFileRead;

	do {
		if(CheckIfNeedForcedStopWorking()) {
			Logger.w(TEXT_USERCANCLE);
			return FALSE;
		}

		//lpThisNodePtr = lpNodeToExtract;
		if(PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & lpNodeToExtract->entryType)) {

			CreateDirectoryW(lpNodeToExtract->szName, NULL);
			SetCurrentDirectoryW(lpNodeToExtract->szName);

			StartExtract(lpNodeToExtract->child->next, lpFileRead);

			SetCurrentDirectoryA("..\\");

		} else {

			//unzip files
			if(!DecompressFile(lpNodeToExtract->szName, lpNodeToExtract->lpPckIndexTable, lpFileRead)) {
				Logger_el(TEXT_UNCOMP_FAIL);

				return FALSE;
			} else {
				//dwCount++;
				SetParams_ProgressInc();
			}
		}

		lpNodeToExtract = lpNodeToExtract->next;

	} while(NULL != lpNodeToExtract);

	return TRUE;
}

BOOL CPckClass::DecompressFile(LPCWSTR	lpszFilename, const PCKINDEXTABLE* lpPckFileIndexTable, LPVOID lpvoidFileRead)
{
	const PCKFILEINDEX* lpPckFileIndex = &lpPckFileIndexTable->cFileIndex;

	CMapViewFileWrite cFileWrite;

	LPBYTE	lpMapAddressToWrite;
	DWORD	dwFileLengthToWrite;

	//if (0 == strcmp(lpszFilename, "SwordFairyMaleSkillAttack1.gfx")){
	//	dwFileLengthToWrite = 1;
	//}

	dwFileLengthToWrite = lpPckFileIndex->dwFileClearTextSize;

	//The following is to create a file to save the decompressed file
	if(!cFileWrite.Open(lpszFilename, CREATE_ALWAYS)) {
		Logger_el(TEXT_OPENWRITENAME_FAIL, lpszFilename);
		return FALSE;
	}

	//If the file size is 0, return directly after creating the file.
	if(0 == dwFileLengthToWrite)
		return TRUE;

	if(!cFileWrite.Mapping(dwFileLengthToWrite)) {
		Logger_el(TEXT_CREATEMAP_FAIL);
		return FALSE;
	}

	if(NULL == (lpMapAddressToWrite = cFileWrite.View(0, 0))) {
		Logger_el(TEXT_VIEWMAPNAME_FAIL, lpszFilename);
		return FALSE;
	}

	BOOL rtn = GetSingleFileData(lpvoidFileRead, lpPckFileIndexTable, (char*)lpMapAddressToWrite);

	cFileWrite.SetFilePointer(dwFileLengthToWrite, FILE_BEGIN);
	cFileWrite.SetEndOfFile();

	return rtn;
}

