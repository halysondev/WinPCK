
#include "PckClassWriteOperator.h"

//Create and update pck packages
BOOL CPckClassWriteOperator::UpdatePckFile(const wchar_t * szPckFile, const vector<wstring> &lpszFilePath, const PCK_PATH_NODE* lpNodeToInsert)
{
	DWORD		dwNewFileCount = 0;			//Number of files, number of files in the original pck file
	DWORD		dwDuplicateFileCount = 0;
	uint64_t		qwTotalNewFileSize = 0;						//All file sizes when uncompressed

	int			level = m_lpPckParams->dwCompressLevel;
	int			threadnum = m_lpPckParams->dwMTThread;

	uint64_t		dwAddressWhereToAppendData;
	THREAD_PARAMS		cThreadParams;

	//Start looking for files
	const PCK_PATH_NODE*		lpNodeToInsertPtr;

#pragma region Reset compression parameters
	m_zlib.init_compressor(level);
#pragma endregion


#pragma region Setting parameters
	
	m_FilesToBeAdded.clear();
	m_PckAllInfo.lpFilesToBeAdded = &m_FilesToBeAdded;

	if(m_PckAllInfo.isPckFileLoaded) {

		lpNodeToInsertPtr = lpNodeToInsert;

		//Add data from the end of the file, and you can roll back if the operation fails.
		dwAddressWhereToAppendData = m_PckAllInfo.qwPckSize;

		//Get the relative path of the current node
		if(!GetCurrentNodeString(cThreadParams.cDataFetchMethod.szCurrentNodeString, lpNodeToInsert)) {
			assert(FALSE);
			return FALSE;
		}

		cThreadParams.cDataFetchMethod.nCurrentNodeStringLen = wcslen(cThreadParams.cDataFetchMethod.szCurrentNodeString);

		Logger.i(TEXT_LOG_UPDATE_ADD
			"-"
			TEXT_LOG_LEVEL_THREAD, level, threadnum);

	} else {
		//create a new file
		//m_PckAllInfo.dwAddressOfFileEntry = PCK_DATA_START_AT;
		dwAddressWhereToAppendData = PCK_DATA_START_AT;

		lpNodeToInsertPtr = m_PckAllInfo.cRootNode.child;
		*cThreadParams.cDataFetchMethod.szCurrentNodeString = 0;
		cThreadParams.cDataFetchMethod.nCurrentNodeStringLen = 0;

		Logger.i(TEXT_LOG_UPDATE_NEW
			"-"
			TEXT_LOG_LEVEL_THREAD, level, threadnum);

	}

	wcscpy_s(m_PckAllInfo.szNewFilename, szPckFile);

#pragma endregion

#pragma region Traverse the files to be added 
	if(!EnumAllFilesByPathList(lpszFilePath, dwNewFileCount, qwTotalNewFileSize, m_PckAllInfo.lpFilesToBeAdded))
		return FALSE;

	if(0 == dwNewFileCount)return TRUE;

	m_PckAllInfo.dwFileCountToAdd = dwNewFileCount;
#pragma endregion
	//Parameter Description
	// mt_dwFileCount	Total number of files added, including duplicates
	// dwFileCount		The calculation process uses parameters. This parameter will be used in the following calculation process to represent the total number of files added, excluding duplicates.

	//The number of files is written to the window class and saved to show progress
	SetParams_ProgressUpper(dwNewFileCount);

	//Calculate how much space is required qwTotalFileSize
	cThreadParams.qwCompressTotalFileSize = GetPckFilesizeByCompressed(szPckFile, qwTotalNewFileSize, m_PckAllInfo.qwPckSize);

	//Compare with the files in the original directory to see if there are duplicate names
	//Strategy: Unconditionally overwrite it. If the names are the same and both are files or folders, then overwrite them.
	//
	//Calling FindFileNode returns -1 to exit, and returns 0, which means adding it directly. If it is not 0, it means there are duplicates.
	//Write dedicated writethread and compressthread to call
	//Add the add-specific attribute in PCKINDEXTABLE_COMPRESS to determine whether to enable this node (when the name is the same) 0 is used, 1 is not used
	//Use 2 loops to write the compressed index at the end

	//The dwFileCount variable here refers to the number of added files excluding duplicate names.
	if(m_PckAllInfo.isPckFileLoaded) {
		if(!FindDuplicateNodeFromFileList(lpNodeToInsertPtr, dwDuplicateFileCount))
			return FALSE;
	}

	//log
	Logger.i(TEXT_UPDATE_FILE_INFO, m_PckAllInfo.dwFileCountToAdd, cThreadParams.qwCompressTotalFileSize);

#pragma region Save
	CMapViewFileMultiPckWrite cFileWriter(m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys.dwMaxSinglePckSize);

	//OPEN_ALWAYS, create a new pck (CREATE_ALWAYS) or update an existing pck (OPEN_EXISTING)
	if(!cFileWriter.OpenPckAndMappingWrite(m_PckAllInfo.szNewFilename, OPEN_ALWAYS, cThreadParams.qwCompressTotalFileSize)) {
		return FALSE;
	}

#pragma endregion

	cThreadParams.cDataFetchMethod.ciFilesList = m_PckAllInfo.lpFilesToBeAdded->cbegin();
	cThreadParams.cDataFetchMethod.ciFilesListEnd = m_PckAllInfo.lpFilesToBeAdded->cend();

	cThreadParams.lpFileWrite = &cFileWriter;
	cThreadParams.pck_data_src = DATA_FROM_FILE;
	cThreadParams.dwFileCountOfWriteTarget = dwNewFileCount;
	cThreadParams.lpPckClassThreadWorker = this;
	cThreadParams.dwAddressStartAt = dwAddressWhereToAppendData;
	cThreadParams.lpPckAllInfo = &m_PckAllInfo;
	cThreadParams.pckParams = m_lpPckParams;

	//Write file index
	m_PckAllInfo.dwFileCount = m_PckAllInfo.dwFileCountOld - dwDuplicateFileCount;

	CPckThreadRunner m_threadRunner(&cThreadParams);
	m_threadRunner.start();

	//Re-open it here, or open it directly, which is completed by the interface thread
	m_lpPckParams->cVarParams.dwOldFileCount = m_PckAllInfo.dwFileCountOld;
	m_lpPckParams->cVarParams.dwPrepareToAddFileCount = dwNewFileCount;
	m_lpPckParams->cVarParams.dwChangedFileCount = m_PckAllInfo.dwFileCountToAdd;
	m_lpPckParams->cVarParams.dwDuplicateFileCount = dwDuplicateFileCount;
	m_lpPckParams->cVarParams.dwFinalFileCount = m_PckAllInfo.dwFinalFileCount;

	Logger.i(TEXT_LOG_WORKING_DONE);

	return TRUE;

}
