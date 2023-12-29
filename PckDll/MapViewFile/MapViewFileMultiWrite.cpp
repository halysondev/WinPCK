#include "MapViewFileMulti.h"
#include "CharsCodeConv.h"

CMapViewFileMultiWrite::CMapViewFileMultiWrite()
{}

CMapViewFileMultiWrite::~CMapViewFileMultiWrite()
{}


BOOL CMapViewFileMultiWrite::AddFile(CMapViewFileWrite *lpWrite, QWORD qwMaxSize, LPCWSTR lpszFilename)
{
	size_t nCellCount = m_file_cell.size();

	FILE_CELL cFileCell = { 0 };

	wcscpy_s(cFileCell.szFilename, lpszFilename);

	cFileCell.lpMapView = lpWrite;
	cFileCell.qwCellSize = lpWrite->GetFileSize();
#if 0
	if((0 != cFileCell.qwCellSize) && (0 != nCellCount)){

		LPFILE_CELL lpFileCell = &m_file_cell.at(nCellCount - 1);
		lpFileCell->qwMaxCellSize = lpFileCell->qwCellSize;
		lpFileCell->qwCellAddressEnd = lpFileCell->qwCellAddressBegin + lpFileCell->qwCellSize;
		m_uqwMaxSize.qwValue = m_uqwFullSize.qwValue;
	}
#endif
	cFileCell.qwCellAddressBegin = m_uqwMaxSize.qwValue;
	cFileCell.qwCellAddressEnd = m_uqwMaxSize.qwValue + qwMaxSize;
	cFileCell.qwMaxCellSize = qwMaxSize;

	m_file_cell.push_back(cFileCell);

	m_uqwFullSize.qwValue += cFileCell.qwCellSize;
	m_uqwMaxSize.qwValue += qwMaxSize;

	return TRUE;
}

BOOL CMapViewFileMultiWrite::AddFile(LPCSTR lpszFilename, DWORD dwCreationDisposition, QWORD qwMaxSize, BOOL isNTFSSparseFile)
{
	CMapViewFileWrite *lpWrite = new CMapViewFileWrite();

	if(!lpWrite->Open(lpszFilename, dwCreationDisposition, isNTFSSparseFile)) {

		delete lpWrite;
		return FALSE;
	}
	CAnsi2Ucs cA2U(CP_ACP);

	return AddFile(lpWrite, qwMaxSize, cA2U.GetString(lpszFilename));
}

BOOL CMapViewFileMultiWrite::AddFile(LPCWSTR lpszFilename, DWORD dwCreationDisposition, QWORD qwMaxSize, BOOL isNTFSSparseFile)
{
	CMapViewFileWrite *lpWrite = new CMapViewFileWrite();

	if(!lpWrite->Open(lpszFilename, dwCreationDisposition, isNTFSSparseFile)) {

		delete lpWrite;
		return FALSE;
	}
	return AddFile(lpWrite, qwMaxSize, lpszFilename);
}

BOOL CMapViewFileMultiWrite::Mapping(QWORD dwMaxSize)
{
	size_t nCellCount = m_file_cell.size();

	int iCellID = GetCellIDByPoint(dwMaxSize);
	QWORD	qwLastExpandSize = dwMaxSize - m_file_cell[iCellID].qwCellAddressBegin;

	for(int i = 0;i < iCellID;i++) {

		CMapViewFileWrite *lpWrite = (CMapViewFileWrite*)m_file_cell[i].lpMapView;

		if(!lpWrite->Mapping(m_file_cell[i].qwMaxCellSize)) {
			return FALSE;
		}
		m_file_cell[i].qwCellSize = m_file_cell[i].qwMaxCellSize;
	}

	if (0 != qwLastExpandSize) {
		CMapViewFileWrite *lpWrite = (CMapViewFileWrite*)m_file_cell[iCellID].lpMapView;

		if (!lpWrite->Mapping(qwLastExpandSize)) {
			return FALSE;
		}

		m_file_cell[iCellID].qwCellSize = qwLastExpandSize;

	}

	if(dwMaxSize > m_uqwFullSize.qwValue)
		m_uqwFullSize.qwValue = dwMaxSize;

	return TRUE;
}

LPBYTE CMapViewFileMultiWrite::View(QWORD dwAddress, DWORD dwSize)
{
	return CMapViewFileMulti::View(dwAddress, dwSize, FALSE);
}

BOOL CMapViewFileMultiWrite::SetEndOfFile()
{
	BOOL rtn = TRUE;
	size_t nCellCount = m_file_cell.size();

	for(int i = 0;i < nCellCount;i++) {
		if(!((CMapViewFileWrite*)m_file_cell[i].lpMapView)->SetEndOfFile()) {
			rtn = FALSE;
		}
		m_file_cell[i].qwCellSize = m_file_cell[i].lpMapView->GetFileSize();
	}
	m_uqwFullSize.qwValue = m_uqwCurrentPos.qwValue;
	return rtn;
}

DWORD CMapViewFileMultiWrite::Write(LPVOID buffer, DWORD dwBytesToWrite)
{
	size_t nCellCount = m_file_cell.size();
	int iCellIDBegin = GetCellIDByPoint(m_uqwCurrentPos.qwValue);
	LPBYTE buffer2write = (LPBYTE)buffer;

	DWORD dwBytesLeft = dwBytesToWrite;
	DWORD dwBytesWriteAll = 0;

	while((0 < dwBytesLeft) && (nCellCount >iCellIDBegin)) {

		DWORD dwBytesWrite = ((CMapViewFileWrite*)m_file_cell[iCellIDBegin].lpMapView)->Write(buffer2write, dwBytesLeft);

		dwBytesLeft -= dwBytesWrite;
		buffer2write += dwBytesWrite;
		dwBytesWriteAll += dwBytesWrite;
	}
	m_uqwCurrentPos.qwValue += dwBytesWriteAll;
	return dwBytesWriteAll;
}

//When creating, if the amount of remaining file space is not enough, add the amount
#define	PCK_STEP_ADD_SIZE				(64*1024*1024)
//When the amount of space remaining in the file is less than this value when created, the data is expanded.
#define	PCK_SPACE_DETECT_SIZE			(4*1024*1024)
//The value of the extended data when the amount of remaining file space is less than this value (PCK_SPACE_DETECT_SIZE) when created
//#define PCK_RENAME_EXPAND_ADD			(16*1024*1024)

//qwCurrentPckFilesize is the size of the existing file, qwToAddSpace is the size that needs to be expanded, and the return value is (qwCurrentPckFilesize + the maximum size that can be expanded)
QWORD CMapViewFileMultiWrite::GetExpanedPckFilesize(QWORD qwDiskFreeSpace, QWORD qwToAddSpace, QWORD qwCurrentPckFilesize)
{
	//Calculate how much space is required qwTotalFileSize
	QWORD	qwTotalFileSizeTemp = qwToAddSpace;

	//if (-1 != qwDiskFreeSpace) {

		//If the requested space is less than the remaining disk space, the requested file space size is equal to the remaining disk space.
		if (qwDiskFreeSpace < qwTotalFileSizeTemp)
			qwTotalFileSizeTemp = qwDiskFreeSpace;
	//}

	return (qwTotalFileSizeTemp + qwCurrentPckFilesize);

}

BOOL CMapViewFileMultiWrite::IsNeedExpandWritingFile(
	QWORD dwWritingAddressPointer,
	QWORD dwFileSizeToWrite)
{
	//Judge whether the value of dwAddress will exceed dwTotalFileSizeAfterCompress
	//If it exceeds, it means that the file space applied for is too small, re-apply for ReCreateFileMapping
	//The new file size is increased from the original one (lpfirstFile->dwFileSize + 1mb) >= 64mb? (lpfirstFile->dwFileSize + 1mb): 64mb
	//1mb=0x100000
	//64mb=0x4000000

	QWORD qwOldFileSize = m_uqwFullSize.qwValue;

	if ((dwWritingAddressPointer + dwFileSizeToWrite + PCK_SPACE_DETECT_SIZE) > qwOldFileSize) {

		//Print log
		//CPckClassLog			m_PckLogFD;

		QWORD qwSizeToExpand = ((dwFileSizeToWrite + PCK_SPACE_DETECT_SIZE) > PCK_STEP_ADD_SIZE ? (dwFileSizeToWrite + PCK_SPACE_DETECT_SIZE) : PCK_STEP_ADD_SIZE);
		ULARGE_INTEGER lpfree;

		if (GetDiskFreeSpaceExA(GetFileDiskName(), NULL, NULL, &lpfree)) {

			qwSizeToExpand = GetExpanedPckFilesize(lpfree.QuadPart, qwSizeToExpand, 0);
		}
		else {
			//qwSizeToExpand = GetExpanedPckFilesize(-1, qwSizeToExpand, 0);
		}

		if (dwFileSizeToWrite > qwSizeToExpand) {
			//m_PckLogFD.PrintLogW("Insufficient disk space, requested space: %d, remaining space: %d", dwFileSizeToWrite, qwSizeToExpand);
			//SetErrMsgFlag(PCK_ERR_DISKFULL);
			return FALSE;
		}

		QWORD qwNewExpectedFileSize = qwOldFileSize + qwSizeToExpand;

		UnmapViewAll();
		UnMaping();

		if (!Mapping(qwNewExpectedFileSize)) {

			//m_PckLogFD.PrintLogW(TEXT_VIEWMAP_FAIL);
			//SetErrMsgFlag(PCK_ERR_VIEWMAP_FAIL);
			Mapping(qwOldFileSize);
			return FALSE;
		}
		//dwExpectedTotalCompressedFileSize = qwNewExpectedFileSize;
	}
	return TRUE;
}

BOOL CMapViewFileMultiWrite::ViewAndWrite2(QWORD dwAddress, const void *  buffer, DWORD dwSize)
{
	LPVOID pViewAddress = NULL;

	if (0 == dwSize)
		return TRUE;

	QWORD	dwAddressEndAt = dwAddress + dwSize;
	int iCellIDBegin = GetCellIDByPoint(dwAddress);
	int iCellIDEnd = GetCellIDByPoint(dwAddressEndAt);

	if ((-1 == iCellIDBegin) || (-1 == iCellIDEnd))
		return FALSE;

	QWORD	dwRealAddress = dwAddress - m_file_cell[iCellIDBegin].qwCellAddressBegin;

	if (iCellIDBegin == iCellIDEnd) {

		if (NULL != (pViewAddress = m_file_cell[iCellIDBegin].lpMapView->View(dwRealAddress, dwSize))) {

			memcpy(pViewAddress, buffer, dwSize);
			return TRUE;
		}

		return FALSE;
	}
	else {

		//iCellIDBegin iCellIDMid iCellIDMid iCellIDEnd
		int iCellIDMidCount = iCellIDEnd - iCellIDBegin - 1;
		if (0 > iCellIDMidCount)
			return FALSE;

		//Calculate the required size of each file
		size_t sizeBegin = m_file_cell[iCellIDBegin].qwCellAddressEnd - dwAddress;
		size_t sizeEnd = dwAddress + dwSize - m_file_cell[iCellIDEnd].qwCellAddressBegin;

		const BYTE* lpBuffer = (LPBYTE)buffer;

		if (NULL == (pViewAddress = m_file_cell[iCellIDBegin].lpMapView->View(dwRealAddress, sizeBegin))) 
			return FALSE;
		

		memcpy(pViewAddress, lpBuffer, sizeBegin);
		lpBuffer += sizeBegin;
		
		if (0 < iCellIDMidCount) {
			for (int i = (iCellIDBegin + 1); i < iCellIDEnd; i++) {

				if (NULL == (pViewAddress = m_file_cell[i].lpMapView->View(0, m_file_cell[i].qwCellSize))) 
					return FALSE;
				
				memcpy(pViewAddress, lpBuffer, m_file_cell[i].qwCellSize);
				lpBuffer += m_file_cell[i].qwCellSize;

			}
		}

		if (NULL == (pViewAddress = m_file_cell[iCellIDEnd].lpMapView->View(0, sizeEnd))) 
			return FALSE;
		
		memcpy(pViewAddress, lpBuffer, sizeEnd);
		//lpBuffer += sizeEnd;

		return TRUE;
	}
	return TRUE;
}


//Calculate the required size of each file
BOOL CMapViewFileMultiWrite::Write2(QWORD dwAddress, const void* buffer, DWORD dwBytesToWrite)
{
	static int nBytesWriten = 0;
	//PCK_STEP_ADD_SIZE
	if (!IsNeedExpandWritingFile(dwAddress, dwBytesToWrite)) {
		return FALSE;
	}

	if (!ViewAndWrite2(dwAddress, buffer, dwBytesToWrite)) {
		return FALSE;
	}

	nBytesWriten += dwBytesToWrite;
	if (FLUSH_SIZE_THRESHOLD < nBytesWriten)
	{
		for (int i = 0; i < m_file_cell.size(); i++) {
			m_file_cell[i].lpMapView->FlushFileBuffers();
		}
		
		nBytesWriten = 0;
	}

	UnmapViewAll();

	return TRUE;
}
