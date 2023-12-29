//////////////////////////////////////////////////////////////////////
// PckStructs.h: used to parse the data in the pck file of Perfect World Company and display it in the List
// head File
//
// This program is written by Li Qiufeng/stsm/liqf
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2015.5.13
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "PckDefines.h"
#include <vector>

#if !defined(_PCKSTRUCTS_H_)
#define _PCKSTRUCTS_H_


typedef struct _PCK_KEYS
{
	int32_t			id;
	wchar_t			name[64];
	PCK_CATEGORY	CategoryId;
	uint32_t		Version;
	//head
	uint32_t		HeadVerifyKey1;
	uint32_t		HeadVerifyKey2;
	//tail
	uint32_t		TailVerifyKey1;
	uint64_t		IndexesEntryAddressCryptKey;
	uint32_t		TailVerifyKey2;	//The -3rd DWORD at the end of the file (xx xx xx xx 00 00 00 00 00 00 00 00)
	//index
	uint32_t		IndexCompressedFilenameDataLengthCryptKey1;//Each file in pck has an index stored at the end of the file (except the last 280 bytes)
	uint32_t		IndexCompressedFilenameDataLengthCryptKey2;//The data is len^key1, len^key2, data(PCKFILEINDEX), (11 11 11 11 22 22 22 22 dd dd dd dd .....)
	//pkx
	uint32_t		dwMaxSinglePckSize;
}PCK_KEYS, *LPPCK_KEYS;

typedef struct _PCK_VERSION_FUNC
{

	//const		PCK_KEYS*	cPckXorKeys;
	PCK_KEYS	cPckXorKeys;
	//Head size
	size_t		dwHeadSize;
	//tail size
	size_t		dwTailSize;
	//size of file index
	size_t		dwFileIndexSize;
	//read head
	//read end
	//read file index
	BOOL(*PickIndexData)(void*, void*);
	//Fill in the header
	void*(*FillHeadData)(void*);
	//Fill in the end
	void*(*FillTailData)(void*);
	//Fill in file index
	void*(*FillIndexData)(void*, void*);
}PCK_VERSION_FUNC, *LPPCK_VERSION_FUNC;

//****** structures ******* 
#pragma pack(push, 4)

/*
** PCKHEAD The file header structure of the PCK file
** size = 12
**
** dwHeadCheckHead
** Depends on the game to which the file belongs. Different games have different values.
**
** dwPckSize
** The size of the entire pck file
**
** dwHeadCheckTail
** Available when file version = 2.0.2, merged into dwPckSize when version = 2.0.3
** Used as a high position in the middle
**
*/

typedef struct _PCK_HEAD_V2020
{
	uint32_t		dwHeadCheckHead;
	uint32_t		dwPckSize;
	uint32_t		dwHeadCheckTail;
}PCKHEAD_V2020, *LPPCKHEAD_V2020;

typedef struct _PCK_HEAD_V2030
{
	uint32_t		dwHeadCheckHead;
	union
	{
		uint64_t		dwPckSize;
		struct
		{
			uint32_t		dwPckSizeLow;
			uint32_t		dwHeadCheckTail;
		};
	};
}PCKHEAD_V2030, *LPPCKHEAD_V2030;

/*
** PCKTAIL The file tail (last 8 bytes) structure of the PCK file
** size = 8
**
** dwFileCount
** Number of compressed files in the file
**
** dwVersion
** pck file version
**
*/

typedef struct _PCK_TAIL_V2020
{
	uint32_t		dwIndexTableCheckHead;	//guardByte0
	uint32_t		dwVersion0;				//dwVersion
	uint32_t		dwEntryOffset;
	uint32_t		dwFlags;				//package flags. the highest bit means the encrypt state;, a5 != 0 ? 0x80000000 : 0 Zhu Xian a5 === 0
	char			szAdditionalInfo[PCK_ADDITIONAL_INFO_SIZE];
	uint32_t		dwIndexTableCheckTail;	//guardByte1
	uint32_t		dwFileCount;
	uint32_t		dwVersion;
}PCKTAIL_V2020, *LPPCKTAIL_V2020;

typedef struct _PCK_TAIL_V2030
{
	uint32_t		dwIndexTableCheckHead;
	uint32_t		dwVersion0;
	uint64_t		dwEntryOffset;
	uint32_t		dwFlags;
	char			szAdditionalInfo[PCK_ADDITIONAL_INFO_SIZE];
	uint64_t		dwIndexTableCheckTail;
	uint32_t		dwFileCount;
	uint32_t		dwVersion;
}PCKTAIL_V2030, *LPPCKTAIL_V2030;

typedef struct _PCK_TAIL_VXAJH
{
	uint32_t		dwIndexTableCheckHead;
	union
	{
		uint64_t		dwEntryOffset;
		struct
		{
			uint32_t		dwVersion0;
			uint32_t		dwCryptedFileIndexesAddrHigh;
		};
	};
	uint32_t		dwFlags;
	char			szAdditionalInfo[PCK_ADDITIONAL_INFO_SIZE];
	uint32_t		dwIndexTableCheckTail;
	uint32_t		dwFileCount;
	uint32_t		dwVersion;
}PCKTAIL_VXAJH, *LPPCKTAIL_VXAJH;

typedef struct _PCK_FILE_INDEX_V2020
{
	char			szFilename[MAX_PATH_PCK_256];
	uint32_t		dwUnknown1;
	uint32_t		dwAddressOffset;
	uint32_t		dwFileClearTextSize;
	uint32_t		dwFileCipherTextSize;
	uint32_t		dwUnknown2;
}PCKFILEINDEX_V2020, *LPPCKFILEINDEX_V2020;

typedef struct _PCK_FILE_INDEX_V2030
{
	char			szFilename[MAX_PATH_PCK_260];
	uint32_t		dwUnknown1;
	uint64_t		dwAddressOffset;
	uint32_t		dwFileClearTextSize;
	uint32_t		dwFileCipherTextSize;
	uint32_t		dwUnknown2;
}PCKFILEINDEX_V2030, *LPPCKFILEINDEX_V2030;


#if PCK_V2031_ENABLE
/*
The new Zhuxian index size has been changed to 288, and 4 bytes of new content have been added.
It has little impact at present and will not be added for the time being.
*/
typedef struct _PCK_FILE_INDEX_V2031
{
	char			szFilename[MAX_PATH_PCK_260];
	uint32_t		dwUnknown1;
	uint64_t		dwAddressOffset;
	uint32_t		dwFileClearTextSize;
	uint32_t		dwFileCipherTextSize;
	uint32_t		dwUnknown2;
	uint32_t		dwUnknown3;
}PCKFILEINDEX_V2031, *LPPCKFILEINDEX_V2031;

#endif

typedef struct _PCK_FILE_INDEX_VXAJH
{
	char			szFilename[MAX_PATH_PCK_256];
	uint32_t		dwUnknown1;
	uint64_t		dwAddressOffset;
	uint32_t		dwFileClearTextSize;
	uint32_t		dwFileCipherTextSize;
	uint32_t		dwUnknown2;
}PCKFILEINDEX_VXAJH, *LPPCKFILEINDEX_VXAJH;

typedef struct _PCK_FILE_INDEX
{
	wchar_t			szwFilename[MAX_PATH_PCK_260];		//Universal Unicode encoding
	char			szFilename[MAX_PATH_PCK_260];		//Use pck internal ansi encoding, default CP936
	uint64_t		dwAddressOffset;
	ulong_t			dwFileClearTextSize;
	ulong_t			dwFileCipherTextSize;
}PCKFILEINDEX, *LPPCKFILEINDEX;
#pragma pack(pop)


typedef struct _PCK_INDEX_TABLE
{
	int				entryType;
	PCKFILEINDEX	cFileIndex;
	BOOL			isInvalid;			//When adding a file, if the file name is repeated, the overwritten file is set to TRUE (when opening a file to create a file directory tree, the later files with the same name will overwrite the previous ones, and the overwritten value is set to TRUE)
	BOOL			isProtected;		//File protection is set and deletion is not allowed
	BOOL			isToDeDelete;		//When set to TRUE, the file will be deleted, except for isProtected to TRUE.
	uint32_t		dwMallocSize;						//The size of the requested space (>= compressed file size)
	LPBYTE			compressed_file_data;				//The compressed data corresponding to this index
	size_t			nFilelenBytes;			//File name (pck ansi) length in bytes
	size_t			nFilelenLeftBytes;		//The number of available bytes remaining in the file name (pck ansi), used when renaming, use MAX_PATH_PCK_256
}PCKINDEXTABLE, *LPPCKINDEXTABLE;


typedef struct _PCK_PATH_NODE
{
	int				entryType;
	wchar_t			szName[MAX_PATH_PCK_260];
	uint32_t		dwFilesCount;
	uint32_t		dwDirsCount;
	size_t			nNameSizeAnsi;		//The pck ansi length of the node name, record the length of this directory path (such as the .. directory under gfx\) in the .. directory (ansi)
	size_t			nMaxNameSizeAnsi;	//
	uint64_t		qdwDirClearTextSize;
	uint64_t		qdwDirCipherTextSize;
	LPPCKINDEXTABLE	lpPckIndexTable;
	_PCK_PATH_NODE	*parentfirst;		//Use the .. directory in a non-root directory to point to the node of the .. directory in the upper-level directory, that is, the .. directory where the clicked directory is located.
	_PCK_PATH_NODE	*parent;			//The .. directory in a non-root directory is used to point to the node of this directory in the upper-level directory, that is, the upper-level directory of the clicked directory.
	_PCK_PATH_NODE	*child;				//The ordinary directory points to the node of the .. directory of the lower-level directory
	_PCK_PATH_NODE	*next;				//This level directory points to the node of the next item
}PCK_PATH_NODE, *LPPCK_PATH_NODE;


typedef struct _FILES_TO_COMPRESS
{
#if PCK_DEBUG_OUTPUT
	int				id;
#endif
	uint32_t			dwCompressedflag;
	uint32_t			dwFileSize;
	uint32_t			nBytesToCopy;
	char			szFilename[MAX_PATH];
	wchar_t			szwFilename[MAX_PATH];
	size_t			nFileTitleLen;
	_FILES_TO_COMPRESS	*next;
	_PCK_INDEX_TABLE	*samePtr;
}FILES_TO_COMPRESS, *LPFILES_TO_COMPRESS;


typedef struct _PCK_INDEX_TABLE_COMPRESS
{
	union{
		BYTE			compressed_index_data[1];
		struct
		{
			uint32_t			dwIndexValueHead;
			uint32_t			dwIndexValueTail;
			BYTE			buffer[MAX_INDEXTABLE_CLEARTEXT_LENGTH];
		};
	};
	ulong_t				dwIndexDataLength;					//File index compressed size
	uint32_t			dwCompressedFilesize;				//Compressed file size
	uint32_t			dwMallocSize;						//The size of the requested space (>= compressed file size)
	uint64_t			dwAddressFileDataToWrite;			//The address where the compressed data is written to the file
	uint32_t			dwAddressOfDuplicateOldDataArea;	//If the old data area is used, its address
}PCKINDEXTABLE_COMPRESS, *LPPCKINDEXTABLE_COMPRESS;


typedef struct _PCK_ALL_INFOS
{
	BOOL				isPckFileLoaded;	//Has the PCK file been loaded successfully?
	uint64_t				qwPckSize;
	uint32_t				dwFileCount;
	uint64_t				dwAddressOfFileEntry;		//This value points to the end of the pck file data area, which is the starting position of the compressed data of the file index.
	char				szAdditionalInfo[PCK_ADDITIONAL_INFO_SIZE];
	wchar_t				szFilename[MAX_PATH];
	wchar_t				szFileTitle[MAX_PATH];

	//std::vector<PCKINDEXTABLE> lstFileEntry;	//Index of PCK files
	LPPCKINDEXTABLE		lpPckIndexTable;	//Index of PCK files
	PCK_PATH_NODE		cRootNode;			//The root node of the PCK file node

	wchar_t				szNewFilename[MAX_PATH];
	uint32_t				dwFileCountOld;
	uint32_t				dwFileCountToAdd;
	uint32_t				dwFinalFileCount;

	std::vector<FILES_TO_COMPRESS>				*lpFilesToBeAdded;
	const std::vector<PCKINDEXTABLE_COMPRESS>	*lpPckIndexTableToAdd;

	const PCK_VERSION_FUNC*	lpDetectedPckVerFunc;
	const PCK_VERSION_FUNC*	lpSaveAsPckVerFunc;

}PCK_ALL_INFOS, *LPPCK_ALL_INFOS;



class CPckControlCenter;



typedef struct _PCK_VARIETY_PARAMS {

	uint32_t		dwOldFileCount;
	uint32_t		dwPrepareToAddFileCount;
	uint32_t		dwChangedFileCount;
	uint32_t		dwDuplicateFileCount;
	uint32_t		dwFinalFileCount;

	//DWORD		dwUseNewDataAreaInDuplicateFileSize;
	uint32_t		dwDataAreaSize;
	uint32_t		dwRedundancyDataSize;

	LPCSTR		lpszAdditionalInfo;

	uint32_t		dwUIProgress;
	uint32_t		dwUIProgressUpper;

	uint32_t		dwMTMemoryUsed;

	BOOL		bThreadRunning;
	BOOL		bForcedStopWorking;	//Forced stop
	int			errMessageNo;			//0 - ok

}PCK_VARIETY_PARAMS;


typedef struct _PCK_RUNTIME_PARAMS {

	PCK_VARIETY_PARAMS	cVarParams;

	uint32_t		dwMTMaxMemory;		//Maximum memory usage
	uint32_t		dwMTThread;			//Number of compression threads
	uint32_t		dwCompressLevel;	//Data compression rate

	//int			code_page;			//pck file usage encoding

	CPckControlCenter	*lpPckControlCenter;

}PCK_RUNTIME_PARAMS, *LPPCK_RUNTIME_PARAMS;



#endif