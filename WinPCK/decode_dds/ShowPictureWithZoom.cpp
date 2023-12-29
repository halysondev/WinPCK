//////////////////////////////////////////////////////////////////////
// ShowPictureWithZoom.cpp: Display zoomed dds, tga images
// Scale the decoded dds and tga images and display them
//
// This program is written by Li Qiufeng/stsm/liqf
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2018.5.29
//////////////////////////////////////////////////////////////////////

#include "ShowPictureWithZoom.h"

#define CHANGE_RATIO	1.2
#define MAX_ZOOM_RATIO	32.0

CShowPictureWithZoom::CShowPictureWithZoom(HWND hWndShow, LPBYTE &_buffer, size_t _bufsize, LPCWSTR _lpszFileTitle, PICFORMAT _picFormat) : 
	CShowPicture( hWndShow, _buffer,  _bufsize, _lpszFileTitle, _picFormat),
	m_fixedZoomRatio(1.0),
	m_dZoomRatioShow(1.0),
	m_dZoomRatio(1.0),
	m_ShowX(0),
	m_ShowY(0)
{
}


CShowPictureWithZoom::~CShowPictureWithZoom()
{}

double CShowPictureWithZoom::GetZoomRatio()
{
	return m_dZoomRatioShow;
}

void CalcZoomPictureAtOneDimensional(int iClientPointXY, __int64 &_showXY, UINT64 &_picWidthHeight, double _dZoomRatio, UINT _RealPicWidthHeight, LONG	lClientWidthHeight)
{
	/*
	Calculate m_ShowX,m_ShowY
	When the current display size is larger than the window:
	Zoom zooms centered on the current mouse position
	Assume that the position of the mouse point on the picture is rx, ry, the position of the mouse on the window is mx, my, and the display point of picture 0,0 on the window is sx, sy, then rx=mx-sx
	The zoom is centered on the current mouse position. If the mouse point on the picture does not move, that is, the vector value does not move. Let the vector value in the x direction be p, and let the current picture width be sw, then p=rx/sw=( mx-sx)/sw
	After scaling, the width of the picture changes to sw1, the display point changes to sx1, p remains unchanged, p=(mx-sx1)/sw1, and we get sx1 = mx - (mx-sx)*sw1/sw
	*/

	int mouseXY = iClientPointXY;
	//When the mouse pointer is outside the image, the image boundary shall prevail.
	if(_showXY > mouseXY)mouseXY = _showXY;
	__int64 iShowRightBottom = _showXY + _picWidthHeight;
	if(iShowRightBottom < mouseXY)mouseXY = iShowRightBottom;

	//Calculate the scaled m_ShowX, m_ShowY
	UINT64	dwShowPicWidthHeight = _dZoomRatio * _RealPicWidthHeight;

	//When the displayed image width is larger than the current window

	if(lClientWidthHeight < dwShowPicWidthHeight) {
		_showXY = mouseXY - (mouseXY - _showXY) * dwShowPicWidthHeight / _picWidthHeight;
		if(0 < _showXY)_showXY = 0;
	} else {

		_showXY = (lClientWidthHeight - dwShowPicWidthHeight) / 2;
	}

	_picWidthHeight = dwShowPicWidthHeight;
}

void CShowPictureWithZoom::ZoomPictureAtPoint(double dZoomRatio, __int64 pos_x, __int64 pos_y)
{

	//Calculate the results of scaling based on the current point in the X and Y directions respectively.

	RECT	rectWin;
	GetClientRect(m_hWndShow, &rectWin);

	CalcZoomPictureAtOneDimensional(pos_x, m_ShowX, m_ShowPicWidth, dZoomRatio, m_picWidth, rectWin.right);
	CalcZoomPictureAtOneDimensional(pos_y, m_ShowY, m_ShowPicHeight, dZoomRatio, m_picHeight, rectWin.bottom);

	Paint();
}

void CShowPictureWithZoom::ZoomWithMousePoint(UINT nFlags, short zDelta, __int64 pos_x, __int64 pos_y)
{

	//Maximum magnification is 8x, minimum is to fit current window size or 1.0
	double dZoomChangeRatio = 1.0;
	double dNextZoomRatio;
	//enlarge
	if(0 < zDelta) {

		dZoomChangeRatio *= (zDelta / 120.0 * CHANGE_RATIO);

		if((MAX_ZOOM_RATIO + EPSILON) < m_dZoomRatio)
			return;

		 dNextZoomRatio = m_dZoomRatio * dZoomChangeRatio;

		if(dNextZoomRatio > MAX_ZOOM_RATIO) {

			m_dZoomRatioShow = MAX_ZOOM_RATIO;
		} else {

			m_dZoomRatioShow = dNextZoomRatio;
		}

		//zoom out
	} else if(0 > zDelta) {
		dZoomChangeRatio /= (zDelta / (-120.0) * CHANGE_RATIO);

		//Limit zoom range
		double dMinZoomRatio = (m_fixedZoomRatio > (1.0 - EPSILON)) ? 1.0 : m_fixedZoomRatio;

		if(dMinZoomRatio > (m_dZoomRatio + EPSILON))
			return;

		 dNextZoomRatio = m_dZoomRatio * dZoomChangeRatio;

		if(dNextZoomRatio < dMinZoomRatio) {

			m_dZoomRatioShow = dMinZoomRatio;
		} else {

			m_dZoomRatioShow = dNextZoomRatio;
		}

	}

	m_dZoomRatio = dNextZoomRatio;

	ZoomPictureAtPoint(m_dZoomRatioShow, pos_x, pos_y);
}

void CShowPictureWithZoom::ZoomToOriginalResolution(__int64 pos_x, __int64 pos_y)
{
	m_dZoomRatioShow = m_dZoomRatio = 1.0;
	ZoomPictureAtPoint(1.0, pos_x, pos_y);
}


void CShowPictureWithZoom::MovePicture(int xOffset, int yOffset)
{

	RECT	rectWin;
	GetClientRect(m_hWndShow, &rectWin);

	//When the displayed image width is larger than the current window
	if(rectWin.right < m_ShowPicWidth) {

		int min_x = (int)rectWin.right - (int)m_ShowPicWidth;

		//When the display area is larger than the window, the upper left corner of the picture cannot be greater than 0
		//Move the mouse to the left
		if(xOffset > 0) {

			if(min_x <= m_ShowX) {
				m_ShowX -= xOffset;
				if(m_ShowX < min_x) m_ShowX = min_x;
			}
		} else {

			if(0 >= m_ShowX) {
				m_ShowX -= xOffset;
				if(m_ShowX > 0)m_ShowX = 0;
			}
		}

	}

	//When the height of the displayed picture is greater than the current window
	if(rectWin.bottom < m_ShowPicHeight) {

		int min_y = (int)rectWin.bottom - (int)m_ShowPicHeight;

		//Mouse moves up
		if(yOffset > 0) {

			if(min_y <= m_ShowY) {
				m_ShowY -= yOffset;
				if(m_ShowY < min_y) m_ShowY = min_y;
			}
		} else {

			if(0 >= m_ShowY) {
				m_ShowY -= yOffset;
				if(m_ShowY > 0)m_ShowY = 0;
			}
		}

	}

	Paint();
}


void CShowPictureWithZoom::CalcMinZoomRatio()
{
	double dZoomRatio;

	for(dZoomRatio = 1.0; dZoomRatio >= m_dZoomRatioShow; dZoomRatio /= CHANGE_RATIO) {
	}

	m_dZoomRatio = dZoomRatio;
}

BOOL CShowPictureWithZoom::CalcFixedRatioAndSizeOnInit(__int64 &_inout_cx, __int64 &_inout_cy, int _in_min_cx, int _in_min_cy)
{

	//Current resolution
	const int width = GetSystemMetrics(SM_CXSCREEN);
	const int height = GetSystemMetrics(SM_CYSCREEN);

	double screenx_d_realx = width / (double)m_picWidth;
	double screeny_d_realy = height / (double)m_picHeight;

	BOOL bNeedShowMax = TRUE;
	//Compare screenx_d_realx and screeny_d_realy, which one indicates which side crosses the boundary first
	if(screenx_d_realx < screeny_d_realy) {

		if(width < m_picWidth) {
			m_dZoomRatio = screenx_d_realx;

		} else {
			bNeedShowMax = FALSE;

		}
	} else {

		if(height < m_picHeight) {
			m_dZoomRatio = screeny_d_realy;

		} else {
			bNeedShowMax = FALSE;

		}
	}
	m_dZoomRatioShow = m_dZoomRatio;

	if((1.0 - EPSILON) > m_dZoomRatioShow) {
		CalcMinZoomRatio();
	}


	m_ShowPicWidth = m_dZoomRatioShow * m_picWidth;
	m_ShowPicHeight = m_dZoomRatioShow * m_picHeight;

	if(bNeedShowMax) {
		return TRUE;
	} else {
		_inout_cx = (m_picWidth > _in_min_cx) ? m_picWidth + 10 : _in_min_cx + 10;
		_inout_cy = (m_picHeight > _in_min_cy) ? m_picHeight + 30 : _in_min_cy + 30;
		return FALSE;
	}

}



double CShowPictureWithZoom::CalcFixedRatioByClientSize(UINT uRealX, UINT uRealY, UINT uToX, UINT uToY)
{
	//Calculate what is the right zoom ratio
	double zoomx = uToX / (double)uRealX;
	double zoomy = uToX / (double)uRealX;

	return zoomx > zoomy ? zoomy : zoomx;
}

void CShowPictureWithZoom::ChangeClientSize(WORD nWidth, WORD nHeight)
{
	m_ShowX = ((int)nWidth - (int)m_ShowPicWidth) / 2;
	m_ShowY = ((int)nHeight - (int)m_ShowPicHeight) / 2;

	//Calculate what is the right zoom ratio
	m_fixedZoomRatio = CalcFixedRatioByClientSize(m_picWidth, m_picHeight, nWidth, nHeight);

	Paint();
}

void CShowPictureWithZoom::Paint()
{
	CShowPicture::Paint(m_ShowX, m_ShowY, 0, 0, m_dZoomRatioShow);

	//Calculate invalid area
	//x
	RECT	rectWin;
	GetClientRect(m_hWndShow, &rectWin);

	if(m_ShowX > 0) {

		RECT rectInvalid = { 0, 0, m_ShowX, rectWin.bottom };
		InvalidateRect(m_hWndShow, &rectInvalid, TRUE);
	}

	if(m_ShowY > 0) {

		RECT rectInvalid = { m_ShowX >0 ? m_ShowX : 0, 0, rectWin.right, m_ShowY };
		InvalidateRect(m_hWndShow, &rectInvalid, TRUE);
	}

	int iShowBottom = m_ShowY + m_ShowPicHeight;

	if(iShowBottom < rectWin.bottom) {

		RECT rectInvalid = { m_ShowX >0 ? m_ShowX : 0, iShowBottom, rectWin.right, rectWin.bottom };
		InvalidateRect(m_hWndShow, &rectInvalid, TRUE);
	}

	int iShowRight = m_ShowX + m_ShowPicWidth;
	if(iShowRight < rectWin.right) {

		RECT rectInvalid = { iShowRight, m_ShowY >0 ? m_ShowY : 0,rectWin.right, iShowBottom < rectWin.bottom ? iShowBottom : rectWin.bottom };
		InvalidateRect(m_hWndShow, &rectInvalid, TRUE);
	}
}