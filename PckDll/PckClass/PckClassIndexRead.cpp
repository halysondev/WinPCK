#include "PckClassIndex.h"
#include "PckClassZlib.h"


BOOL CPckClassIndex::ReadPckFileIndexes()
{
	CMapViewFileMultiPckRead cRead;

	if(!cRead.OpenPckAndMappingRead(m_PckAllInfo.szFilename)) {
		Logger_el(UCSTEXT(TEXT_OPENNAME_FAIL), m_PckAllInfo.szFilename);
		return FALSE;
	}

	if(!(NULL != (m_PckAllInfo.lpPckIndexTable = AllocPckIndexTableByFileCount()))) {
		return FALSE;
	}

	//Start reading file
	BYTE *lpFileBuffer;
	if(NULL == (lpFileBuffer = cRead.View(m_PckAllInfo.dwAddressOfFileEntry, cRead.GetFileSize() - m_PckAllInfo.dwAddressOfFileEntry))) {
		//Logger_el(TEXT_VIEWMAP_FAIL);
		return FALSE;
	}

	LPPCKINDEXTABLE lpPckIndexTable = m_PckAllInfo.lpPckIndexTable;
	BOOL			isLevel0;
	DWORD			byteLevelKey;
	//Stores two DWORD compressed data length information in the header of each file index
	DWORD			dwFileIndexTableCryptedDataLength[2];
	DWORD			dwFileIndexTableClearDataLength = m_PckAllInfo.lpDetectedPckVerFunc->dwFileIndexSize;
	DWORD			IndexCompressedFilenameDataLengthCryptKey[2] = { \
		m_PckAllInfo.lpDetectedPckVerFunc->cPckXorKeys.IndexCompressedFilenameDataLengthCryptKey1, \
		m_PckAllInfo.lpDetectedPckVerFunc->cPckXorKeys.IndexCompressedFilenameDataLengthCryptKey2 };

	//When pck is compressed, the compressed length of the file name will not exceed 0x100, so when
	//Start a byte. If it is 0x75, there is no compression. If it is 0x74, it is compressed. 0x75->FILEINDEX_LEVEL0
	//cRead.SetFilePointer(m_PckAllInfo.dwAddressOfFileEntry, FILE_BEGIN);

	byteLevelKey = (*(DWORD*)lpFileBuffer) ^ IndexCompressedFilenameDataLengthCryptKey[0];
	isLevel0 = (m_PckAllInfo.lpDetectedPckVerFunc->dwFileIndexSize == byteLevelKey)/* ? TRUE : FALSE*/;

	if(isLevel0) {

		for(DWORD i = 0;i < m_PckAllInfo.dwFileCount;i++) {
			//First copy the two compressed data length information
			memcpy(dwFileIndexTableCryptedDataLength, lpFileBuffer, 8);

			*(QWORD*)dwFileIndexTableCryptedDataLength ^= *(QWORD*)IndexCompressedFilenameDataLengthCryptKey;

			if(dwFileIndexTableCryptedDataLength[0] != dwFileIndexTableCryptedDataLength[1]) {

				Logger_el(TEXT_READ_INDEX_FAIL);
				return FALSE;
			}

			lpFileBuffer += 8;

			m_PckAllInfo.lpDetectedPckVerFunc->PickIndexData(&lpPckIndexTable->cFileIndex, lpFileBuffer);
			lpFileBuffer += dwFileIndexTableClearDataLength;
			++lpPckIndexTable;

		}
	} else {

		for(DWORD i = 0;i < m_PckAllInfo.dwFileCount;++i) {

			memcpy(dwFileIndexTableCryptedDataLength, lpFileBuffer, 8);
			*(QWORD*)dwFileIndexTableCryptedDataLength ^= *(QWORD*)IndexCompressedFilenameDataLengthCryptKey;
			lpFileBuffer += 8;

			if(dwFileIndexTableCryptedDataLength[0] != dwFileIndexTableCryptedDataLength[1]) {

				Logger_el(TEXT_READ_INDEX_FAIL);
				return FALSE;
			}
			
			DWORD dwFileBytesRead = MAX_INDEXTABLE_CLEARTEXT_LENGTH/*dwFileIndexTableClearDataLength*/;
			BYTE pckFileIndexBuf[MAX_INDEXTABLE_CLEARTEXT_LENGTH];

			m_zlib.decompress(pckFileIndexBuf, &dwFileBytesRead,
				lpFileBuffer, dwFileIndexTableCryptedDataLength[0]);

#if PCK_V2031_ENABLE
			/*
			The new Zhuxian index size has been changed to 288, and 4 bytes of new content have been added.
			PCKFILEINDEX_V2030->
			*/
			PCKFILEINDEX_V2031* testnewindex = (PCKFILEINDEX_V2031*)pckFileIndexBuf;
#endif

			m_PckAllInfo.lpDetectedPckVerFunc->PickIndexData(&lpPckIndexTable->cFileIndex, pckFileIndexBuf);

			lpFileBuffer += dwFileIndexTableCryptedDataLength[0];
			++lpPckIndexTable;

		}
	}

	return TRUE;
}