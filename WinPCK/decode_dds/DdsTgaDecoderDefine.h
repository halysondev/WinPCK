//////////////////////////////////////////////////////////////////////
// miscpicdlg.h: WinPCK interface thread part
// Dialog code, module for decoding dds and tga
//
// This program is written by Li Qiufeng/stsm/liqf
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2012.4.10
//////////////////////////////////////////////////////////////////////
#include <windows.h>

#ifndef _MISCPICDLG_H_
#define _MISCPICDLG_H_

//#define	FMT_RAW			0	//bmp,jpg,png,gif,emf.....
//#define	FMT_DDS			1
//#define	FMT_TGA			2


#define	TEXT_SAVE_FILTER		L"PNG file(*.png)\0*.png\0\0"

#define	TEXT_SHOWPIC_ERROR		"Image display failed£¡"
#define	TEXT_SAVE_DDS_ERROR		"fail to open the file£¡"
#define	TEXT_DDS_NOT_SUPPORT	"This type is not supported£¡"
#define	TEXT_NOT_DEFINE			"undefined format£¡"

#define	TEXT_PNG_OUTOFMEMORY	"memory overflow"
#define	TEXT_MALLOC_FAIL		"Failed to apply for memory£¡"


#define	FMTTGA_A1R5G5B5		16
#define	FMTTGA_R8G8B8		24
#define	FMTTGA_A8R8G8B8		32

#define	FMTTGA_RAWTBL	1
#define	FMTTGA_RAWRGB	2
#define	FMTTGA_RLETBL	9
#define	FMTTGA_RLERGB	10

typedef enum { FMT_DDS, FMT_TGA, FMT_RAW, FMT_UNKNOWN } PICFORMAT;

typedef unsigned __int64 QWORD, *LPQWORD;

#pragma pack(push)
#pragma pack(4)

typedef struct {
	BYTE	b;
	BYTE	g;
	BYTE	r;
	BYTE	a;
}CBRGB;

typedef union _CMYRGB{
	CBRGB cRGBVal;
	UINT32 dwargb;
}CMYRGB, *LPCRGB;

#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)

typedef struct _TGAHEAD {

	BYTE	byteTgaInfoSize;		//The number of characters in the image information field (see later in this subtable)
									//This field is a 1-byte unsigned integer, indicating the length of the image format difference field
									//The value range is 0 to 255. When it is 0, it means there is no image.
									//Information field.
	BYTE	byteColorTable;			//Color table type
									//This field contains a binary 1 to represent the image corresponding to format 1
	BYTE	byteTgaType;			//Image type code
									//This field is always 1, which is why this type is format 1
	//Color table specifications, if the color table type field is 0, it is ignored; otherwise, the description is as follows
	UINT16	wColorTableOffset;		//Color table first address
	UINT16	wColorTableSize;		//The length of the color table
	BYTE	byteColorTableBits;		//The number of bits in the color table entry
	//Image specifications
	UINT16	wXOffset;				//Image x coordinate starting position
	UINT16	wYOffset;				//Image y coordinate starting position
	UINT16	wWidth;					//image width
	UINT16	wHeight;				//image height
	BYTE	byteBitsPerPixel;		//The number of bits occupied by each pixel of the image
	//image descriptor bytes
	BYTE	byteAttrBitsPerPixel:4;	//bits 3-0 - the number of attribute bits corresponding to each pixel;
									//For TGA 16, this value is 0 or 1,
									//For TGA 24, this value is 0,
									//For TGA 32, the value is 8
	BYTE	byteTgaOtherConfig:4;	//bit 4 - reserved, must be 0
									//bit 5 - Screen starting position flag
									//0 = origin is in the lower left corner
									//1 = origin is in the upper left corner
									//For truevision image must be 0
									//bits 7-6 - Cross data storage flag
									// 00 = no crossover
									// 01 = two-way odd/even crossover
									// 10 = four-way intersection
									// 11 = reserved
} TGAHEAD, *LPTGAHEAD;

//typedef struct _BMPHEAD {
//
//	UINT16	wBmpHead;
//	UINT32	dwFilesize;
//	UINT32	dwReserved;
//	UINT32	dwBmpDataOffset;
//
//	UINT32	dwBmpHeaderSize;
//	UINT32	dwWidth;
//	UINT32	dwHeight;
//	UINT16	wPlanes;
//	UINT16	wBitsPerPixel;
//	UINT32	dwCompression;
//	UINT32	dwBitmapDataSize;
//	UINT32	dwhresolution;//ec4
//	UINT32	dwvresolution;//ec4
//	UINT32	dwColors;
//	UINT32	dwImportantColors;
//
//	//struct palette{
//	//	BYTE	blue;
//	//	BYTE	green;
//	//	BYTE	red;
//	//	BYTE	zero;
//	//};
//
//} BMPHEAD, *LPBMPHEAD;
//
//typedef struct _BMPHEAD {
//	BITMAPFILEHEADER filehead;
//	BITMAPV4HEADER	DIBhead;
//} BMPHEAD, *LPBMPHEAD;
//

//typedef struct tagBITMAPINFOHEADER{
//  DWORD  biSize; 
//  LONG   biWidth; 
//  LONG   biHeight; 
//  WORD   biPlanes; 
//  WORD   biBitCount; 
//  DWORD  biCompression; 
//  DWORD  biSizeImage; 
//  LONG   biXPelsPerMeter; 
//  LONG   biYPelsPerMeter; 
//  DWORD  biClrUsed; 
//  DWORD  biClrImportant; 
//} BITMAPINFOHEADER, *PBITMAPINFOHEADER; 

//BITMAPINFOHEADER
#pragma pack(pop)

#endif

