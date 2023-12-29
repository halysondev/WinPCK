//////////////////////////////////////////////////////////////////////
// PckDefines.h: used to parse the data in the pck file of Perfect World Company and display it in the List
// head File
//
// This program is written by Li Qiufeng/stsm/liqf
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2015.5.13
//////////////////////////////////////////////////////////////////////

#include "pck_dependencies.h"
#include <limits.h>

#define __UCSTEXT(quote) L##quote
#define UCSTEXT(quote) __UCSTEXT(quote)


#if !defined(_PCKDEFINES_H_)
#define _PCKDEFINES_H_

#pragma warning ( disable : 4996 )
#pragma warning ( disable : 4005 )
#pragma warning ( disable : 4995 )

typedef enum _FMTPCK
{
	FMTPCK_PCK = 0,
	FMTPCK_ZUP = 1,
	FMTPCK_CUP = 2,
	FMTPCK_UNKNOWN = 0x7fffffff
}FMTPCK;

typedef enum _PCKVER
{
	PCK_V2020,
	PCK_V2030,
	PCK_VXAJH
}PCK_CATEGORY;

//LOG INFO STRING
#define TEXT_LOG_OPENFILE				"open a file %s"
#define TEXT_LOG_CLOSEFILE				"close file"
#define	TEXT_LOG_FLUSH_CACHE			"write to cache..."
#define	TEXT_LOG_RESTOR_OK				"Data recovery successful"

#define TEXT_LOG_WORKING_DONE			"Operation completed"

#define	TEXT_LOG_LEVEL_THREAD			"Compression ratio=%d:Thread=%d"

#define	TEXT_LOG_UPDATE_ADD				"update mode"
#define	TEXT_LOG_UPDATE_NEW				"New mode"

#define	TEXT_LOG_RENAME					"Rename (delete) files in package..."
#define	TEXT_LOG_REBUILD				"Rebuild PCK file..."
#define	TEXT_LOG_RECOMPRESS				"Recompress PCK files..."


#define	TEXT_LOG_CREATE					"New PCK file:%s..."

#define	TEXT_LOG_COMPRESSOK				"Compression completed, index written..."

#define	TEXT_LOG_EXTRACT				"unzip files..."



//ERROR STRING
#define	TEXT_ERROR						"error"

#define TEXT_INVALID_VERSION			"Invalid version ID!"

#define	TEXT_MALLOC_FAIL				"Failed to apply for memory!"
#define	TEXT_CREATEMAP_FAIL				"Failed to create file mapping!"
#define	TEXT_CREATEMAPNAME_FAIL			"Mapping file \"%s\" failed!"
#define	TEXT_OPENNAME_FAIL				"Failed to open file \"%s\"!"
#define	TEXT_VIEWMAP_FAIL				"Failed to create mapping view!"
#define	TEXT_VIEWMAPNAME_FAIL			"Failed to create mapping view for file \"%s\"!"

#define TEXT_READ_INDEX_FAIL			"File index table read error!"
#define TEXT_UNKNOWN_PCK_FILE			"Unrecognized PCK file!"

#define	TEXT_OPENWRITENAME_FAIL			"Failed to open file \"%s\" for writing!"
#define	TEXT_READFILE_FAIL				"File reading failed!"
#define	TEXT_WRITEFILE_FAIL				"File writing failed!"
#define TEXT_WRITE_PCK_INDEX			"Error writing file index!"

#define	TEXT_USERCANCLE					"User cancels exit!"

#define	TEXT_COMPFILE_TOOBIG			"The compressed file is too large!"
#define	TEXT_UNCOMP_FAIL				"Failed to decompress file!"

#define	TEXT_UNCOMPRESSDATA_FAIL		"File %s \r\nData decompression failed!"
#define	TEXT_UNCOMPRESSDATA_FAIL_REASON	"Data decompression failed: %s"

#define	TEXT_ERROR_OPEN_AFTER_UPDATE	"The opening failed. It may be that the last operation caused the file to be damaged. \r\nTry to restore to the last opened state?"
#define	TEXT_ERROR_GET_RESTORE_DATA		"An error occurred while getting recovery information"
#define	TEXT_ERROR_RESTORING			"Error while restoring"
#define	TEXT_ERROR_DUP_FOLDER_FILE		"Exists (file name = folder name), exit."

#define TEXT_UPDATE_FILE_INFO			"Number of pre-added files=%d: Expected file size=%lld, start the job..."

#define TEXT_PCK_SIZE_INVALID			"Invalid PCK file size"
#define TEXT_VERSION_NOT_FOUND			"No valid version found"

#endif