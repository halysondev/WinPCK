
#include "PckClassIndexWriter.h"
#include "PckIndexCache.h"

CPckClassIndexWriter::CPckClassIndexWriter()
{}

CPckClassIndexWriter::~CPckClassIndexWriter()
{}

//Write all indexes
BOOL CPckClassIndexWriter::WriteAllIndex(CMapViewFileMultiPckWrite *lpWrite, LPPCK_ALL_INFOS lpPckAllInfo,  QWORD &dwAddress)
{
	CPckMemoryCache cPckCache;

	//The file progress, initialization, and writing index progress are displayed in the window.
	DWORD dwValidFileCount = lpPckAllInfo->dwFileCount + lpPckAllInfo->dwFileCountToAdd;
	SetParams_ProgressUpper(dwValidFileCount);

	//write original file
	LPPCKINDEXTABLE	lpPckIndexTableOld = lpPckAllInfo->lpPckIndexTable;
	DWORD dwOldPckFileCount = lpPckAllInfo->dwFileCountOld;
	DWORD dwFinalFileCount = 0;

	for(DWORD i = 0; i < dwOldPckFileCount; i++) {

		if(!lpPckIndexTableOld->isInvalid) {
			PCKINDEXTABLE_COMPRESS	pckIndexTableTemp;

			FillAndCompressIndexData(&pckIndexTableTemp, &lpPckIndexTableOld->cFileIndex);
			cPckCache.add(pckIndexTableTemp.compressed_index_data, pckIndexTableTemp.dwIndexDataLength + 8);
			
			++dwFinalFileCount;
			SetParams_ProgressInc();
		}
		else {
			--dwValidFileCount;
		}
		lpPckIndexTableOld++;

	}

	SetParams_ProgressUpper(dwValidFileCount, FALSE);

	lpPckAllInfo->dwFileCount = dwFinalFileCount;

	const vector<PCKINDEXTABLE_COMPRESS> *lpCompedPckIndexTableNew = lpPckAllInfo->lpPckIndexTableToAdd;

	DWORD dwNewPckFileCount = lpPckAllInfo->dwFileCountToAdd;
#if PCK_DEBUG_OUTPUT
	DWORD dwVectorSize = 0;
	if(NULL != lpCompedPckIndexTableNew) dwVectorSize = lpCompedPckIndexTableNew->size();
	assert(dwNewPckFileCount <= dwVectorSize);
#endif
	for(DWORD i = 0; i < dwNewPckFileCount; i++) {

		const PCKINDEXTABLE_COMPRESS *lpPckIndexTablePtr = &(*lpCompedPckIndexTableNew)[i];
		cPckCache.add(lpPckIndexTablePtr->compressed_index_data, lpPckIndexTablePtr->dwIndexDataLength + 8);

		++dwFinalFileCount;
		SetParams_ProgressInc();
	}

	lpPckAllInfo->dwFinalFileCount = dwFinalFileCount;

	lpWrite->Write2(dwAddress, cPckCache.c_buffer(), cPckCache.size());
	dwAddress += cPckCache.size();

	return TRUE;
}