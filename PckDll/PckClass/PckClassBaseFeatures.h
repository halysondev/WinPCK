#pragma once
#include <stdio.h>
#include "PckHeader.h"
#include "PckClassLog.h"

#include "AllocMemPool.h"
#include "PckClassZlib.h"

#include <vector>

class CPckClassBaseFeatures 
{
public:
	CPckClassBaseFeatures();
	~CPckClassBaseFeatures();

	virtual const	LPPCKINDEXTABLE		GetPckIndexTable();
	virtual const	LPPCK_PATH_NODE		GetPckPathNode();

	//Whether the file has been loaded
	BOOL	isFileLoaded();

	//File size
	uint64_t	GetPckSize();

	//Get the number of files
	uint32_t	GetPckFileCount();

	//Data area size
	uint64_t	GetPckDataAreaSize();

	//Data area redundant data size
	uint64_t	GetPckRedundancyDataSize();

protected:
	//Reset information
	void	ResetPckInfos();

	//Set progress
	void	SetParams_ProgressInc();
	void	SetParams_Progress(DWORD dwUIProgres);

	//Set the maximum value in progress information
	void	SetParams_ProgressUpper(DWORD dwUIProgressUpper, BOOL bReset = TRUE);
	void	AddParams_ProgressUpper(DWORD dwUIProgressUpperAdd);

	//multi-threaded process
	void	SetThreadFlag(BOOL isThreadWorking);
	BOOL	CheckIfNeedForcedStopWorking();
	void	SetErrMsgFlag(int errMsg);

	PCK_ALL_INFOS			m_PckAllInfo;

	CAllocMemPool			m_NodeMemPool;

	CPckClassZlib			m_zlib;

	//runtime variables 
	LPPCK_RUNTIME_PARAMS	m_lpPckParams;

};
