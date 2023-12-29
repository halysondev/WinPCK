#pragma once

typedef unsigned __int64	QWORD;
#include <string>
#include "PckStructs.h"
#include "PckClassLog.h"
#include "MapViewFileMultiPck.h"
#include "PckClassBaseFeatures.h"


class CPckClassFileDisk :
	protected virtual CPckClassBaseFeatures
{
public:
	CPckClassFileDisk();
	~CPckClassFileDisk();

protected:
	BOOL		MakeFolderExist(const wchar_t* lpszDirectory);
private:
	BOOL		MakeFolderExistInternal(const wchar_t* lpszDirectory);
protected:
	BOOL		EnumAllFilesByPathList(const vector<wstring> &lpszFilePath, DWORD &_out_FileCount, QWORD &_out_TotalFileSize, vector<FILES_TO_COMPRESS> *lpFileLinkList);

	//Disk space required when rebuilding pck (target file name, size of PCK file to be rebuilt)
	QWORD	GetPckFilesizeRebuild(const wchar_t * lpszFilename, QWORD qwPckFilesize);

	//The size of the file required when renaming
	QWORD	GetPckFilesizeRename(const wchar_t * lpszFilename, QWORD qwCurrentPckFilesize);

	//When adding and creating a new file, the size of the pck file written
	QWORD	GetPckFilesizeByCompressed(const char* lpszFilename, QWORD qwToCompressFilesize, QWORD qwCurrentPckFilesize);
	QWORD	GetPckFilesizeByCompressed(const wchar_t * lpszFilename, QWORD qwToCompressFilesize, QWORD qwCurrentPckFilesize);

private:

	//qwCurrentPckFilesize is the size of the existing file, qwToAddSpace is the size that needs to be expanded, and the return value is (qwCurrentPckFilesize + the maximum size that can be expanded)
	QWORD	GetPckFilesizeByCompressed(QWORD qwDiskFreeSpace, QWORD qwToAddSpace, QWORD qwCurrentPckFilesize);
	//Traverse folders
	VOID	EnumFile(LPWSTR szFilename, BOOL IsPatition, DWORD &dwFileCount, vector<FILES_TO_COMPRESS> *lpFileLinkList, QWORD &qwTotalFileSize, size_t nLen);

};

