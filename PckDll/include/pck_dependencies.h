#pragma once
#include <stdint.h>
#include "pck_default_vars.h"
#include "PckModelStripDefines.h"

//callback function to display list
typedef void(*SHOW_LIST_CALLBACK)(void*, int32_t, const wchar_t *, int32_t, uint64_t, uint64_t, void*);
typedef int32_t(*FeedbackCallback)(void* pTag, int32_t eventId, size_t wParam, ssize_t lParam);

//Log echo callback function
typedef void(*ShowLogA)(const char log_level, const char *str);
typedef void(*ShowLogW)(const char log_level, const wchar_t *str);

typedef struct _PCK_UNIFIED_FILEENTRY {
	int32_t				entryType;
	wchar_t			szName[MAX_PATH_PCK_260];
}PCK_UNIFIED_FILE_ENTRY, *LPPCK_UNIFIED_FILE_ENTRY;

typedef PCK_UNIFIED_FILE_ENTRY*			LPENTRY;
typedef const PCK_UNIFIED_FILE_ENTRY*	LPCENTRY;
