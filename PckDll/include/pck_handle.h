#include "pck_default_vars.h"

#ifndef APSTUDIO_READONLY_SYMBOLS
#include "pck_dependencies.h"
#endif

#ifndef WINPCK_DLL_H
#define WINPCK_DLL_H



typedef enum _PCKRTN
{
	WINPCK_OK = 0,			/* success */
	WINPCK_ERROR,			/* general error */
	WINPCK_WORKING,			/* Task running */
	WINPCK_INVALIDPCK,		/* Invalid PCK file or file not open */
	WINPCK_NOTFOUND,
}PCKRTN;

#ifdef __cplusplus
#define EXTERN_C       extern "C"
#define EXTERN_C_START extern "C" {
#define EXTERN_C_END   }
#else
#define EXTERN_C       extern
#define EXTERN_C_START
#define EXTERN_C_END
#endif


#ifdef _WINDLL	//.dll
#define WINPCK_API EXTERN_C _declspec(dllexport)
#elif defined(_DLL)	//.exe
#define WINPCK_API EXTERN_C _declspec(dllimport)
#else	//other
#define WINPCK_API EXTERN_C
#endif

WINPCK_API LPCSTR		pck_version();
WINPCK_API PCKRTN		pck_open(const wchar_t *  lpszFile);
WINPCK_API PCKRTN		pck_close();

//return -1 is invalid 
WINPCK_API int			pck_getVersion();
WINPCK_API PCKRTN		pck_setVersion(int verID);
WINPCK_API uint32_t		pck_getVersionCount();
WINPCK_API LPCWSTR 		pck_getVersionNameById(int verID);
//AlgorithmId can not be -1, Version must be AFPCK_VERSION_202(0x00020002) or AFPCK_VERSION_203(0x00020003), return value is id , PCK_VERSION_INVALID = -1
WINPCK_API int 			pck_addVersionAlgorithmId(int AlgorithmId, int Version);


//Get the current configuration name
WINPCK_API LPCWSTR 		pck_GetCurrentVersionName();

//Effective
WINPCK_API BOOL			pck_IsValidPck();

//Get node path
WINPCK_API BOOL			pck_getNodeRelativePath(LPWSTR _out_szCurrentNodePathString, LPCENTRY lpFileEntry);
WINPCK_API LPCENTRY		pck_getFileEntryByPath(LPWSTR _in_szCurrentNodePathString);

//Node query 
WINPCK_API LPCENTRY		pck_getRootNode();

//pck file size
WINPCK_API uint64_t		pck_filesize();
//pck effective data area size
WINPCK_API uint64_t		pck_file_data_area_size();
//Redundant data size
WINPCK_API uint64_t		pck_file_redundancy_data_size();
//pck file quantity
WINPCK_API uint32_t		pck_filecount();

//Get data from node
WINPCK_API uint64_t		pck_getFileSizeInEntry(LPCENTRY lpFileEntry);
WINPCK_API uint64_t		pck_getCompressedSizeInEntry(LPCENTRY lpFileEntry);
WINPCK_API uint32_t		pck_getFoldersCountInEntry(LPCENTRY lpFileEntry);
WINPCK_API uint32_t		pck_getFilesCountInEntry(LPCENTRY lpFileEntry);

//The current length of the current node file name
WINPCK_API uint32_t		pck_getFilelenBytesOfEntry(LPCENTRY lpFileEntry);
//Maximum length of current node file name - current length
WINPCK_API uint32_t		pck_getFilelenLeftBytesOfEntry(LPCENTRY lpFileEntry);


//Returns the offset value of the compressed data of the file in the pck file
WINPCK_API uint64_t		pck_getFileOffset(LPCENTRY lpFileEntry);

//Whether it is a file that supports updating
WINPCK_API BOOL			pck_isSupportAddFileToPck();

//Set extensions
WINPCK_API LPCSTR 		pck_GetAdditionalInfo();
WINPCK_API uint32_t		pck_GetAdditionalInfoMaxSize();
//
WINPCK_API PCKRTN		pck_SetAdditionalInfo(LPCSTR  lpszAdditionalInfo);

//Rename a node
WINPCK_API PCKRTN		pck_RenameEntry(LPENTRY lpFileEntry, LPCWSTR lpszReplaceString);
//submit
WINPCK_API PCKRTN		pck_RenameSubmit();
//use it like do_RenameEntry(L"x.pck", L"gfx\\test", L"testabc");
WINPCK_API PCKRTN		do_RenameEntry(LPCWSTR  szPckFile, LPCWSTR lpFullPathToRename, LPCWSTR lpszReplaceString);

//Preview file
WINPCK_API PCKRTN		pck_GetSingleFileData(LPCENTRY lpFileEntry, LPSTR _inout_buffer, size_t _in_sizeOfBuffer = 0);

//unzip files
WINPCK_API PCKRTN		pck_ExtractFilesByEntrys(LPCENTRY* lpFileEntryArray, int nEntryCount, LPCWSTR  lpszDestDirectory);
WINPCK_API PCKRTN		pck_ExtractAllFiles(LPCWSTR  lpszDestDirectory);
WINPCK_API PCKRTN		do_ExtractPartFiles(LPCWSTR  lpszSrcPckFile, LPCWSTR  lpszDestDirectory, LPCWSTR  lpszFileToExtract);
WINPCK_API PCKRTN		do_ExtractAllFiles(LPCWSTR  lpszSrcPckFile, LPCWSTR  lpszDestDirectory);

//Rebuild pck file
WINPCK_API PCKRTN		pck_TestScript(LPCWSTR  lpszScriptFile);
WINPCK_API PCKRTN		pck_RebuildPckFile(LPCWSTR szRebuildPckFile, BOOL bUseRecompress);
//set lpszScriptFile = NULL to disable Script filter function
WINPCK_API PCKRTN		pck_RebuildPckFileWithScript(LPCWSTR  lpszScriptFile, LPCWSTR szRebuildPckFile, BOOL bUseRecompress);
WINPCK_API PCKRTN		do_RebuildPckFileWithScript(LPCWSTR szSrcPckFile, LPCWSTR  lpszScriptFile, LPCWSTR szDstRebuildPckFile, BOOL bUseRecompress, int level = 9);
//Game streamlined
WINPCK_API PCKRTN		pck_StripPck(LPCWSTR szStripedPckFile, int flag);
WINPCK_API PCKRTN		do_StripPck(LPCWSTR szSrcPckFile, LPCWSTR szStripedPckFile, int flag, int level);

//Create and update pck files
WINPCK_API void			pck_StringArrayReset();
WINPCK_API void			pck_StringArrayAppend(LPCWSTR  lpszFilePath);
WINPCK_API PCKRTN		pck_UpdatePckFileSubmit(LPCWSTR  szPckFile, LPCENTRY lpFileEntry);
//Add files to pck
WINPCK_API PCKRTN		do_AddFileToPckFile(LPCWSTR  lpszFilePathSrc, LPCWSTR  szDstPckFile, LPCWSTR  lpszPathInPckToAdd, int level = 9);
//Create new pck file
WINPCK_API PCKRTN		do_CreatePckFile(LPCWSTR  lpszFilePathSrc, LPCWSTR  szDstPckFile, int _versionId = 0, int level = 9);

//Delete a node and apply changes via commit
WINPCK_API PCKRTN		pck_DeleteEntry(LPCENTRY lpFileEntry);
//submit
WINPCK_API PCKRTN		pck_DeleteEntrySubmit();
//Delete the corresponding file in pck through the pck file name and the file name to be deleted. There will be a commit operation after execution. count is the number of files to be deleted.
WINPCK_API PCKRTN		do_DeleteFromPck(LPCWSTR  szSrcPckFile, int count, ...);

//Inquiries and catalog browsing
//return = searched filecount
WINPCK_API uint32_t		pck_searchByName(LPCWSTR  lpszSearchString, void* _in_param, SHOW_LIST_CALLBACK showListCallback);
WINPCK_API uint32_t		pck_listByNode(LPCENTRY lpFileEntry, void* _in_param, SHOW_LIST_CALLBACK _showListCallback);
WINPCK_API PCKRTN		do_listPathInPck(LPCWSTR  szSrcPckFile, LPCWSTR  lpszListPath, void* _in_param, SHOW_LIST_CALLBACK _showListCallback);

//multi-threaded tasks
//yes -1, no - 0
WINPCK_API BOOL			pck_isThreadWorking();
WINPCK_API void			pck_forceBreakThreadWorking();
WINPCK_API BOOL			pck_isLastOptSuccess();
WINPCK_API BOOL			pck_getLastErrorMsg();

//Memory usage
WINPCK_API uint32_t		pck_getMTMemoryUsed();
WINPCK_API uint32_t		pck_getMTMaxMemory();
WINPCK_API void			pck_setMTMaxMemory(uint32_t dwMTMaxMemoryInBytes);
WINPCK_API uint32_t		pck_getMaxMemoryAllowed();
//Threads
WINPCK_API uint32_t		pck_getMaxThreadUpperLimit();
WINPCK_API uint32_t		pck_getMaxThread();
WINPCK_API void			pck_setMaxThread(uint32_t dwThread);
//Compression level
WINPCK_API uint32_t		pck_getMaxCompressLevel();
WINPCK_API uint32_t		pck_getDefaultCompressLevel();
WINPCK_API uint32_t		pck_getCompressLevel();
WINPCK_API void			pck_setCompressLevel(uint32_t dwCompressLevel);
//schedule
WINPCK_API uint32_t		pck_getUIProgress();
WINPCK_API void			pck_setUIProgress(uint32_t dwUIProgress);
WINPCK_API uint32_t		pck_getUIProgressUpper();


//Add/new file returns result list
WINPCK_API uint32_t		pck_getUpdateResult_OldFileCount();
WINPCK_API uint32_t		pck_getUpdateResult_PrepareToAddFileCount();
WINPCK_API uint32_t		pck_getUpdateResult_ChangedFileCount();
WINPCK_API uint32_t		pck_getUpdateResult_DuplicateFileCount();
WINPCK_API uint32_t		pck_getUpdateResult_FinalFileCount();

//log
WINPCK_API void			log_regShowFunc(ShowLogW _ShowLogCallBack);

WINPCK_API void			pck_logNA(LPCSTR  _fmt, ...);
WINPCK_API void			pck_logNW(LPCWSTR  _fmt, ...);

WINPCK_API void			pck_logIA(LPCSTR  _fmt, ...);
WINPCK_API void			pck_logIW(LPCWSTR  _fmt, ...);

WINPCK_API void			pck_logWA(LPCSTR  _fmt, ...);
WINPCK_API void			pck_logWW(LPCWSTR  _fmt, ...);

WINPCK_API void			pck_logEA(LPCSTR  _fmt, ...);
WINPCK_API void			pck_logEW(LPCWSTR  _fmt, ...);

WINPCK_API void			pck_logDA(LPCSTR  _fmt, ...);
WINPCK_API void			pck_logDW(LPCWSTR  _fmt, ...);

#ifdef UNICODE
#define pck_logN pck_logNW
#define pck_logI pck_logIW
#define pck_logW pck_logWW
#define pck_logE pck_logEW
#define pck_logD pck_logDW
#else
#define pck_logN pck_logNA
#define pck_logI pck_logIA
#define pck_logW pck_logWA
#define pck_logE pck_logEA
#define pck_logD pck_logDA
#endif

//Open, close, restore and other event registration
WINPCK_API void			pck_regMsgFeedback(void* pTag, FeedbackCallback _FeedbackCallBack);

#endif //WINPCK_DLL_H


