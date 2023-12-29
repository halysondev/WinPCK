//////////////////////////////////////////////////////////////////////
// GetDragPath.cpp: WinPCK interface thread part
// Get the ShellWindow path
//
// This program is written by Li Qiufeng/stsm/liqf
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2012.4.10
//////////////////////////////////////////////////////////////////////

#include <exdisp.h>
#include <atlbase.h>
#include <Windows.h>
#include "GetDragPath.h"

static const char  *hexstr = "0123456789abcdef";


inline char hexchar2char(wchar_t ch)
{
	if(ch >= '0' && ch <= '9')
		return ch - '0';
	ch = toupper(ch);
	if(ch >= 'A' && ch <= 'Z')
		return ch - 'A' + 10;
	return 0;
}

void decodeandcopy(wchar_t *dst, wchar_t *src)
{
	int len = wcslen(src);
	while(*src) {
		if('/' == *src) {

			*dst++ = '\\';
			src++;
		} else if('%' == *src) {

			if(*(src + 1) && *(src + 2)) {

				char	ch1 = 0;

				src++;

				ch1 = hexchar2char(*src++);
				ch1 <<= 4;
				ch1 += hexchar2char(*src++);

				*dst++ = ch1;
			} else {
				*dst++ = *src++;
			}
		} else {
			*dst++ = *src++;
		}
	}
	*dst = 0;
}

//Directly obtain the window of explorer.exe where the mouse position is located through Com and obtain the path
BOOL GetWndPath(HWND hWnd, wchar_t * szPath)
{
	BOOL rtn = FALSE;
	tagVARIANT	vari;
	vari.vt = VT_I4;

	CComPtr<IShellWindows> psw;

	psw.CoCreateInstance(CLSID_ShellWindows);
	if(psw) {

		long lShellWindowCount = 0;
		psw->get_Count(&lShellWindowCount);

		for(long i = 0;i < lShellWindowCount;i++) {

			CComPtr<IDispatch> pdispShellWindow;

			vari.lVal = i;
			psw->Item(vari, &pdispShellWindow);

			CComQIPtr<IWebBrowser2> pIE(pdispShellWindow);

			if(pIE) {

				//TCHAR	strWindowClass[MAX_PATH];
				HWND hWndID = NULL;
				pIE->get_HWND((SHANDLE_PTR*)&hWndID);

				//wchar_t testa[MAX_PATH]; 
				//CComBSTR	bstrURL;
				//pIE->get_LocationURL(&bstrURL);

				if(hWnd == hWndID) {

					CComBSTR	bstrURL;
					//bstrURL.Attach(szPath);

					pIE->get_LocationURL(&bstrURL);


					if(0 == wcsncmp(bstrURL.m_str, L"file://", 7)) {
						if('/' == *(bstrURL.m_str + 7))
							//lstrcpy(szPath, bstrURL.m_str + 8);
							decodeandcopy(szPath, bstrURL.m_str + 8);
						else
							//lstrcpy(szPath, bstrURL.m_str + 5);
							decodeandcopy(szPath, bstrURL.m_str + 5);

						//TCHAR *lpszPathPtr = szPath;
						//while(0 != *lpszPathPtr)
						//{
						//	if('/' == *lpszPathPtr)*lpszPathPtr = '\\';
						//	lpszPathPtr++;
						//}
#ifdef _DEBUG
						OutputDebugStringW(L"GetWndPath:");
						OutputDebugStringW(szPath);
						OutputDebugStringW(L"\r\n");
#endif
					}

					rtn = TRUE;
				}
			}
			pIE.Release();
			pdispShellWindow.Release();
		}

	}
	psw.Release();

	return rtn;
}


