#pragma once

#include "MapViewFileMulti.h"

#ifdef _DEBUG
#define _TEST_MAX_PCK_CELL 0
#else
#define _TEST_MAX_PCK_CELL 0
#endif

class CMapViewFileMultiPck
{
public:
	CMapViewFileMultiPck();
	~CMapViewFileMultiPck();

	//BOOL	CheckPckPackSize(QWORD qwPckSize);
	
protected:
	/**
	 * @brief Returns the basename of the file. (path without file extension)
	 * 
	 * @param lpszDest		Destination buffer
	 * @param lpszPckPath	Path to the file (.pck file)
	 */
	inline void GetBaseName(LPSTR lpszDest, LPCSTR lpszPckPath)
	{
	    SIZE_T ulLen = strrchr(lpszPckPath, '.') - lpszPckPath;
	    strncpy(lpszDest, lpszPckPath, ulLen);
	    lpszDest[ulLen] = '\0';
	}
	
	inline void GetBaseName(LPWSTR lpszDest, LPCWSTR lpszPckPath)
	{
	    SIZE_T ulLen = wcsrchr(lpszPckPath, L'.') - lpszPckPath;
	    wcsncpy(lpszDest, lpszPckPath, ulLen);
	    lpszDest[ulLen] = L'\0';
	}
	
	/**
	 * @brief Returns the .pkxN path.
	 *
	 * @param lpszDest		Destination buffer
	 * @param lpszBaseName	Basename of the (.pck) file
	 * @param uiNum			The .pkx file number
	 */
	void GetPkxPath(LPSTR lpszDest, LPCSTR lpszBaseName, UINT uiNum);
	void GetPkxPath(LPWSTR lpszDest, LPCWSTR lpszBaseName, UINT uiNum);

protected:
	//File size in PCK header structure
	UNQWORD m_uqwPckStructSize;
};


class CMapViewFileMultiPckRead : 
	public CMapViewFileMultiRead,
	public CMapViewFileMultiPck
{
public:
	CMapViewFileMultiPckRead();
	~CMapViewFileMultiPckRead();

	BOOL OpenPck(LPCSTR lpszFilename);
	BOOL OpenPck(LPCWSTR lpszFilename);

	BOOL	OpenPckAndMappingRead(LPCSTR lpFileName);
	BOOL	OpenPckAndMappingRead(LPCWSTR lpFileName);

	//LPBYTE OpenMappingAndViewAllRead(LPCSTR lpFileName);
	//LPBYTE OpenMappingAndViewAllRead(LPCWSTR lpFileName);

	QWORD	GetFileSize();

private:

};

class CMapViewFileMultiPckWrite:
	public CMapViewFileMultiWrite,
	public CMapViewFileMultiPck
{
public:
	CMapViewFileMultiPckWrite(QWORD qwMaxPckSize);
	~CMapViewFileMultiPckWrite();

	BOOL OpenPck(LPCSTR lpszFilename, DWORD dwCreationDisposition, BOOL isNTFSSparseFile = FALSE);
	BOOL OpenPck(LPCWSTR lpszFilename, DWORD dwCreationDisposition, BOOL isNTFSSparseFile = FALSE);

	BOOL	OpenPckAndMappingWrite(LPCSTR lpFileName, DWORD dwCreationDisposition, QWORD qdwSizeToMap, BOOL isNTFSSparseFile = FALSE);
	BOOL	OpenPckAndMappingWrite(LPCWSTR lpFileName, DWORD dwCreationDisposition, QWORD qdwSizeToMap, BOOL isNTFSSparseFile = FALSE);

private:

	QWORD	m_Max_PckFile_Size;
	QWORD	m_Max_PkxFile_Size;
};
