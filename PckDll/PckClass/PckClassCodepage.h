#pragma once
#include "PckClassBaseFeatures.h"

#define CP936	936
#define PCK_INTERNAL_CODEPAGE	CP936

class CPckClassCodepage
{
private:
	CPckClassCodepage();
	~CPckClassCodepage();

public:
	//File name transcoding, the return value is the length of the converted string (strlen, wcslen)
	//ansi -> uc cp936
	static size_t	PckFilenameCode2UCS(const char *_in, wchar_t *_out, size_t _outsize);
	//uc -> ansi cp936
	static size_t	PckFilenameCode2Ansi(const wchar_t *_in, char *_out, size_t _outsize);
	//ansi -> uc native
	static size_t	NativeFilenameCode2UCS(const char *_in, wchar_t *_out, size_t _outsize);
	//uc -> ansi native
	static size_t	NativeFilenameCode2Ansi(const wchar_t *_in, char *_out, size_t _outsize);

};

