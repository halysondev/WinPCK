#pragma once
#include "PckClassHeadTailWriter.h"
#include "PckClassIndexWriter.h"
#include "PckClassFileDisk.h"
#include "PckThreadRunner.h"

class CPckClassWriteOperator :
	public virtual CPckClassHeadTailWriter,
	public virtual CPckClassIndexWriter,
	public virtual CPckClassFileDisk
{
public:
	CPckClassWriteOperator();
	~CPckClassWriteOperator();

	friend class CPckThreadRunner;

	//Rebuild pck file
	virtual BOOL	RebuildPckFile(const wchar_t * lpszScriptFile, const wchar_t * szRebuildPckFile, BOOL bUseRecompress);

protected:
	BOOL	RebuildPckFile(const wchar_t * szRebuildPckFile);
	BOOL	RecompressPckFile(const wchar_t * szRecompressPckFile, int isStripMode = PCK_STRIP_NONE);

public:
#pragma region Game streamlined
	virtual BOOL	StripPck(const wchar_t * lpszStripedPckFile, int flag);
#pragma endregion

#pragma region PckClassAppendFiles.cpp
public:
	//Create and update pck files
	virtual BOOL	UpdatePckFile(const wchar_t * szPckFile, const vector<wstring> &lpszFilePath, const PCK_PATH_NODE* lpNodeToInsert);
#pragma endregion
#pragma region PckClassRenamer.cpp

public:
	//Rename file
	virtual BOOL	RenameFilename();

#pragma endregion

private:
	vector<FILES_TO_COMPRESS>	m_FilesToBeAdded;
};

