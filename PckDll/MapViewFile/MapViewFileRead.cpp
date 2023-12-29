//////////////////////////////////////////////////////////////////////
// MapViewFileRead.cpp: used to map file view (read)
//
//
// This program is written by Li Qiufeng/stsm/liqf
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2014.4.24
//////////////////////////////////////////////////////////////////////

#include "MapViewFile.h"

CMapViewFileRead::CMapViewFileRead()
{}

CMapViewFileRead::~CMapViewFileRead()
{}

BOOL CMapViewFileRead::Open(LPCSTR lpszFilename)
{
	return CMapViewFile::Open(lpszFilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS);
}

BOOL CMapViewFileRead::Open(LPCWSTR lpszFilename)
{
	return CMapViewFile::Open(lpszFilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS);
}

BOOL CMapViewFileRead::Mapping()
{
	if(NULL == (hFileMapping = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, 0, GenerateMapName()))) {
		assert(FALSE);
		return FALSE;
	}

	return TRUE;
}

LPBYTE CMapViewFileRead::View(QWORD qdwAddress, DWORD dwSize)
{
	return ViewReal(qdwAddress, dwSize, FILE_MAP_READ);
}


LPBYTE CMapViewFileRead::ReView(LPVOID lpMapAddressOld, QWORD dwAddress, DWORD dwSize)
{
	UnmapView(lpMapAddressOld);
	return View(dwAddress, dwSize);
}

BOOL CMapViewFileRead::OpenMappingRead(LPCSTR lpFileName)
{
	if(!(Open(lpFileName)))
		return FALSE;

	if(!(Mapping()))
		return FALSE;

	return TRUE;
}

BOOL CMapViewFileRead::OpenMappingRead(LPCWSTR lpFileName)
{
	if(!(Open(lpFileName)))
		return FALSE;

	if(!(Mapping()))
		return FALSE;

	return TRUE;
}

LPBYTE CMapViewFileRead::OpenMappingViewAllRead(LPCSTR lpFileName)
{
	if(OpenMappingRead(lpFileName))
		return View(0, 0);
	else
		return NULL;
}

LPBYTE CMapViewFileRead::OpenMappingViewAllRead(LPCWSTR lpFileName)
{
	if(OpenMappingRead(lpFileName))
		return View(0, 0);
	else
		return NULL;
}