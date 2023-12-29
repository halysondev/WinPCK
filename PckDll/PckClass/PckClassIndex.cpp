
#include "PckClassIndex.h"
#include <thread>
#include <vector>

CPckClassIndex::CPckClassIndex()
{}

CPckClassIndex::~CPckClassIndex()
{
	if(NULL != m_PckAllInfo.lpPckIndexTable)
		free(m_PckAllInfo.lpPckIndexTable);
	Logger.OutputVsIde(__FUNCTION__"\r\n");
}

void* CPckClassIndex::AllocMemory(size_t	sizeStuct)
{
	void*		lpMallocNode;

	if(NULL == (lpMallocNode = malloc(sizeStuct))) {
		SetErrMsgFlag(PCK_ERR_MALLOC);
		return NULL;
	}
	//Initialize memory
	memset(lpMallocNode, 0, sizeStuct);
	return lpMallocNode;
}

LPPCKINDEXTABLE	CPckClassIndex::AllocPckIndexTableByFileCount()
{
	//Here m_PckAllInfo.dwFileCount + 1 is given, the last 1 is used as the end of the mark index, and its entryType is set to PCK_ENTRY_TYPE_TAIL_INDEX
	//PCK_ENTRY_TYPE_TAIL_INDEX, this index does not store data, it is only used to mark the end
	return (LPPCKINDEXTABLE)AllocMemory(sizeof(PCKINDEXTABLE) * (m_PckAllInfo.dwFileCount + 1));
}

void CPckClassIndex::GenerateUnicodeStringToIndex()
{
	LPPCKINDEXTABLE lpPckIndexTable = m_PckAllInfo.lpPckIndexTable;

	for(DWORD i = 0;i < m_PckAllInfo.dwFileCount;++i) {
		//File name length
		lpPckIndexTable->nFilelenBytes = strlen(lpPckIndexTable->cFileIndex.szFilename);
		//The remaining space in the file name does not occupy the last \0
		lpPckIndexTable->nFilelenLeftBytes = MAX_PATH_PCK_256 - lpPckIndexTable->nFilelenBytes - 1;
		//pck ansi -> unicode
		CPckClassCodepage::PckFilenameCode2UCS(lpPckIndexTable->cFileIndex.szFilename, lpPckIndexTable->cFileIndex.szwFilename, sizeof(lpPckIndexTable->cFileIndex.szwFilename) / sizeof(wchar_t));
		++lpPckIndexTable;
	}

}

//Calculate the number of valid files during reconstruction and exclude duplicate files
DWORD CPckClassIndex::ReCountFiles()
{
	DWORD deNewFileCount = 0;
	LPPCKINDEXTABLE lpPckIndexTableSource = m_PckAllInfo.lpPckIndexTable;

	for(DWORD i = 0; i < m_PckAllInfo.dwFileCount; ++i) {

		if(!lpPckIndexTableSource->isInvalid) {
			++deNewFileCount;
		}
		++lpPckIndexTableSource;
	}
	return deNewFileCount;
}

//Fill the modified index data into the structure by version and compress it
LPPCKINDEXTABLE_COMPRESS CPckClassIndex::FillAndCompressIndexData(LPPCKINDEXTABLE_COMPRESS lpPckIndexTableComped, LPPCKFILEINDEX lpPckFileIndexToCompress)
{
	BYTE pckFileIndexBuf[MAX_INDEXTABLE_CLEARTEXT_LENGTH];
	lpPckIndexTableComped->dwIndexDataLength = MAX_INDEXTABLE_CLEARTEXT_LENGTH;

	m_zlib.compress(lpPckIndexTableComped->buffer, &lpPckIndexTableComped->dwIndexDataLength,
		m_PckAllInfo.lpSaveAsPckVerFunc->FillIndexData(lpPckFileIndexToCompress, pckFileIndexBuf), m_PckAllInfo.lpSaveAsPckVerFunc->dwFileIndexSize);
	//will be obtained
	lpPckIndexTableComped->dwIndexValueHead = lpPckIndexTableComped->dwIndexDataLength ^ m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys.IndexCompressedFilenameDataLengthCryptKey1;
	lpPckIndexTableComped->dwIndexValueTail = lpPckIndexTableComped->dwIndexDataLength ^ m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys.IndexCompressedFilenameDataLengthCryptKey2;

	return lpPckIndexTableComped;
}