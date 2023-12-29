#include "PckClassWriteOperator.h"
#include "PckClassFileDisk.h"
#include "PckClassRebuildFilter.h"

#include <functional>

#pragma warning ( disable : 4996 )
#pragma warning ( disable : 4267 )
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4244 )

CPckClassWriteOperator::CPckClassWriteOperator()
{}

CPckClassWriteOperator::~CPckClassWriteOperator()
{
	Logger.OutputVsIde(__FUNCTION__"\r\n");
}

/********************************
*
*public function
*
********************/

BOOL CPckClassWriteOperator::RebuildPckFile(const wchar_t * lpszScriptFile, const wchar_t * szRebuildPckFile, BOOL bUseRecompress)
{
	CPckClassRebuildFilter cScriptFilter;

	if ((nullptr != lpszScriptFile) && (0 != *lpszScriptFile)) 
		cScriptFilter.ApplyScript(lpszScriptFile, &m_PckAllInfo.cRootNode);

	return bUseRecompress ? RecompressPckFile(szRebuildPckFile) : RebuildPckFile(szRebuildPckFile);
}

BOOL CPckClassWriteOperator::StripPck(const wchar_t * lpszStripedPckFile, int flag)
{
	//filter first*\textures\*.dds
	CPckClassRebuildFilter cScriptFilter;

	if (PCK_STRIP_DDS & flag) {
		
		cScriptFilter.StripModelTexture(
			m_PckAllInfo.lpPckIndexTable, 
			m_PckAllInfo.dwFileCount,
			m_PckAllInfo.cRootNode.child,
			m_PckAllInfo.szFileTitle
		);
	}
	return RecompressPckFile(lpszStripedPckFile, flag);
}

BOOL CPckClassWriteOperator::RebuildPckFile(const wchar_t * szRebuildPckFile)
{

	Logger.i(TEXT_LOG_REBUILD);

	
	QWORD	dwAddress = PCK_DATA_START_AT;
	DWORD	dwFileCount = m_PckAllInfo.dwFileCount;
	DWORD	dwValidFileCount = ReCountFiles();
	QWORD	dwTotalFileSizeAfterRebuild = GetPckFilesizeRebuild(szRebuildPckFile, m_PckAllInfo.qwPckSize);

	//Parameters required when constructing the head and tail
	PCK_ALL_INFOS		pckAllInfo;
	//Parameters required when constructing the head and tail
	memcpy(&pckAllInfo, &m_PckAllInfo, sizeof(PCK_ALL_INFOS));
	wcscpy_s(pckAllInfo.szNewFilename, szRebuildPckFile);

	//Thread tag
	SetThreadFlag(TRUE);

	//Set the total value of the interface progress bar
	SetParams_ProgressUpper(dwValidFileCount);

	//Open source file 
	CMapViewFileMultiPckRead	cFileRead;
	if(!cFileRead.OpenPckAndMappingRead(pckAllInfo.szFilename)) 
		return FALSE;

	//Open target file 
	//The following is to create a file to save the reconstructed file
	CMapViewFileMultiPckWrite	cFileWrite(pckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys.dwMaxSinglePckSize);

	if(!cFileWrite.OpenPckAndMappingWrite(szRebuildPckFile, CREATE_ALWAYS, dwTotalFileSizeAfterRebuild))
		return FALSE;

	vector<PCKINDEXTABLE_COMPRESS> cPckIndexTable(dwValidFileCount);

	//Do not use Enum for traversal processing, use _PCK_INDEX_TABLE instead

	LPPCKINDEXTABLE lpPckIndexTableSource = pckAllInfo.lpPckIndexTable;
	pckAllInfo.dwFileCountToAdd = 0;

	for(DWORD i = 0; i < dwFileCount; ++i) {

		if(CheckIfNeedForcedStopWorking()) {
			Logger.w(TEXT_USERCANCLE);
			break;
		}

		if(lpPckIndexTableSource->isInvalid) {
			++lpPckIndexTableSource;
			continue;
		}

		LPBYTE lpBufferToRead;

		DWORD dwNumberOfBytesToMap = lpPckIndexTableSource->cFileIndex.dwFileCipherTextSize;
		DWORD dwSrcAddress = lpPckIndexTableSource->cFileIndex.dwAddressOffset;	//Save original address

		if (0 != dwNumberOfBytesToMap) {

			if (NULL == (lpBufferToRead = cFileRead.View(dwSrcAddress, dwNumberOfBytesToMap))) {
				Logger_el(TEXT_VIEWMAP_FAIL);
				return FALSE;
			}

			cFileWrite.Write2(dwAddress, lpBufferToRead, dwNumberOfBytesToMap);
			cFileRead.UnmapViewAll();

		}

		//Write the PckFileIndex file compression information to this file and compress it
		lpPckIndexTableSource->cFileIndex.dwAddressOffset = dwAddress;	//The starting address of the compressed data of this file

		dwAddress += dwNumberOfBytesToMap;	//The starting address of the compressed data of the next file

		FillAndCompressIndexData(&cPckIndexTable[pckAllInfo.dwFileCountToAdd], &lpPckIndexTableSource->cFileIndex);

		lpPckIndexTableSource->cFileIndex.dwAddressOffset = dwSrcAddress;	//Restore original address

		++lpPckIndexTableSource;
		++(pckAllInfo.dwFileCountToAdd);
		SetParams_ProgressInc();

	}

	pckAllInfo.dwFileCountOld = pckAllInfo.dwFileCount = 0;
	pckAllInfo.lpPckIndexTableToAdd = &cPckIndexTable;

	//Close reading file
	//Write file index
	pckAllInfo.dwAddressOfFileEntry = dwAddress;

	WriteAllIndex(&cFileWrite, &pckAllInfo, dwAddress);

	WriteHeadAndTail(&cFileWrite, &pckAllInfo, dwAddress);

	//Thread tag
	SetThreadFlag(FALSE);

	Logger.i(TEXT_LOG_WORKING_DONE);

	return TRUE;
}


//Heavy compressed files
BOOL CPckClassWriteOperator::RecompressPckFile(const wchar_t * szRecompressPckFile, int iStripMode)
{

	Logger.i(TEXT_LOG_RECOMPRESS);

	//QWORD	dwAddress = PCK_DATA_START_AT;
	//DWORD	dwFileCount = m_PckAllInfo.dwFileCount;
	DWORD	dwNoDupFileCount = ReCountFiles();
	QWORD	dwTotalFileSizeAfterRebuild = GetPckFilesizeRebuild(szRecompressPckFile, m_PckAllInfo.qwPckSize);

	THREAD_PARAMS		cThreadParams;
	CMapViewFileMultiPckRead	cFileRead;

#pragma region Reset compression parameters
	m_zlib.init_compressor(m_lpPckParams->dwCompressLevel);
#pragma endregion

	//Parameters required when constructing the head and tail
	PCK_ALL_INFOS		pckAllInfo;
	memcpy(&pckAllInfo, &m_PckAllInfo, sizeof(PCK_ALL_INFOS));
	wcscpy_s(pckAllInfo.szNewFilename, szRecompressPckFile);

	//Set the total value of the interface progress bar
	SetParams_ProgressUpper(dwNoDupFileCount);

	//Open source file 
	if(!cFileRead.OpenPckAndMappingRead(pckAllInfo.szFilename))
		return FALSE;

#pragma region Create target file
	CMapViewFileMultiPckWrite cFileWriter(pckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys.dwMaxSinglePckSize);

	//OPEN_ALWAYS, create a new pck (CREATE_ALWAYS) or update an existing pck (OPEN_EXISTING)
	if(!cFileWriter.OpenPckAndMappingWrite(pckAllInfo.szNewFilename, CREATE_ALWAYS, dwTotalFileSizeAfterRebuild))
		return FALSE;

#pragma endregion

	cThreadParams.cDataFetchMethod.lpFileReadPCK = &cFileRead;
	cThreadParams.cDataFetchMethod.iStripFlag = iStripMode;
	cThreadParams.cDataFetchMethod.dwProcessIndex = 0;
	cThreadParams.cDataFetchMethod.dwTotalIndexCount = pckAllInfo.dwFileCount;
	cThreadParams.cDataFetchMethod.lpPckIndexTablePtrSrc = pckAllInfo.lpPckIndexTable;

	cThreadParams.lpPckClassThreadWorker = this;
	cThreadParams.lpFileWrite = &cFileWriter;
	cThreadParams.pck_data_src = DATA_FROM_PCK;
	cThreadParams.dwAddressStartAt = PCK_DATA_START_AT;
	cThreadParams.lpPckAllInfo = &pckAllInfo;
	cThreadParams.pckParams = m_lpPckParams;
	cThreadParams.dwFileCountOfWriteTarget = dwNoDupFileCount;

	//Write file index
	pckAllInfo.lpPckIndexTable = NULL;
	pckAllInfo.dwFileCountOld = pckAllInfo.dwFileCount = 0;

	CPckThreadRunner m_threadRunner(&cThreadParams);
	m_threadRunner.start();

	Logger.n(TEXT_LOG_WORKING_DONE);

	return TRUE;
}