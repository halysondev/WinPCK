#include "MapViewFileMultiPck.h"

#define PKX_FILES_LIMIT	5	// The maximum number of pre-created .pkx files

#if _TEST_MAX_PCK_CELL
CMapViewFileMultiPckWrite::CMapViewFileMultiPckWrite(QWORD qwMaxPckSize):
	//alcalm_Max_PckFile_Size(100 * 1024 * 1024)
	m_Max_PckFile_Size(100 * 1024 * 1024)
{}
#else
CMapViewFileMultiPckWrite::CMapViewFileMultiPckWrite(QWORD qwMaxPckSize) :
	m_Max_PckFile_Size(qwMaxPckSize),
	m_Max_PkxFile_Size(0xfffffe00U)
{}
#endif


CMapViewFileMultiPckWrite::~CMapViewFileMultiPckWrite()
{}


BOOL CMapViewFileMultiPckWrite::OpenPck(LPCSTR lpszFilename, DWORD dwCreationDisposition, BOOL isNTFSSparseFile)
{
	BOOL rtn = FALSE;
	
	// Creates a .pck file first
	if (AddFile(lpszFilename, dwCreationDisposition, m_Max_PckFile_Size, isNTFSSparseFile)) {
		CHAR lpszBaseName[MAX_PATH], lpszPkxPath[MAX_PATH];

		GetBaseName(lpszBaseName, lpszFilename);

		// Then creates .pkx files up to the limit
		for (UINT i = 0; i < PKX_FILES_LIMIT; ++i) {
			GetPkxPath(lpszPkxPath, lpszBaseName, i);

			if (!AddFile(lpszPkxPath, dwCreationDisposition, m_Max_PkxFile_Size, isNTFSSparseFile)) {
				goto fin;
			}
		}

		rtn = TRUE;
	}

fin:
	m_uqwPckStructSize.qwValue = CMapViewFileMulti::GetFileSize();

	return rtn;
}

//OPEN_ALWAYS, CREATE_ALWAYES, OPEN_EXISTING
BOOL CMapViewFileMultiPckWrite::OpenPck(LPCWSTR lpszFilename, DWORD dwCreationDisposition, BOOL isNTFSSparseFile)
{
	BOOL rtn = FALSE;

	if (AddFile(lpszFilename, dwCreationDisposition, m_Max_PckFile_Size, isNTFSSparseFile)) {
		WCHAR lpszBaseName[MAX_PATH], lpszPkxPath[MAX_PATH];

		GetBaseName(lpszBaseName, lpszFilename);

		for (UINT i = 0; i < PKX_FILES_LIMIT; ++i) {
			GetPkxPath(lpszPkxPath, lpszBaseName, i);

			if (!AddFile(lpszPkxPath, dwCreationDisposition, m_Max_PkxFile_Size, isNTFSSparseFile)) {
				goto fin;
			}
		}

		rtn = TRUE;
	}

fin:
	m_uqwPckStructSize.qwValue = CMapViewFileMulti::GetFileSize();

	return rtn;
}


BOOL CMapViewFileMultiPckWrite::OpenPckAndMappingWrite(LPCSTR lpFileName, DWORD dwCreationDisposition, QWORD qdwSizeToMap, BOOL isNTFSSparseFile)
{
	if(!OpenPck(lpFileName, dwCreationDisposition, isNTFSSparseFile))
		return FALSE;

	if(!Mapping(qdwSizeToMap))
		return FALSE;

	return TRUE;
}

BOOL CMapViewFileMultiPckWrite::OpenPckAndMappingWrite(LPCWSTR lpFileName, DWORD dwCreationDisposition, QWORD qdwSizeToMap, BOOL isNTFSSparseFile)
{
	if(!OpenPck(lpFileName, dwCreationDisposition, isNTFSSparseFile))
		return FALSE;

	if(!Mapping(qdwSizeToMap))
		return FALSE;

	return TRUE;
}
