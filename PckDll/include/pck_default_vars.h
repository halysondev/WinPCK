#pragma once

#include <Windows.h>
#include <stdint.h>
#include "gccException.h"

typedef unsigned long       ulong_t;
typedef int                 BOOL;
typedef unsigned char       BYTE;

typedef const wchar_t * 	LPCWSTR;
typedef wchar_t *			LPWSTR;
typedef const char * 		LPCSTR;
typedef char *				LPSTR;

/* Define ssize_t */
#ifdef _WIN64
typedef long long ssize_t;
#else
typedef int ssize_t;
#endif

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif


//Version Information
#define WINPCK_VERSION_NUMBER  1,33,1,0
#define WINPCK_VERSION        "1.33.1.0"

//Scheduling output
#ifdef _DEBUG
#define PCK_DEBUG_OUTPUT		0
#define PCK_DEBUG_OUTPUT_FILE	0
#else
#define PCK_DEBUG_OUTPUT		0
#define PCK_DEBUG_OUTPUT_FILE	0
#endif

#define PCK_V2031_ENABLE		0

//Open, close, restore and other event registration
#define PCK_FILE_OPEN_SUCESS		1
#define PCK_FILE_CLOSE				2


#define MAX_PATH			260

//zlib
#define Z_OK				0

//params
#define	MT_MAX_MEMORY				0x80000000	//2048MB
//#define MT_MAX_MEMORY				0x100000000
#define MAX_COMPRESS_LEVEL			12
#define Z_DEFAULT_COMPRESS_LEVEL	9

#define PCK_OK					0   /* Successful result */
/* beginning-of-error-codes */
#define PCK_MSG_USERCANCELED	1   /* User cancels */
#define PCK_ERROR				2   /* Generic error */
#define PCK_ERR_DISKFULL		3	/* Disk is full */ //Insufficient disk space, requested space: %d, remaining space: %d
#define PCK_ERR_VIEWMAP_FAIL	4	/* Mapping failed */
#define PCK_ERR_VIEW			5	/* View failed */
#define PCK_ERR_OPENMAPVIEWR	6	/* OpenMappingViewAllRead failed */
#define PCK_ERR_MALLOC			7	/* Memory request failed */

/* end-of-error-codes */

//The length of the string used when converting numbers to characters
#define CHAR_NUM_LEN 12

#define	MAX_INDEXTABLE_CLEARTEXT_LENGTH	0x120
#define MAX_TAIL_LENGTH					300

#define	PCK_BEGINCOMPRESS_SIZE			20
#define	MAX_PATH_PCK_256				256
#define	MAX_PATH_PCK_260				260
#define PCK_ADDITIONAL_INFO_SIZE		252
#define PCK_DATA_START_AT				12

#define PCK_ENTRY_TYPE_INDEX			1
#define PCK_ENTRY_TYPE_NODE				2
#define PCK_ENTRY_TYPE_FOLDER			4
#define PCK_ENTRY_TYPE_DOTDOT			8
#define PCK_ENTRY_TYPE_ROOT				16
#define PCK_ENTRY_TYPE_TAIL_INDEX		0x80000000
//#define PCK_ENTRY_TYPE_TAIL_INDEX		0x100000000

#define	PCK_ADDITIONAL_KEY				"Angelica File Package"
#define	PCK_ADDITIONAL_INFO				PCK_ADDITIONAL_KEY", Perfect World Co. Ltd. 2002~2008. All Rights Reserved.\r\nCreated by WinPCK v" WINPCK_VERSION  
