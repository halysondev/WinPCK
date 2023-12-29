//////////////////////////////////////////////////////////////////////
// ZupHeader.h: used to parse the data in the zup file of Perfect World Company and display it in the List
// head File
//
// This program is written by Li Qiufeng/stsm/liqf
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2012.5.23
//////////////////////////////////////////////////////////////////////

#if !defined(_ZUPHEADER_H_)
#define _ZUPHEADER_H_

//#define	TEXT_SORRY			"sorry"
#define	TEXT_NOTSUPPORT		"This feature is not supported at the moment"

typedef struct _ZUP_INC {
	uint16_t	wID;
	char		szMD5[34];
	uint8_t		abMD5[16];
	
	_ZUP_INC	*next;
}ZUPINC, *LPZUPINC;

typedef struct _ZUP_INC_LIST {

	uint16_t		wID;
	uint16_t		wMinVersion;
	uint16_t		wTargetVersion;
	uint32_t		dwTotalSize;
	char			szSingature[192];
	uint8_t			abSingature[128];
	LPZUPINC		lpZupIncFileList;

	_ZUP_INC_LIST	*next;

}ZUPINCLIST, *LPZUPINCLIST;



#endif