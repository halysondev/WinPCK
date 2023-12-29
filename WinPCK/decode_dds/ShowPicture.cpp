//////////////////////////////////////////////////////////////////////
// ShowPicture.cpp: display dds, tga images
// Decode dds, tga and display
//
// This program is written by Li Qiufeng/stsm/liqf
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2018.5.29
//////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include "ShowPicture.h"
#include <stdio.h>
#include <tchar.h>

#include <math.h>

CShowPicture::CShowPicture(HWND hWndShow, LPBYTE &_buffer, size_t _bufsize, LPCWSTR _lpszFileTitle, PICFORMAT _picFormat) :
	m_hWndShow(hWndShow),
	m_lpszFileTitle(_lpszFileTitle),
	m_picFormat(_picFormat),
	m_bufsize(_bufsize),
	m_stream(NULL),
	m_hGlobal(NULL),
	m_lpmyImage(NULL),
	m_lpoGraph(NULL),
	m_DecodedDIBData(NULL)
{


	if(FMT_RAW != (m_picFormat))	//dds,tga
	{
		m_resBufferInClass = (LPBYTE)malloc(m_bufsize);
	} else {							//bmp,jpg,png..
		m_resBufferInClass = (LPBYTE)(m_hGlobal = GlobalAlloc(GMEM_FIXED, m_bufsize));
	}

	if(NULL != m_resBufferInClass) {
		//Copy data
		memcpy(m_resBufferInClass, _buffer, _bufsize);
	}

	//Release the incoming memory after copying is complete
	free(_buffer);
	_buffer = NULL;
	
	GdiplusStartup(&m_pGdiPlusToken, &m_oGdiPlusStartupInput, NULL);
}


CShowPicture::~CShowPicture()
{
	if(NULL != m_lpoGraph)
		delete m_lpoGraph;

	if(NULL != m_lpmyImage)
		delete m_lpmyImage;

	if(NULL != m_stream)
		m_stream->Release();

	if(FMT_RAW != m_picFormat) {
		if(NULL != m_resBufferInClass) {
			free(m_resBufferInClass);
		}
	} else {

		if(NULL != m_hGlobal)
			GlobalFree(m_hGlobal);
	}

	if(NULL != m_DecodedDIBData) {
		free(m_DecodedDIBData);
	}

	DeleteObject(m_MemBitmap);
	DeleteDC(m_MemDC);

	GdiplusShutdown(m_pGdiPlusToken);
}

BOOL CShowPicture::isEqual(double d1, double d2)
{
	return (EPSILON > fabs((d1-d2)));
}

LPCWSTR	CShowPicture::GetWindowTitle(LPWSTR	lpszTitle, size_t bufSize)
{
	static wchar_t szTitle[MAX_PATH];

	if(NULL == lpszTitle)
		lpszTitle = szTitle;

	//window text
	if(FMT_RAW != m_picFormat) {
		swprintf_s(lpszTitle, MAX_PATH, L"Picture view - %s, %dx%d, %S", m_lpszFileTitle, m_picWidth, m_picHeight, m_szPictureFormat);
	} else {
		swprintf_s(lpszTitle, MAX_PATH, L"Picture view - %s, %dx%d", m_lpszFileTitle, m_picWidth, m_picHeight);
	}

	//SetWindowText(m_hWndShow, lpszTitle);
	return lpszTitle;
}

BOOL CShowPicture::ShowTitleOnWindow()
{
	wchar_t szTitle[MAX_PATH];

	//window text
	if(FMT_RAW != m_picFormat) {

		swprintf_s(szTitle, MAX_PATH, L"Picture view - %s, %dx%d, %S", m_lpszFileTitle, m_picWidth, m_picHeight, m_szPictureFormat);

	} else {
		swprintf_s(szTitle, MAX_PATH, L"Picture view - %s, %dx%d", m_lpszFileTitle, m_picWidth, m_picHeight);
	}

	SetWindowTextW(m_hWndShow, szTitle);
	return TRUE;
}

//Returns the width and height of the image
UINT CShowPicture::GetWidth()
{
	return m_picWidth;
}

UINT CShowPicture::GetHeight()
{
	return m_picHeight;
}


void CalcZoomInAreaAndDrawPosition(__int64 &dst_xy, __int64 &dst_wh, __int64 &src_xy, __int64 &src_wh, double dZoomRatio, LONG iClientWidthHeight, UINT uiPicWidthHeight)
{
	/*
	When the zoomed image is larger than the window, set the coordinates of the upper left corner of the window to 0,0
	The area not displayed on the left end is set to left_invisible (absolute value), the right end is set to right_invisible, and the zoom rate is Ratio.
	Then the actual image size area not displayed on the left is left_invisible_real = (int)left_invisible/Ratio, and the remainder is left_invisible_rem
	The right side is the same as right_invisible = tmp_dst_w - left_invisible - rectDlg.right, right_invisible_real = (int)right_invisible/Ratio, right_invisible_rem
	At the same time, the remainder after division (left_invisible_rem) is the offset of the data to be displayed on the window after scaling.
	Assume the actual image width is m_picWidth. The actual data to be retrieved is: visible_width_real = m_picWidth - left_invisible_real - right_invisible_real,
	The upper left corner of the target displays coordinates dst_x = left_invisible_rem
	Source upper left corner X coordinate src_x = left_invisible_real
	*/

	__int64		left_invisible = -dst_xy;
	double	left_invisible_real_double = left_invisible / dZoomRatio;
	__int64		left_invisible_real = (__int64)left_invisible_real_double;
	__int64		left_invisible_rem = (__int64)(left_invisible - left_invisible_real * dZoomRatio);
	__int64		right_invisible = dst_wh - left_invisible - iClientWidthHeight;
	__int64		right_invisible_real_double = (__int64)(right_invisible / dZoomRatio);
	__int64		right_invisible_real = (__int64)right_invisible_real_double;
	__int64		right_invisible_rem = (__int64)(right_invisible - right_invisible_real * dZoomRatio);
	__int64		visible_width_real = uiPicWidthHeight - left_invisible_real - right_invisible_real;

	dst_xy = -left_invisible_rem;
	src_xy = left_invisible_real;
	src_wh = visible_width_real;
	dst_wh = iClientWidthHeight + right_invisible_rem + left_invisible_rem;

}

//Display image on device
BOOL CShowPicture::Paint(__int64 nXOriginDest, __int64 nYOriginDest, int nXOriginSrc, int nYOriginSrc, double dZoomRatio)
{
	/*
	Set the coordinates of the upper left corner of the window to 0,0
	pic_x: The position of the upper left corner of the image to be displayed relative to the window
	*/
	RECT rectDlg;

	HDC pDC = ::GetDC(m_hWndShow);
	GetClientRect(m_hWndShow, &rectDlg);

	//Recalculate the source starting point, width and height and the target starting point, width and height
	__int64 src_x = nXOriginSrc;
	__int64 src_y = nYOriginSrc;
	__int64 src_w = m_picWidth;
	__int64 src_h = m_picHeight;
	__int64 dst_x = nXOriginDest;
	__int64 dst_y = nYOriginDest;
	__int64 dst_w = m_picWidth;
	__int64 dst_h = m_picHeight;

	PAINTSTRUCT ps;
	HDC pDC1 = BeginPaint(m_hWndShow, &ps);

	if(((1.0 - EPSILON) < dZoomRatio) && ((1.0 + EPSILON) > dZoomRatio)) {
		//if(isEqual(1.0, dZoomRatio)) {
			//No scaling
		if(nXOriginDest < 0) {

			dst_x = 0;
			src_x = nXOriginSrc - nXOriginDest;
			dst_w = (((__int64)m_picWidth - src_x) > (__int64)rectDlg.right) ? (__int64)rectDlg.right : ((__int64)m_picWidth - src_x);
		}
		if(nYOriginSrc < 0) {

			dst_y = 0;
			src_y = nYOriginSrc - nYOriginDest;
			dst_h = (((__int64)m_picHeight - src_y) >(__int64)rectDlg.bottom) ? (__int64)rectDlg.bottom : ((__int64)m_picHeight - src_y);
		}

		BitBlt(	
			pDC,				// Target DC handle
			(int)dst_x,				// X coordinate of the upper left corner of the target
			(int)dst_y,				// Y coordinate of the upper left corner of the target
			(int)dst_w,				// target width
			(int)dst_h,				// target height
			m_MemDC, 			// Source DC handle
			(int)src_x,				// X coordinate of the upper left corner of the source
			(int)src_y,				// Y coordinate of the upper left corner of the source
			SRCCOPY);

	} else {
		//Zoom
		dst_w = (__int64)(m_picWidth * dZoomRatio + 0.5);
		dst_h = (__int64)(m_picHeight * dZoomRatio + 0.5);

		if(rectDlg.right < dst_w)
			CalcZoomInAreaAndDrawPosition(dst_x, dst_w, src_x, src_w, dZoomRatio, rectDlg.right, m_picWidth);

		if(rectDlg.bottom < dst_h)
			CalcZoomInAreaAndDrawPosition(dst_y, dst_h, src_y, src_h, dZoomRatio, rectDlg.bottom, m_picHeight);

		SetStretchBltMode(pDC, COLORONCOLOR);
		StretchBlt(
			pDC,				// Target DC handle
			(int)dst_x,				// X coordinate of the upper left corner of the target
			(int)dst_y,				// Y coordinate of the upper left corner of the target
			(int)dst_w,				// target width
			(int)dst_h,				// target height
			m_MemDC,			// Source DC handle
			(int)src_x,				// X coordinate of the upper left corner of the source
			(int)src_y,				// Y coordinate of the upper left corner of the source
			(int)src_w,				// source width
			(int)src_h,				// source height
			SRCCOPY);
	}

	ReleaseDC(m_hWndShow, pDC);
	EndPaint(m_hWndShow, &ps);

	return TRUE;
}

//First display the grid with a transparent background on the window
BOOL CShowPicture::DrawBlockOnDlg()
{
	//HDC pDC = ::GetDC(GetDlgItem(IDC_STATIC_PIC));
	HDC pDC = ::GetDC(m_hWndShow);
	//GetClientRect(hWnd, &rect);

	//CDC MemDC; //First define a display device object
	HDC MemDCTemp;
	//CBitmap MemBitmap;//Define a bitmap object
	HBITMAP MemBitmapTemp;
	//Then create a memory display device compatible with the screen display  
	m_MemDC = CreateCompatibleDC(NULL);
	MemDCTemp = CreateCompatibleDC(NULL);
	//You can't draw at this time because there is no place to draw
	//The following creates a bitmap that is compatible with the screen display. The size of the bitmap can be determined by the size of the window.
	m_MemBitmap = CreateCompatibleBitmap(pDC, m_picWidth, m_picHeight);
	MemBitmapTemp = CreateCompatibleBitmap(pDC, 24, 8);

	ReleaseDC(m_hWndShow, pDC);

	//Select a bitmap into a memory display device 
	//Only the memory display device with a bitmap selected can have a place to draw and draw to the specified bitmap.
	HBITMAP pOldBit = (HBITMAP)SelectObject(m_MemDC, m_MemBitmap);
	HBITMAP pOldBit1 = (HBITMAP)SelectObject(MemDCTemp, MemBitmapTemp);

	//First use the background color to clear the bitmap. Here I use white as the background.
	//You can also use the colors you should use
	SetBkColor(MemDCTemp, RGB(204, 204, 204));
	RECT thisrect = { 0, 0, 24, 8 };
	ExtTextOut(MemDCTemp, 0, 0, ETO_OPAQUE, &thisrect, NULL, 0, NULL);

	thisrect.left += 8;
	SetBkColor(MemDCTemp, RGB(255, 255, 255));
	ExtTextOut(MemDCTemp, 0, 0, ETO_OPAQUE, &thisrect, NULL, 0, NULL);

	thisrect.left += 8;
	SetBkColor(MemDCTemp, RGB(204, 204, 204));
	ExtTextOut(MemDCTemp, 0, 0, ETO_OPAQUE, &thisrect, NULL, 0, NULL);

	for(unsigned int j = 0;j < m_picHeight;j += 8) {
		for(unsigned int i = 0;i < m_picWidth;i += 16) {
			if(j & 8)
				BitBlt(m_MemDC, i, j, i + 16, j + 8, MemDCTemp, 0, 0, SRCCOPY);
			else
				BitBlt(m_MemDC, i, j, i + 16, j + 8, MemDCTemp, 8, 0, SRCCOPY);
		}
	}
	//MemBitmap.DeleteObject(); 
	SelectObject(MemDCTemp, pOldBit1);
	//a = DeleteObject(MemBitmap);
	DeleteObject(MemBitmapTemp);
	//MemDC.DeleteDC();
	DeleteDC(MemDCTemp);

	return TRUE;
}

BOOL CShowPicture::Decode()
{
	if(NULL == m_resBufferInClass)return FALSE;
	if (!BuildImage())return FALSE;
	if (!DrawPictureOnDC())return FALSE;
	if (!ShowTitleOnWindow())return FALSE;
	return TRUE;
}