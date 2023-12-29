//////////////////////////////////////////////////////////////////////
// PckClass.h: used to parse the data in the pck file of Perfect World Company and display it in the List
// head File
//
// This program is written by Li Qiufeng/stsm/liqf. The pck structure refers to Ruoshui's pck structure.txt, and
// Refer to the part of its Yi language code and read the pck file list
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2012.4.10
//////////////////////////////////////////////////////////////////////

#include <assert.h>
#include "pck_default_vars.h"

#include "PckClassVersionDetect.h"
#include "PckClassWriteOperator.h"

#if !defined(_PCKCLASS_H_)
#define _PCKCLASS_H_

class CPckClass : 
	public virtual CPckClassWriteOperator,
	public virtual CPckClassVersionDetect
{
//function
public:
	CPckClass(LPPCK_RUNTIME_PARAMS inout);
	virtual ~CPckClass();

	virtual BOOL	Init(const wchar_t * szFile);

#pragma region PckClassExtract.cpp

	BOOL	ExtractFiles(const PCK_UNIFIED_FILE_ENTRY **lpFileEntryArray, int nEntryCount, const wchar_t *lpszDestDirectory);
	BOOL	ExtractAllFiles(const wchar_t *lpszDestDirectory);

private:
	//unzip files
	BOOL	ExtractFiles(const PCKINDEXTABLE **lpIndexToExtract, int nFileCount);
	BOOL	ExtractFiles(const PCK_PATH_NODE **lpNodeToExtract, int nFileCount);

public:
	//Preview file
	virtual BOOL	GetSingleFileData(const PCKINDEXTABLE* const lpPckFileIndexTable, char *buffer, size_t sizeOfBuffer = 0);
protected:
	virtual BOOL	GetSingleFileData(LPVOID lpvoidFileRead, const PCKINDEXTABLE* const lpPckFileIndexTable, char *buffer, size_t sizeOfBuffer = 0);
private:
	//PckClassExtract.cpp
	BOOL	StartExtract(LPPCK_PATH_NODE lpNodeToExtract, LPVOID lpMapAddress);
	BOOL	DecompressFile(const wchar_t * lpszFilename, const PCKINDEXTABLE* lpPckFileIndexTable, LPVOID lpvoidFileRead);
#pragma endregion

#pragma region PckClassMount.cpp
protected:
	//PckClass.cpp
	BOOL	MountPckFile(const wchar_t * szFile);
private:
	void	BuildDirTree();
#pragma endregion




};

#endif