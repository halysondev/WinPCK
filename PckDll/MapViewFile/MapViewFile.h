#pragma once
//////////////////////////////////////////////////////////////////////
// MapViewFile.h: used to map file views
//
//
// This program is written by Li Qiufeng/stsm/liqf
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2012.4.10
//////////////////////////////////////////////////////////////////////

#include "gccException.h"
#include <windows.h>
#include <assert.h>
#include <vector>

#define TEST_T 1
/*
#define CREATE_NEW          1
#define CREATE_ALWAYS       2
#define OPEN_EXISTING       3
#define OPEN_ALWAYS         4
#define TRUNCATE_EXISTING   5

#define INVALID_FILE_SIZE ((DWORD)0xFFFFFFFF)
#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
*/

#if _MSC_VER >= 1400
#pragma warning ( disable : 4996 )
#endif

#ifdef _WIN64
typedef unsigned __int64  uintptr_t;
#else
typedef unsigned int uintptr_t;
#endif

#if !defined(_MAPVIEWFILE_H_)
#define _MAPVIEWFILE_H_

typedef unsigned __int64	QWORD;

//LARGE_INTEGER
typedef union _QWORD
{
	QWORD qwValue;
	LONGLONG llwValue;
	struct
	{
		DWORD dwValue;
		DWORD dwValueHigh;
	};
	struct
	{
		LONG lValue;
		LONG lValueHigh;
	};
}UNQWORD, *LPUNQWORD;


class CMapViewFile
{
public:
	CMapViewFile();
	virtual ~CMapViewFile();

	BOOL FileExists(LPCSTR szName);
	BOOL FileExists(LPCWSTR szName);

	BOOL	Open(LPCSTR lpszFilename, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes);
	BOOL	Open(LPCWSTR lpszFilename, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes);

	void	SetFilePointer(QWORD lDistanceToMove, DWORD dwMoveMethod = FILE_BEGIN);
	QWORD	GetFilePointer();

	DWORD	Read(LPVOID buffer, DWORD dwBytesToRead);

	QWORD	GetFileSize();

	virtual LPBYTE	View(QWORD dwAddress, DWORD dwSize);
	//virtual LPBYTE	ReView(LPVOID lpMapAddressOld, QWORD dwAddress, DWORD dwSize);

	void	UnmapView(LPVOID lpTargetAddress);
	void	UnmapViewAll();
	void	UnMaping();
	void	clear();

	//Get the disk name of the current file
	const char*	GetFileDiskName();

	//Force cache to be written to disk
	virtual BOOL	FlushFileBuffers() { throw std::exception("programe can not reach here"); }
	
protected:
	////Automatically generate the name required when CreateFileMappingA
	LPCSTR	GenerateMapName();

	void MakeUnlimitedPath(LPWSTR _dst, LPCWSTR	_src, size_t size);
	void MakeUnlimitedPath(LPSTR _dst, LPCSTR _src, size_t size);

	template <typename T>
	void GetDiskNameFromFilename(T* lpszFilename);

	uint8_t*	ViewReal(QWORD qwAddress, DWORD dwSize, DWORD dwDesiredAccess);

protected:

	HANDLE	hFile;
	HANDLE	hFileMapping;
	//LPVOID	lpMapAddress;
	std::vector<void*> vMapAddress;


	//The disk corresponding to the file
	char	m_szDisk[8];

	//Used to store MapName
	char szFileMappingName[32];

private:
/*
Created as NTFS sparse file
The process is£ºCreateFile->SetSparseFile->CreateFileMapping->MapViewOfFile->UnmapViewOfFile->CloseHandle
*/
	virtual void	SetSparseFile() { throw std::exception("programe can not reach here"); }
};



class CMapViewFileRead : public CMapViewFile
{
public:
	CMapViewFileRead();
	virtual ~CMapViewFileRead();

	BOOL	Open(LPCSTR lpszFilename);
	BOOL	Open(LPCWSTR lpszFilename);

	BOOL	Mapping();

	LPBYTE	View(QWORD dwAddress, DWORD dwSize);
	virtual LPBYTE	ReView(LPVOID lpMapAddressOld, QWORD dwAddress, DWORD dwSize);

	BOOL	OpenMappingRead(LPCSTR lpFileName);
	BOOL	OpenMappingRead(LPCWSTR lpFileName);

	LPBYTE OpenMappingViewAllRead(LPCSTR lpFileName);
	LPBYTE OpenMappingViewAllRead(LPCWSTR lpFileName);

protected:


};


class CMapViewFileWrite : public CMapViewFile
{
public:
	CMapViewFileWrite();
	virtual ~CMapViewFileWrite();

	BOOL	Open(LPCSTR lpszFilename, DWORD dwCreationDisposition, BOOL isNTFSSparseFile = FALSE);
	BOOL	Open(LPCWSTR lpszFilename, DWORD dwCreationDisposition, BOOL isNTFSSparseFile = FALSE);

	BOOL	Mapping(QWORD dwMaxSize);

	LPBYTE	View(QWORD dwAddress, DWORD dwSize);
	virtual LPBYTE	ReView(LPVOID lpMapAddressOld, QWORD dwAddress, DWORD dwSize);

	BOOL	SetEndOfFile();

	DWORD	Write(LPVOID buffer, DWORD dwBytesToWrite);


	BOOL	OpenMappingWrite(LPCSTR lpFileName, DWORD dwCreationDisposition, QWORD qdwSizeToMap);
	BOOL	OpenMappingWrite(LPCWSTR lpFileName, DWORD dwCreationDisposition, QWORD qdwSizeToMap);

	virtual BOOL	FlushFileBuffers();

private:

	virtual void	SetSparseFile();
	//BOOL	Write2(QWORD dwAddress, LPVOID buffer, DWORD dwBytesToWrite);


};

#endif //_MAPVIEWFILE_H_