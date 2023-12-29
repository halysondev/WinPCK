//////////////////////////////////////////////////////////////////////
// ZupClassExtract.cpp: used to parse the data in the zup file of Perfect World Company and display it in the List
// head File
//
// This program is written by Li Qiufeng/stsm/liqf
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2012.5.23
//////////////////////////////////////////////////////////////////////

#include "ZupClass.h"
#include <functional>

#pragma warning ( disable : 4267 )

BOOL CZupClass::GetSingleFileData(const PCKINDEXTABLE* const lpZupFileIndexTable, char *buffer, size_t sizeOfBuffer)
{

	const PCKINDEXTABLE* lpPckFileIndexTable = nullptr;

	//"element\" = 0x6d656c65, 0x5c746e656d656c65
	if(0x6d656c65 == *(uint32_t*)lpZupFileIndexTable->cFileIndex.szFilename) {

		lpPckFileIndexTable = GetBaseFileIndex(lpZupFileIndexTable, m_lpZupIndexTable);

		ulong_t	dwFileLengthDecompress2 = lpZupFileIndexTable->cFileIndex.dwFileClearTextSize;
		ulong_t	dwFileLengthDecompress1 = lpPckFileIndexTable->cFileIndex.dwFileClearTextSize;

		if(0 != sizeOfBuffer && sizeOfBuffer < dwFileLengthDecompress2) {
			dwFileLengthDecompress2 = sizeOfBuffer;
			if(sizeOfBuffer < dwFileLengthDecompress1)
				dwFileLengthDecompress1 = sizeOfBuffer;
		}

		char	*_cipherbuf = (char*)malloc(dwFileLengthDecompress1);

		if(NULL == _cipherbuf) {
			Logger_el(TEXT_MALLOC_FAIL);
			return FALSE;
		}

		if(CPckClass::GetSingleFileData(lpPckFileIndexTable, _cipherbuf, dwFileLengthDecompress1)) {

			if(m_zlib.check_zlib_header(_cipherbuf + 4)) {

				if(Z_OK != m_zlib.decompress_part((uint8_t*)buffer, &dwFileLengthDecompress2,
					(uint8_t*)_cipherbuf + 4, dwFileLengthDecompress1 - 4, lpZupFileIndexTable->cFileIndex.dwFileClearTextSize)) {
					if(lpZupFileIndexTable->cFileIndex.dwFileClearTextSize == lpZupFileIndexTable->cFileIndex.dwFileCipherTextSize) {
						memcpy(buffer, _cipherbuf + 4, dwFileLengthDecompress2);
					} else {
						assert(FALSE);
						Logger_el(TEXT_UNCOMPRESSDATA_FAIL, lpZupFileIndexTable->cFileIndex.szFilename);
					}
				}
			} else {
				memcpy(buffer, _cipherbuf + 4, dwFileLengthDecompress2);
			}

			free(_cipherbuf);

		} else {
			free(_cipherbuf);
			return FALSE;
		}


	} else {

		lpPckFileIndexTable = lpZupFileIndexTable;
		return CPckClass::GetSingleFileData(lpPckFileIndexTable, buffer, sizeOfBuffer);
	}

	return TRUE;


	//PCK_GETDATA pPckGetSingleFileData = std::bind((BOOL (CPckClass::*)(const PCKINDEXTABLE* const, char*, size_t))&CPckClass::GetSingleFileData, static_cast<CPckClass*>(this), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	//return ZupGetSingleFileData(lpZupFileIndexTable, buffer, sizeOfBuffer, pPckGetSingleFileData);
}

BOOL CZupClass::GetSingleFileData(LPVOID lpvoidFileRead, const PCKINDEXTABLE* const lpZupFileIndexTable, char *buffer, size_t sizeOfBuffer)
{

	const PCKINDEXTABLE* lpPckFileIndexTable = nullptr;

	//"element\" = 0x6d656c65, 0x5c746e656d656c65
	if (0x6d656c65 == *(uint32_t*)lpZupFileIndexTable->cFileIndex.szFilename) {

		lpPckFileIndexTable = GetBaseFileIndex(lpZupFileIndexTable, m_lpZupIndexTable);

		ulong_t	dwFileLengthDecompress2 = lpZupFileIndexTable->cFileIndex.dwFileClearTextSize;
		ulong_t	dwFileLengthDecompress1 = lpPckFileIndexTable->cFileIndex.dwFileClearTextSize;

		if (0 != sizeOfBuffer && sizeOfBuffer < dwFileLengthDecompress2) {
			dwFileLengthDecompress2 = sizeOfBuffer;
			if (sizeOfBuffer < dwFileLengthDecompress1)
				dwFileLengthDecompress1 = sizeOfBuffer;
		}

		char	*_cipherbuf = (char*)malloc(dwFileLengthDecompress1);

		if (NULL == _cipherbuf) {
			Logger_el(TEXT_MALLOC_FAIL);
			return FALSE;
		}

		if (CPckClass::GetSingleFileData(lpvoidFileRead, lpPckFileIndexTable, _cipherbuf, dwFileLengthDecompress1)) {

			if (m_zlib.check_zlib_header(_cipherbuf + 4)) {

				if (Z_OK != m_zlib.decompress_part((uint8_t*)buffer, &dwFileLengthDecompress2,
					(uint8_t*)_cipherbuf + 4, dwFileLengthDecompress1 - 4, lpZupFileIndexTable->cFileIndex.dwFileClearTextSize)) {
					if (lpZupFileIndexTable->cFileIndex.dwFileClearTextSize == lpZupFileIndexTable->cFileIndex.dwFileCipherTextSize) {
						memcpy(buffer, _cipherbuf + 4, dwFileLengthDecompress2);
					}
					else {
						assert(FALSE);
						Logger_el(TEXT_UNCOMPRESSDATA_FAIL, lpZupFileIndexTable->cFileIndex.szFilename);
					}
				}
			}
			else {
				memcpy(buffer, _cipherbuf + 4, dwFileLengthDecompress2);
			}

			free(_cipherbuf);

		}
		else {
			free(_cipherbuf);
			return FALSE;
		}


	}
	else {

		lpPckFileIndexTable = lpZupFileIndexTable;
		return CPckClass::GetSingleFileData(lpvoidFileRead, lpPckFileIndexTable, buffer, sizeOfBuffer);
	}

	return TRUE;

	//PCK_GETDATA pPckGetSingleFileData = std::bind((BOOL(CPckClass::*)(LPVOID, const PCKINDEXTABLE* const, char*, size_t))&CPckClass::GetSingleFileData, static_cast<CPckClass*>(this), lpvoidFileRead, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	//return ZupGetSingleFileData(lpZupFileIndexTable, buffer, sizeOfBuffer, pPckGetSingleFileData);
}