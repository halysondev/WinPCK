//////////////////////////////////////////////////////////////////////
// PckClass.cpp: used to parse the data in the pck file of Perfect World Company and display it in the List
// Initialization of related classes, etc.
//
// This program is written by Li Qiufeng/stsm/liqf. The pck structure refers to Ruoshui's pck structure.txt, and
// Refer to the part of its Yi language code and read the pck file list
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2012.4.10
//////////////////////////////////////////////////////////////////////

#include "PckClass.h"

#pragma warning ( disable : 4996 )
#pragma warning ( disable : 4146 )
#pragma warning ( disable : 4267 )

CPckClass::CPckClass(LPPCK_RUNTIME_PARAMS inout)
{
	m_lpPckParams = inout;
	m_zlib.init_compressor(m_lpPckParams->dwCompressLevel);
}

CPckClass::~CPckClass()
{
	Logger.OutputVsIde(__FUNCTION__"\r\n");
}

BOOL CPckClass::Init(LPCWSTR	szFile)
{
	wcscpy_s(m_PckAllInfo.szFilename, szFile);
	GetFileTitleW(m_PckAllInfo.szFilename, m_PckAllInfo.szFileTitle, MAX_PATH);

	if(!MountPckFile(m_PckAllInfo.szFilename)) {

		ResetPckInfos();

		return FALSE;
	} else {

		BuildDirTree();
		return (m_PckAllInfo.isPckFileLoaded = TRUE);
	}
}
