#pragma once
#include "MapViewFile.h"

#include <vector>

#ifdef _DEBUG
#define FLUSH_SIZE_THRESHOLD	(320 * 1024 * 1024)
#else
#define FLUSH_SIZE_THRESHOLD	(32 * 1024 * 1024)
#endif

class CMapViewFileMulti
{
protected:
	typedef struct _FILE_CELL
	{

		CMapViewFile*	lpMapView;

		QWORD	qwCellAddressBegin;
		QWORD	qwCellAddressEnd;
		QWORD	qwMaxCellSize;
		QWORD	qwCellSize;

		wchar_t	szFilename[MAX_PATH];

	}FILE_CELL, *LPFILE_CELL;

private:
	typedef struct _CROSS_VIEW
	{
		LPBYTE	lpBufferTarget;
		LPBYTE	lpBufferTargetPtr;
		LPVOID	lpMapAddress;
		size_t	size;
		int		iCellID;
	}CROSS_VIEW, *LPCROSS_VIEW;

public:
	CMapViewFileMulti();
	~CMapViewFileMulti();

	void	SetFilePointer(LONGLONG lDistanceToMove, DWORD dwMoveMethod = FILE_BEGIN);
	virtual	QWORD	GetFileSize();

	//void	UnmapView(LPVOID lpTargetAddress);
	void	UnmapViewAll();
	void	UnMaping();
	void	clear();

	DWORD	Read(LPVOID buffer, DWORD dwBytesToRead);

	//Get the disk name of the current file
	const char*	GetFileDiskName();

	//Return the number and size of cells
	DWORD	GetCellCount();
	DWORD	GetCellSize();

protected:
	//Current file pointer position
	UNQWORD	m_uqwCurrentPos;
	//Total size of currently opened files
	UNQWORD m_uqwFullSize;
	//The maximum size of currently available files
	UNQWORD m_uqwMaxSize;

	std::vector<FILE_CELL>	m_file_cell;
	std::vector<CROSS_VIEW>	m_cross_view;

	int	GetCellIDByPoint(QWORD qwPoint);

	LPBYTE	View(QWORD dwAddress, DWORD dwSize, BOOL isReadOnly);

private:
	BOOL	BuildCrossViewBuffer(LPBYTE lpCrossBuffer, LPBYTE &lpCrossBufferPtr, int cell_id, QWORD qwAddress, DWORD dwSize, BOOL isReadOnly);

};

class CMapViewFileMultiRead : public CMapViewFileMulti
{
public:
	CMapViewFileMultiRead();
	~CMapViewFileMultiRead();

	BOOL	AddFile(LPCSTR lpszFilename);
	BOOL	AddFile(LPCWSTR lpszFilename);

	BOOL	Mapping();
	LPBYTE	View(QWORD dwAddress, DWORD dwSize);

private:

	BOOL	AddFile(CMapViewFileRead *lpRead, LPCWSTR lpszFilename);

};

class CMapViewFileMultiWrite : public CMapViewFileMulti
{

public:
	CMapViewFileMultiWrite();
	~CMapViewFileMultiWrite();

	BOOL	AddFile(LPCSTR lpszFilename, DWORD dwCreationDisposition, QWORD qwMaxSize, BOOL isNTFSSparseFile = FALSE);
	BOOL	AddFile(LPCWSTR lpszFilename, DWORD dwCreationDisposition, QWORD qwMaxSize, BOOL isNTFSSparseFile = FALSE);

	BOOL	Mapping(QWORD dwMaxSize);
	LPBYTE	View(QWORD dwAddress, DWORD dwSize);

	BOOL	SetEndOfFile();

	DWORD	Write(LPVOID buffer, DWORD dwBytesToWrite);

	BOOL	Write2(QWORD dwAddress, const void* buffer, DWORD dwBytesToWrite);

private:

	BOOL	AddFile(CMapViewFileWrite *lpWrite, QWORD qwMaxSize, LPCWSTR lpszFilename);

	//qwCurrentPckFilesize is the size of the existing file, qwToAddSpace is the size that needs to be expanded, and the return value is (qwCurrentPckFilesize + the maximum size that can be expanded)
	QWORD	GetExpanedPckFilesize(QWORD qwDiskFreeSpace, QWORD qwToAddSpace, QWORD qwCurrentPckFilesize);

	//Compress and rebuild, expand space when compressed writing space is insufficient
	BOOL	IsNeedExpandWritingFile(
		QWORD dwWritingAddressPointer,
		QWORD dwFileSizeToWrite);

	BOOL	ViewAndWrite2(QWORD dwAddress, const void *  buffer, DWORD dwSize);
};


