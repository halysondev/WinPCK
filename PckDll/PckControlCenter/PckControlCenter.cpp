//////////////////////////////////////////////////////////////////////
// PckControlCenter.cpp: used to parse the data in the pck file of Perfect World Company and display it in the List
// Header file, data interaction between interface and PCK class, control center
//
// This program is written by Li Qiufeng/stsm/liqf. The pck structure refers to Ruoshui's pck structure.txt, and
// Refer to the part of its Yi language code and read the pck file list
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2012.10.10
//////////////////////////////////////////////////////////////////////

#include "PckControlCenter.h"
#include "PckClass.h"
#include "PckClassLog.h"
#include <thread>
	
//const DWORD	CPckControlCenter::m_dwNumberOfProcessors = thread::hardware_concurrency();

CPckControlCenter::CPckControlCenter():
	m_lpClassPck(NULL),
	m_lpPckRootNode(NULL),
	m_emunFileFormat(FMTPCK_UNKNOWN),
	cParams({ 0 })
{
	init();
	New();
}

CPckControlCenter::~CPckControlCenter()
{
	regMsgFeedback(NULL, DefaultFeedbackCallback);
	Close();
	uninit();
	Logger.OutputVsIde(__FUNCTION__"\r\n");
}

void CPckControlCenter::init()
{
	cParams.lpPckControlCenter = this;
	//cParams.code_page = 936;
	cParams.dwCompressLevel = getDefaultCompressLevel();
	cParams.dwMTThread = thread::hardware_concurrency();
	cParams.dwMTMaxMemory = getMaxMemoryAllowed();
}

void CPckControlCenter::uninit()
{
	lpszFilePathToAdd.clear();
}

void CPckControlCenter::Reset(uint32_t dwUIProgressUpper)
{
	memset(&cParams.cVarParams, 0, sizeof(PCK_VARIETY_PARAMS));
	cParams.cVarParams.dwUIProgressUpper = dwUIProgressUpper;
}
