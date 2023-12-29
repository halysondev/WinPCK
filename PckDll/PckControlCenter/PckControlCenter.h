//////////////////////////////////////////////////////////////////////
// CPckControlCenter.h: used to parse the data in the pck file of Perfect World Company and display it in the List
// Header file, data interaction between interface and PCK class, control center
//
// This program is written by Li Qiufeng/stsm/liqf. The pck structure refers to Ruoshui's pck structure.txt, and
// Refer to the part of its Yi language code and read the pck file list
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2012.10.10
//////////////////////////////////////////////////////////////////////

#pragma once
#include <string>
#include "PckStructs.h"
#include "PckClassLog.h"
#include <vector>

typedef struct _PCK_PATH_NODE * LPPCK_PATH_NODE;
typedef struct _PCK_RUNTIME_PARAMS * LPPCK_RUNTIME_PARAMS;

#if 0
#ifdef _WINDLL	//.dll
#define EXPORT_CLASS _declspec(dllexport) 
#elif defined(_DLL)	//.exe
#define EXPORT_CLASS _declspec(dllimport)
#else	//other
#define EXPORT_CLASS 
#endif
#else
#define EXPORT_CLASS 
#endif

class CPckClass;
class CPckClassLog;

class EXPORT_CLASS CPckControlCenter
{
	//function
public:
	CPckControlCenter();
	virtual ~CPckControlCenter();

	void	New();

private:

	void	init();
	void	uninit();
	void	Reset(uint32_t dwUIProgressUpper = 1);
#pragma region Open and close files

	void	Close();
public:

	BOOL	Open(const wchar_t * lpszFile);

#pragma endregion

#pragma region Rename node

	//Rename a node
	BOOL	RenameEntry(LPENTRY lpFileEntry, LPCWSTR lpszReplaceString);
	//submit
	BOOL	RenameSubmit();

#pragma endregion

#pragma region Preview unzipped files

	//Preview file
	BOOL		GetSingleFileData(LPCENTRY lpFileEntry, char *buffer, size_t sizeOfBuffer = 0);

	//unzip files
	BOOL		ExtractFiles(const PCK_UNIFIED_FILE_ENTRY **lpFileEntryArray, int nEntryCount, LPCWSTR lpszDestDirectory);
	BOOL		ExtractAllFiles(LPCWSTR lpszDestDirectory);

#pragma endregion

#pragma region Rebuild pck file
	//Rebuild pck file
	BOOL	TestScript(LPCWSTR lpszScriptFile);
	BOOL	RebuildPckFile(LPCWSTR lpszScriptFile, LPCWSTR szRebuildPckFile, BOOL bUseRecompress);

#pragma endregion

#pragma region Game streamlined
	BOOL	StripPck(LPCWSTR lpszStripedPckFile, int flag);
#pragma endregion



#pragma region create or update pck file

	//Vector operation used when compressing multiple file lists
	void	StringArrayReset();
	void	StringArrayAppend(LPCWSTR lpszFilePath);
	BOOL	UpdatePckFileSubmit(LPCWSTR szPckFile, LPCENTRY lpFileEntry);

#pragma endregion

#pragma region Delete node
	//Delete a node
	BOOL	DeleteEntry(LPCENTRY lpFileEntry);
	//submit
	BOOL	DeleteEntrySubmit();

#pragma endregion


#pragma region Version related

	int		GetPckVersion();
	BOOL	SetPckVersion(int verID);
	//Get the current configuration name
	LPCWSTR	GetCurrentVersionName();
	static uint32_t	GetVersionCount();
	static LPCWSTR	GetVersionNameById(int verID);
	static int		AddVersionAlgorithmId(int AlgorithmId, int Version);

#pragma endregion

#pragma region Node attribute operations

	LPCENTRY GetRootNode();
	//Get node path
	static BOOL			GetCurrentNodeString(LPWSTR szCurrentNodePathString, LPCENTRY lpFileEntry);
	LPCENTRY			GetFileEntryByPath(LPCWSTR _in_szCurrentNodePathString);
#pragma endregion

#pragma region pckÎÄ¼þÊôÐÔ

	BOOL				IsValidPck();

	//Get the number of files
	uint32_t			GetPckFileCount();
	//actual file size
	uint64_t			GetPckSize();
	uint64_t			GetPckDataAreaSize();
	uint64_t			GetPckRedundancyDataSize();

	static uint64_t		GetFileSizeInEntry(LPCENTRY lpFileEntry);
	static uint64_t		GetCompressedSizeInEntry(LPCENTRY lpFileEntry);
	static uint32_t		GetFoldersCountInEntry(LPCENTRY lpFileEntry);
	static uint32_t		GetFilesCountInEntry(LPCENTRY lpFileEntry);

	static size_t		GetFilelenBytesOfEntry(LPCENTRY lpFileEntry);
	static size_t		GetFilelenLeftBytesOfEntry(LPCENTRY lpFileEntry);

	static uint64_t		GetFileOffset(LPCENTRY lpFileEntry);

	//Set extensions
	const char*			GetAdditionalInfo();
	static uint32_t		GetAdditionalInfoMaxSize();
	BOOL				SetAdditionalInfo(LPCSTR lpszAdditionalInfo);

	//Whether it is a file that supports updating
	BOOL				isSupportAddFileToPck();

#pragma endregion

	//Open, close, restore and other event registration
	static void		regMsgFeedback(void* pTag, FeedbackCallback _FeedbackCallBack);
	static int		DefaultFeedbackCallback(void* pTag, int32_t eventId, size_t wParam, ssize_t lParam);

#pragma region Inquiries and catalog browsing
private:
	static void		DefaultShowFilelistCallback(void* _in_param, int sn, LPCWSTR lpszFilename, int entry_type, uint64_t qwFileSize, uint64_t qwFileSizeCompressed, void* fileEntry);

public:
	uint32_t		SearchByName(LPCWSTR lpszSearchString, void* _in_param, SHOW_LIST_CALLBACK _showListCallback);
	static uint32_t	ListByNode(LPCENTRY lpFileEntry, void* _in_param, SHOW_LIST_CALLBACK _showListCallback);

#pragma endregion

#pragma region thread control
	//Thread running parameters
	BOOL			isThreadWorking();
	void			ForceBreakThreadWorking();
#pragma endregion

	//error no
	int				GetLastErrorMsg();
	BOOL			isLastOptSuccess();

#pragma region Memory usage
	//Memory usage
	uint32_t			getMTMemoryUsed();
	//memory value
	void			setMTMaxMemory(uint32_t dwMTMaxMemory);
	uint32_t			getMTMaxMemory();

	//Maximum memory
	static uint32_t	getMaxMemoryAllowed();

#pragma endregion

#pragma region Threads
	//Threads
	uint32_t	getMaxThread();
	void	setMaxThread(uint32_t dwThread);
	//Thread default parameters
	static uint32_t	getMaxThreadUpperLimit();
#pragma endregion

#pragma region Compression level

	//Compression level
	uint32_t	getCompressLevel();
	void	setCompressLevel(uint32_t dwCompressLevel = Z_DEFAULT_COMPRESS_LEVEL);

	//Compression level default parameters
	static uint32_t	getMaxCompressLevel();
	static uint32_t	getDefaultCompressLevel();
#pragma endregion

#pragma region Progress related

	uint32_t	getUIProgress();
	void	setUIProgress(uint32_t dwUIProgress);
	uint32_t	getUIProgressUpper();

#pragma endregion

#pragma region Print the results of adding/adding files

	uint32_t	GetUpdateResult_OldFileCount();
	uint32_t	GetUpdateResult_PrepareToAddFileCount();
	uint32_t	GetUpdateResult_ChangedFileCount();
	uint32_t	GetUpdateResult_DuplicateFileCount();
	uint32_t	GetUpdateResult_FinalFileCount();

#pragma endregion

private:

	FMTPCK	GetPckTypeFromFilename(const wchar_t * lpszFile);

	LPPCK_PATH_NODE				m_lpPckRootNode;
	std::vector<std::wstring>	lpszFilePathToAdd;	//Provide data when adding multiple files

	std::wstring				szUpdateResultString;

	PCK_RUNTIME_PARAMS			cParams;
	CPckClass					*m_lpClassPck;

	//Format
	FMTPCK						m_emunFileFormat;

	static FeedbackCallback		pFeedbackCallBack;
	static void*				pTag;

};