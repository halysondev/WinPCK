//////////////////////////////////////////////////////////////////////
// PckClassLog.cpp: information feedback, information output, debugging, etc.
//
// This program is written by Li Qiufeng/stsm/liqf. The pck structure refers to Ruoshui's pck structure.txt, and
// Refer to the part of its Yi language code and read the pck file list
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2015.5.19
//////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4996 )

#include <stdio.h>
#include "PckClassLog.h"
#include "CharsCodeConv.h"
#include <assert.h>


CPckClassLog& Logger = CPckClassLog::GetInstance();

ShowLogW	CPckClassLog::ShowLogExtern = CPckClassLog::PrintLogToConsole;

CPckClassLog::CPckClassLog()
{
}

CPckClassLog::~CPckClassLog()
{
#if PCK_DEBUG_OUTPUT
	OutputDebugStringA(__FUNCTION__"\r\n");
#endif
}

void CPckClassLog::PckClassLog_func_register(ShowLogW _ShowLogW)
{
	ShowLogExtern = _ShowLogW;
}

wchar_t* CPckClassLog::GetErrorMsg(CONST DWORD dwError, wchar_t *szMessage)
{
	//static WCHAR szMessage[1024] = L"wrong reason£º";
	wcscpy(szMessage, L"wrong reason£º");

	size_t len = wcslen(szMessage);
	WCHAR *lpszMessage = szMessage + len;
	// retrieve a message from the system message table
	if(!FormatMessageW(
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		lpszMessage,
		LOG_BUFFER - len,
		NULL)) {
		wcscpy(lpszMessage, L"unknown mistake");
	}
	SetLastError(0);
	return szMessage;
}

#pragma region PrintLogToDst

void CPckClassLog::ShowLog(const char _loglevel, const char *_logtext)
{
	CAnsi2Ucs cA2U(936);
	ShowLog(_loglevel, cA2U.GetString(_logtext));
}

void CPckClassLog::ShowLog(const char _loglevel, const wchar_t *_logtext)
{
	ShowLogExtern(_loglevel, _logtext);
}

void CPckClassLog::PrintLogToConsole(const char log_level, const wchar_t *str)
{
	
	SYSTEMTIME systime;
	GetLocalTime(&systime);
	printf("%02d:%02d:%02d :%c ", systime.wHour, systime.wMinute, systime.wSecond, log_level);

	wprintf(str);
	printf("\r\n");
}

void CPckClassLog::e(const char *_text, ...)
{
	va_list	ap; 
	va_start(ap, _text); 
	PrintLog('E', _text, ap);
	va_end(ap); 
	PrintErrorLog();
	assert(FALSE);
}

void CPckClassLog::w(const char *_text, ...)
{
	va_list	ap;
	va_start(ap, _text);
	PrintLog('W', _text, ap);
	va_end(ap);
}

void CPckClassLog::i(const char *_text, ...)
{
	va_list	ap;
	va_start(ap, _text);
	PrintLog('I', _text, ap);
	va_end(ap);
}

void CPckClassLog::d(const char *_text, ...)
{
	va_list	ap;
	va_start(ap, _text);
	PrintLog('D', _text, ap);
	va_end(ap);
}

void CPckClassLog::n(const char *_text, ...)
{
	va_list	ap;
	va_start(ap, _text);
	PrintLog('N', _text, ap);
	va_end(ap);
}

void CPckClassLog::e(const wchar_t *_text, ...)
{
	va_list	ap;
	va_start(ap, _text);
	PrintLog('E', _text, ap);
	va_end(ap);
	PrintErrorLog();
	assert(FALSE);
}

void CPckClassLog::w(const wchar_t *_text, ...)
{
	va_list	ap;
	va_start(ap, _text);
	PrintLog('W', _text, ap);
	va_end(ap);
}

void CPckClassLog::i(const wchar_t *_text, ...)
{
	va_list	ap;
	va_start(ap, _text);
	PrintLog('I', _text, ap);
	va_end(ap);
}

void CPckClassLog::d(const wchar_t *_text, ...)
{
	va_list	ap;
	va_start(ap, _text);
	PrintLog('D', _text, ap);
	va_end(ap);
}

void CPckClassLog::n(const wchar_t *_text, ...)
{
	va_list	ap;
	va_start(ap, _text);
	PrintLog('N', _text, ap);
	va_end(ap);
}

#pragma region PrintLog

void CPckClassLog::PrintErrorLog()
{
	wchar_t szMessage[LOG_BUFFER];

	m_dwLastError = GetLastError();
	if (0 != m_dwLastError) {
		ShowLog(' ', GetErrorMsg(m_dwLastError, szMessage));
		m_dwLastError = 0;
	}
#if PCK_DEBUG_OUTPUT
	else {
		ShowLog(' ', "System error code not obtained");
	}
#endif
}

void CPckClassLog::PrintLog(const char chLevel, const char *_fmt, va_list ap)
{
	char _maintext[LOG_BUFFER];
	_vsnprintf(_maintext, sizeof(_maintext), _fmt, ap);

	ShowLog(chLevel, _maintext);
}

void CPckClassLog::PrintLog(const char chLevel, const wchar_t *_fmt, va_list ap)
{
	wchar_t _maintext[LOG_BUFFER];
	_vsnwprintf(_maintext, sizeof(_maintext), _fmt, ap);

	ShowLog(chLevel, _maintext);
}

#pragma endregion

#if PCK_DEBUG_OUTPUT
void CPckClassLog::OutputVsIde(const char *_text, ...)
{
	char _maintext[LOG_BUFFER];
	va_list	ap;
	va_start(ap, _text);
	_vsnprintf(_maintext, sizeof(_maintext), _text, ap);
	va_end(ap);
	OutputDebugStringA(_maintext);
}
#endif

#if PCK_DEBUG_OUTPUT_FILE
#pragma warning ( disable : 4996 )

int CPckClassLog::logOutput(const char *file, const char *format, ...)
{
	char szFile[MAX_PATH];
	const int BUFFER_SIZE = 4097;
	char strbuf[BUFFER_SIZE];
	memset(strbuf, 0, sizeof(strbuf));

	const char *lpszTitle = strstr(file, "::");
	if (0 != lpszTitle) {
		sprintf_s(szFile, "d:\\pcktest\\dst\\%s.log", lpszTitle + 2);
	}
	else {
		sprintf_s(szFile, "d:\\pcktest\\dst\\%s.log", file);
	}


	FILE *pFile = fopen(szFile, "ab");
	int ret = -1;
	if (pFile != NULL) {

		va_list ap;
		va_start(ap, format);
		int result = ::vsnprintf(strbuf, BUFFER_SIZE - 1, format, ap);
		va_end(ap);

		std::lock_guard<std::mutex> lckLogFile(m_LockLogFile);
		fseek(pFile, 0, SEEK_END);
		int ret = fwrite(strbuf, 1, strlen(strbuf), pFile);
		fclose(pFile);
		pFile = NULL;
	}

	return ret;
}
#endif
