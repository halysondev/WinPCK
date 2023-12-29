//////////////////////////////////////////////////////////////////////
// SavePicture.cpp: Construct DIB data
// Save the decoded dds and tga data as files
//
// This program is written by Li Qiufeng/stsm/liqf
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2018.5.29
//////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include "ShowPicture.h"

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT num = 0;
	UINT size = 0;

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if(size == 0) {
		return -1;
	}
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL) {
		return -1;
	}

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j) {
		if(wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}

	free(pImageCodecInfo);
	return -1;
}

BOOL CShowPicture::Save(const WCHAR *lpszFilename)
{
	CLSID pngClsid;
	GetEncoderClsid(L"image/png", &pngClsid);
	m_lpmyImage->Save(lpszFilename, &pngClsid, NULL);
	return TRUE;
}