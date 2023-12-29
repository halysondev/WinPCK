//////////////////////////////////////////////////////////////////////
// PckControlCenterParams.cpp: used to parse the data in the pck file of Perfect World Company and display it in the List
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
#include <thread>

#pragma region thread control

//Thread running parameters
BOOL CPckControlCenter::isThreadWorking()
{
	return cParams.cVarParams.bThreadRunning;
}

void CPckControlCenter::ForceBreakThreadWorking()
{
	cParams.cVarParams.bForcedStopWorking = TRUE;
}

#pragma endregion


#pragma region Thread task information
//
int CPckControlCenter::GetLastErrorMsg()
{
	return cParams.cVarParams.errMessageNo;
}

BOOL CPckControlCenter::isLastOptSuccess()
{
	return (PCK_OK == GetLastErrorMsg());
}

#pragma endregion

#pragma region Memory usage
//Memory usage
uint32_t CPckControlCenter::getMTMemoryUsed()
{
	return cParams.cVarParams.dwMTMemoryUsed;
}

//memory value
void CPckControlCenter::setMTMaxMemory(uint32_t dwMTMaxMemory)
{
	if ((0 < dwMTMaxMemory) && (getMaxMemoryAllowed() >= dwMTMaxMemory)) {
		cParams.dwMTMaxMemory = dwMTMaxMemory;
	}
	else {
		cParams.dwMTMaxMemory = getMaxMemoryAllowed();
	}
}

uint32_t CPckControlCenter::getMTMaxMemory()
{
	return cParams.dwMTMaxMemory;
}

//Maximum memory getDefaultMaxMemoryAllowed
uint32_t CPckControlCenter::getMaxMemoryAllowed()
{
	return MT_MAX_MEMORY;
}

#pragma endregion

#pragma region Threads


uint32_t CPckControlCenter::getMaxThread()
{
	return cParams.dwMTThread;
}

void CPckControlCenter::setMaxThread(uint32_t dwThread)
{
	cParams.dwMTThread = dwThread;
}

//Thread default parameters
uint32_t CPckControlCenter::getMaxThreadUpperLimit()
{
	return (thread::hardware_concurrency() + ((thread::hardware_concurrency() + (thread::hardware_concurrency() & 1)) >> 1));
}

#pragma endregion

#pragma region Compression level

//Compression level
uint32_t CPckControlCenter::getCompressLevel()
{
	return cParams.dwCompressLevel;
}

void CPckControlCenter::setCompressLevel(uint32_t dwCompressLevel)
{
	cParams.dwCompressLevel = dwCompressLevel;
}

//Compression level default parameters
//Parameter default value
uint32_t CPckControlCenter::getDefaultCompressLevel()
{
	return Z_DEFAULT_COMPRESS_LEVEL;
}

uint32_t CPckControlCenter::getMaxCompressLevel()
{
	return MAX_COMPRESS_LEVEL;
}

#pragma endregion


#pragma region Progress related

uint32_t CPckControlCenter::getUIProgress()
{
	return cParams.cVarParams.dwUIProgress;
}

void CPckControlCenter::setUIProgress(uint32_t dwUIProgress)
{
	cParams.cVarParams.dwUIProgress = dwUIProgress;
}

uint32_t CPckControlCenter::getUIProgressUpper()
{
	return cParams.cVarParams.dwUIProgressUpper;
}

//void CPckControlCenter::setUIProgressUpper(DWORD dwUIProgressUpper)
//{
//	cParams.cVarParams.dwUIProgressUpper = dwUIProgressUpper;
//}

#pragma endregion

#pragma region Print the results of adding/adding files

uint32_t CPckControlCenter::GetUpdateResult_OldFileCount()
{
	return cParams.cVarParams.dwOldFileCount;
}

uint32_t CPckControlCenter::GetUpdateResult_PrepareToAddFileCount()
{
	return cParams.cVarParams.dwPrepareToAddFileCount;
}

uint32_t CPckControlCenter::GetUpdateResult_ChangedFileCount()
{
	return cParams.cVarParams.dwChangedFileCount;
}

uint32_t CPckControlCenter::GetUpdateResult_DuplicateFileCount()
{
	return cParams.cVarParams.dwDuplicateFileCount;
}

uint32_t CPckControlCenter::GetUpdateResult_FinalFileCount()
{
	return cParams.cVarParams.dwFinalFileCount;
}

#pragma endregion
