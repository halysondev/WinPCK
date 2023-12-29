#pragma once
#include "DdsTgaDecoderDefine.h"
#include <Gdiplus.h>
using namespace Gdiplus;
#include "dxgiformat.h"

class CDdsTgaDecoder
{
public:
	CDdsTgaDecoder();
	~CDdsTgaDecoder();


	BOOL	Decode(LPBYTE _In_data, UINT32 _in_bufsize, LPBYTE &_out_data, UINT &_out__picWidth, UINT &_out_picHeight, PICFORMAT picFormat, LPSTR _out_szFormatDdsTga);
	BOOL	DecodeDDS(LPBYTE _In_data, UINT32 _in_bufsize, LPBYTE &_out_data, UINT &_out__picWidth, UINT &_out_picHeight, LPSTR _out_szFormatDdsTga);
	BOOL	DecodeTGA(LPBYTE _In_data, UINT32 _in_bufsize, LPBYTE &_out_data, UINT &_out__picWidth, UINT &_out_picHeight, LPSTR _out_szFormatDdsTga);

	//Returns the value of stride
	UINT	GetStride();
	//Returns the width and height of the image
	UINT	GetWidth();
	UINT	GetHeight();
	//Returns the m_decodedDIBFormat of the image
	PixelFormat GetPixelFormat();

protected:

	//Image width and height
	UINT		m_picWidth, m_picHeight;
	//Image typedds,tga,other
	//enum { FMT_DDS, FMT_TGA, FMT_RAW} m_picFormat;
	//PICFORMAT	m_picFormat;
	//The specific format of the image displayed
	LPSTR		m_lpszPictureFormat;
	//The original image file data used inside the class, when the type is RAW, points to the memory of hGlobal
	LPBYTE		m_resBufferInClass;
	//The size of the data passed in the file
	UINT32		m_bufsize;
	//DIB data of the decoded picture (DDS, TGA)
	char*		m_DecodedDIBData;
	//The size of the DIB data (DDS, TGA) of the decoded picture
	DWORD		m_dwDdsTgaToDIBDataSize;
	//
	PixelFormat	m_decodedDIBFormat;
	//How many bytes are there in one row of pixels?
	UINT		m_bytesPerLine;
	//How many pixels are there in one row of pixels?
	UINT		m_bytesPerPixel;
	//The size of one row of pixel data in the image after alignment in memory (align=4byte)
	UINT		m_stride;

	//Obtain the format of the internal DIB of dds and tga, and calculate the stride (the size of a row of pixel data in the image after memory alignment),... pass in D3DFORMAT when in dds format, and pass in tga format (int, FMTTGA_A1R5G5B5...)
	PixelFormat DetectDdsFormatAndAllocByFormat(D3DFORMAT fmt);
	PixelFormat DetectTgaFormatAndAllocByFormat(int fmt);
	BOOL CalaStride_DetectDIBFormat_AllocByFormat(UINT16 wWidth, UINT16 wHeight, PixelFormat pixelFormat);
	BOOL GetDDSFormatStringAndDecodeEntry(D3DFORMAT fmt, LPSTR lpszPictureFormat, void(CDdsTgaDecoder::* &decode_dds_dxtN)(LPBYTE));
	//Create DIB data from dds, tga data
	BOOL CreateBmpBufferByDds();
	BOOL CreateBmpBufferByTga();

#pragma region decode_dds_º¯Êý
	void	decode_dds_dxt1(BYTE *ddsimage);
	void	decode_dds_dxt3(BYTE *ddsimage);
	void	decode_dds_dxt5(BYTE *ddsimage);
	void	decode_dds_a8r8g8b8(BYTE *ddsimage);
	void	decode_dds_x8r8g8b8(BYTE *ddsimage);
	void	decode_dds_a8b8g8r8(BYTE *ddsimage);
	void	decode_dds_x8b8g8r8(BYTE *ddsimage);
	void	decode_dds_r8g8b8(BYTE *ddsimage);
	void	decode_dds_a1r5g5b5(BYTE *ddsimage);
	void	decode_dds_x1r5g5b5(BYTE *ddsimage);
	void	decode_dds_r5g6b5(BYTE *ddsimage);
	void	decode_dds_a4r4g4b4(BYTE *ddsimage);
	void	decode_dds_x4r4g4b4(BYTE *ddsimage);
	void	decode_dds_a8r3g3b2(BYTE *ddsimage);
	void	decode_dds_a2r10g10b10(BYTE *ddsimage);
	void	decode_dds_a2b10g10r10(BYTE *ddsimage);
#pragma endregion

#pragma region TGA decoder
	BOOL	makeTgaColorMappedData(BYTE * &tgaimage, int fmt, char * &bufferOfColorMappedData, UINT16 wColorTableSize/*, int nTgaBitsPerPixel*/);
	void	decode_tga_ColorMapped8(BYTE *tgaimage, char *lpColorMappedData);
	void	decode_tga_ColorMapped16(BYTE *tgaimage, char *lpColorMappedData);

	void	decode_tga_RGB(BYTE *tgaimage);

	void	decode_tga_ColorMapped8REL(BYTE *tgaimage, char *lpColorMappedData);
	void	decode_tga_ColorMapped16REL(BYTE *tgaimage, char *lpColorMappedData);

	void	decode_tga_RGBREL(BYTE *tgaimage);
#pragma endregion
};

