#include "PckClassWriteOperator.h"

//Rename file
BOOL CPckClassWriteOperator::RenameFilename()
{
	m_zlib.init_compressor(m_lpPckParams->dwCompressLevel);
	Logger.i(TEXT_LOG_RENAME);

	//The following is to create a file to save the compressed file
	CMapViewFileMultiPckWrite cFileWrite(m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys.dwMaxSinglePckSize);

	if(!cFileWrite.OpenPck(m_PckAllInfo.szFilename, OPEN_EXISTING)) {

		Logger_el(UCSTEXT(TEXT_OPENWRITENAME_FAIL), m_PckAllInfo.szFilename);
		assert(FALSE);
		return FALSE;
	}

	QWORD dwFileSize = GetPckFilesizeRename(m_PckAllInfo.szFilename, cFileWrite.GetFileSize());

	if(!cFileWrite.Mapping(dwFileSize)) {

		Logger_el(UCSTEXT(TEXT_CREATEMAPNAME_FAIL), m_PckAllInfo.szFilename);
		assert(FALSE);
		return FALSE;
	}

	//Write file index
	QWORD dwAddress = m_PckAllInfo.dwAddressOfFileEntry;

	WriteAllIndex(&cFileWrite, &m_PckAllInfo, dwAddress);
	
	WriteHeadAndTail(&cFileWrite, &m_PckAllInfo, dwAddress, FALSE);

	Logger.i(TEXT_LOG_WORKING_DONE);

	return TRUE;
}