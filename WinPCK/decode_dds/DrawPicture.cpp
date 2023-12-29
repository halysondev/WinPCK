//////////////////////////////////////////////////////////////////////
// DrawPicture.cpp: display dds, tga images
// Decode dds, tga and display
//
// This program is written by Li Qiufeng/stsm/liqf
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2018.5.29
//////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include <assert.h>
#include "ShowPicture.h"
#include "DdsTgaDecoder.h"


BOOL CShowPicture::BuildImage()
{
	//Read dds header
	CDdsTgaDecoder cDdsTgaDecoder;
	switch(m_picFormat) {
	case FMT_DDS:

		if(!cDdsTgaDecoder.DecodeDDS(m_resBufferInClass, m_bufsize, m_DecodedDIBData, m_picWidth, m_picHeight, m_szPictureFormat)) {
			MessageBoxA(NULL, TEXT_SHOWPIC_ERROR, NULL, MB_OK | MB_ICONERROR);
			assert(FALSE);
			return FALSE;
		}
		{
			free(m_resBufferInClass);
			m_resBufferInClass = NULL;
			m_lpmyImage = new Bitmap(m_picWidth, m_picHeight, cDdsTgaDecoder.GetStride(), cDdsTgaDecoder.GetPixelFormat(), m_DecodedDIBData);
		}
		break;

	case FMT_TGA:

		if(!cDdsTgaDecoder.DecodeTGA(m_resBufferInClass, m_bufsize, m_DecodedDIBData, m_picWidth, m_picHeight, m_szPictureFormat)) {
			MessageBoxA(NULL, TEXT_SHOWPIC_ERROR, NULL, MB_OK | MB_ICONERROR);
			assert(FALSE);
			return FALSE;
		}

		free(m_resBufferInClass);
		m_resBufferInClass = NULL;

		m_lpmyImage = new Bitmap(m_picWidth, m_picHeight, cDdsTgaDecoder.GetStride(), cDdsTgaDecoder.GetPixelFormat(), m_DecodedDIBData);
		break;

	case FMT_RAW:	//bmp,jpg,png..

		CreateStreamOnHGlobal(m_hGlobal, FALSE, &m_stream);
		if(!m_stream) {
			MessageBoxA(NULL, TEXT_SHOWPIC_ERROR, NULL, MB_OK | MB_ICONERROR);
			assert(FALSE);
			return FALSE;
		}

		//show picture
		m_lpmyImage = new Image(m_stream);

		//Look at the picture size and display window size
		m_picWidth = m_lpmyImage->GetWidth();
		m_picHeight = m_lpmyImage->GetHeight();

		break;

	default:

		MessageBoxA(NULL, TEXT_NOT_DEFINE, NULL, MB_OK | MB_ICONERROR);
		return FALSE;
		break;
	}

	return TRUE;
}

BOOL CShowPicture::DrawPictureOnDC()
{
	DrawBlockOnDlg();

	m_lpoGraph = new Graphics(m_MemDC);

	m_lpoGraph->DrawImage(m_lpmyImage, 0, 0, m_picWidth, m_picHeight);

	if(NULL != m_lpoGraph)
		delete m_lpoGraph;
	m_lpoGraph = NULL;

	//The following in the source code is executed when FMT_RAW == iFormat
	if (FMT_RAW == m_picFormat) {

		//m_lpmyImage cannot be released now, it will be used when saving as png
		if(NULL != m_lpmyImage)
			delete m_lpmyImage;
		m_lpmyImage = NULL;

		if (NULL != m_stream)
			m_stream->Release();
		m_stream = NULL;

		if (NULL != m_hGlobal)
			GlobalFree(m_hGlobal);
		m_hGlobal = NULL;
	}

	return TRUE;
}