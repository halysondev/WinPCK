#pragma once
#include <stdint.h>

typedef int                 BOOL;

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

class CRaw2HexString
{
	//function
public:
	//iShowStartAddr Change the starting value of the displayed address value
	CRaw2HexString(uint8_t* lpbuf, size_t rawlength, __int64 iShowStartAddr = 0);
	//CRaw2HexString(LPBYTE lpbuf, size_t rawlength, char *_dst, size_t _dstsize, __int64 iShowStartAddr = 0);
	//CRaw2HexString(LPBYTE lpbuf, size_t rawlength, wchar_t *_dst, size_t _dstsize, __int64 iShowStartAddr = 0);
	virtual ~CRaw2HexString();

	const char *GetHexString();
private:

	char *	m_buffer;
	BOOL	m_ok;
};