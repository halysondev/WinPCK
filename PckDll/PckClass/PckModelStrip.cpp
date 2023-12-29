#include "PckModelStrip.h"
#include "pck_default_vars.h"

#define BUFFER_SIZE		0x10000

#define STRIP_TYPE_NONE	0
#define STRIP_TYPE_ATT	1
#define STRIP_TYPE_GFX	2
#define STRIP_TYPE_ECM	3

CPckModelStrip::CPckModelStrip()
{}

CPckModelStrip::~CPckModelStrip()
{}

BOOL CPckModelStrip::StripContent(BYTE* buffer, LPPCKFILEINDEX lpFileIndex, int flag)
{
	switch (GetFileTypeByName(lpFileIndex->szwFilename))
	{
	case STRIP_TYPE_ATT:
		if(PCK_STRIP_ATT & flag)
			StripAtt(buffer, lpFileIndex);
		break;

	case STRIP_TYPE_GFX:
		if (PCK_STRIP_GFX & flag)
			StripGfx(buffer, lpFileIndex);
		break;
#if _STRIP_ECM_
	case STRIP_TYPE_ECM:
		if (PCK_STRIP_ECM & flag)
			StripEcm(buffer, lpFileIndex);
		break;
#endif
	default:
		return TRUE;
	}
	return TRUE;
}

int CPckModelStrip::GetFileTypeByName(const wchar_t* lpszFilename)
{
	const wchar_t* lpszFileTitle = wcsrchr(lpszFilename, L'\\');

	if (NULL == lpszFileTitle)
		lpszFileTitle = lpszFilename;

	const wchar_t* lpszExt = wcsrchr(lpszFileTitle, L'.');

	if (NULL == lpszExt)
		return STRIP_TYPE_NONE;

	if (0 == wcsicmp(L".att", lpszExt))
		return STRIP_TYPE_ATT;

	if (0 == wcsicmp(L".gfx", lpszExt))
		return STRIP_TYPE_GFX;
#if _STRIP_ECM_
	if (0 == wcsicmp(L".ecm", lpszExt))
		return STRIP_TYPE_ECM;
#endif
	return STRIP_TYPE_NONE;
}

BOOL CPckModelStrip::StripAtt(BYTE* buffer, LPPCKFILEINDEX lpFileIndex)
{

	LPCSTR	szDstStrAtt = "Path: ";
	BYTE	buffer_dst[BUFFER_SIZE];

	if (BUFFER_SIZE <= lpFileIndex->dwFileClearTextSize) 
		return FALSE;
	
	memcpy(buffer_dst, buffer, lpFileIndex->dwFileClearTextSize);
	buffer_dst[lpFileIndex->dwFileClearTextSize] = 0;

	int nLen_dst = 0;
	int nLen_src_last = lpFileIndex->dwFileClearTextSize;	//How many bytes are left that have not been copied?

	const char* lpszAttBuff;
	const char* lpszAttBuff_Prev = lpszAttBuff = (char*)buffer_dst;
	char* lpszAttDst = (char*)buffer;

	//Search for "Path: ", and then remove the content after the line "Path: "
	//Path: 人物\击中\冷嗤_击中子效果轨迹球.gfx\r\n
	//Hook:\r\n
	//Pos: 0.000000, 0.000000, 0.000000\r\n
	while (NULL != (lpszAttBuff = strstr(lpszAttBuff_Prev, szDstStrAtt)))
	{
		//lpszAttBuff:->
		//Path: 人物\击中\冷嗤_击中子效果轨迹球.gfx\r\n
		//Hook:\r\n
		//Pos: 0.000000, 0.000000, 0.000000\r\n
		lpszAttBuff += strlen(szDstStrAtt);

		//lpszAttBuff:->
		//人物\击中\冷嗤_击中子效果轨迹球.gfx\r\n
		//Hook:\r\n
		//Pos: 0.000000, 0.000000, 0.000000
		//Copy the content from the previous position to "Path:"
		//for (const char *lpbuf = lpszAttBuff_Prev; lpbuf < lpszAttBuff; lpbuf++)
		//{
		//	*lpszAttDst = *lpbuf;
		//	lpszAttDst++;
		//	nLen_dst++;
		//	nLen_src_last--;
		//}
		size_t bytesToCopy = lpszAttBuff - lpszAttBuff_Prev;

		memcpy(lpszAttDst, lpszAttBuff_Prev, bytesToCopy);
		nLen_dst += bytesToCopy;
		nLen_src_last -= bytesToCopy;
		lpszAttDst += bytesToCopy;


		//Skip the content before \r\n from the beginning of lpszAttBuff to the end of the line
		//lpszAttBuff:->
		//\r\n
		//Hook:\r\n
		//Pos: 0.000000, 0.000000, 0.000000\r\n
		//Copy the content from the previous position to "Path:"
		while ('\r' != *lpszAttBuff && '\n' != *lpszAttBuff)
		{
			lpszAttBuff++;
			nLen_src_last--;
		}

		lpszAttBuff_Prev = lpszAttBuff;

	}

	lpszAttBuff = lpszAttBuff_Prev;

	//Copy the last content
	memcpy(buffer + nLen_dst, lpszAttBuff, nLen_src_last);

	lpFileIndex->dwFileClearTextSize = nLen_dst + nLen_src_last;

	return TRUE;
}

BOOL CPckModelStrip::StripGfx(BYTE* buffer, LPPCKFILEINDEX lpFileIndex)
{
	LPCSTR	szDstStrGfx = "GFXELEMENTCOUNT: ";
	buffer[lpFileIndex->dwFileClearTextSize] = 0;

	int nLen_dst = 0;
	//int nLen_src_last = lpFileIndex->dwFileClearTextSize;	//How many bytes are left that have not been copied?

	char* lpszAttBuff;
	char* lpszAttBuff_Prev = lpszAttBuff = (char*)buffer;

	if (NULL != (lpszAttBuff = strstr(lpszAttBuff_Prev, szDstStrGfx)))
	{
		//

		lpszAttBuff += strlen(szDstStrGfx);

		nLen_dst = lpszAttBuff - (char*)buffer + 3;
		strcpy(lpszAttBuff, "0\r\n");

		lpFileIndex->dwFileClearTextSize = nLen_dst;

	}

	return TRUE;
}
#if _STRIP_ECM_
BOOL CPckModelStrip::StripEcm(BYTE* buffer, LPPCKFILEINDEX lpFileIndex)
{
	LPCSTR szEmptyEcm = 
		"MOXTVersion: 15\r\n"
		"SkinModelPath: \r\n"
		"CoGfxNum : 0\r\n"
		"ComActCount : 0\r\n"
		"AddiSkinCount : 0\r\n"
		"ChildCount : 0\r\n";

	memcpy(buffer, szEmptyEcm, strlen(szEmptyEcm));
	lpFileIndex->dwFileClearTextSize = strlen(szEmptyEcm);

	return TRUE;
}
#endif