#include "MapViewFileMultiPck.h"


CMapViewFileMultiPckRead::CMapViewFileMultiPckRead()
{}

CMapViewFileMultiPckRead::~CMapViewFileMultiPckRead()
{}

BOOL CMapViewFileMultiPckRead::OpenPck(LPCSTR lpszFilename)
{
	BOOL rtn = FALSE;
	
	// Reads the .pck file
	if (AddFile(lpszFilename)) {
		UINT uiNum = 0;
		CHAR lpszBaseName[MAX_PATH], lpszPkxPath[MAX_PATH];
		
		GetBaseName(lpszBaseName, lpszFilename);
		
		// Then the .pkx files while they exist
		while (1) {
			GetPkxPath(lpszPkxPath, lpszBaseName, uiNum++);
			
			if (!AddFile(lpszPkxPath)) {
				SetLastError(NOERROR);
				break;
			}
		}

		rtn = TRUE;
	}
	
	m_uqwPckStructSize.qwValue = CMapViewFileMulti::GetFileSize();
	
	return rtn;
}

BOOL CMapViewFileMultiPckRead::OpenPck(LPCWSTR lpszFilename)
{
	BOOL rtn = FALSE;
	
	if (AddFile(lpszFilename)) {
		UINT uiNum = 0;
		WCHAR lpszBaseName[MAX_PATH], lpszPkxPath[MAX_PATH];
		
		GetBaseName(lpszBaseName, lpszFilename);
		
		while (1) {
			GetPkxPath(lpszPkxPath, lpszBaseName, uiNum++);
			
			if (!AddFile(lpszPkxPath)) {
				SetLastError(NOERROR);
				break;
			}
		}
		
		rtn = TRUE;
	}

	m_uqwPckStructSize.qwValue = CMapViewFileMulti::GetFileSize();
	
	return rtn;
}

BOOL CMapViewFileMultiPckRead::OpenPckAndMappingRead(LPCSTR lpFileName)
{
	if(!(OpenPck(lpFileName)))
		return FALSE;

	if(!(Mapping()))
		return FALSE;

	return TRUE;
}

BOOL CMapViewFileMultiPckRead::OpenPckAndMappingRead(LPCWSTR lpFileName)
{
	if(!(OpenPck(lpFileName)))
		return FALSE;

	if(!(Mapping()))
		return FALSE;

	return TRUE;
}

#if 0
LPBYTE CMapViewFileMultiPckRead::OpenMappingAndViewAllRead(LPCSTR lpFileName)
{
	if(OpenPckAndMappingRead(lpFileName))
		return View(0, 0);
	else
		return NULL;
}

LPBYTE CMapViewFileMultiPckRead::OpenMappingAndViewAllRead(LPCWSTR lpFileName)
{
	if(OpenPckAndMappingRead(lpFileName))
		return View(0, 0);
	else
		return NULL;
}
#endif

QWORD CMapViewFileMultiPckRead::GetFileSize()
{
	return m_uqwPckStructSize.qwValue;
}