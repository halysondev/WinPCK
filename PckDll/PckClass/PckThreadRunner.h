#pragma once

#include "MapViewFileMultiPck.h"
#include "PckClassWriteOperator.h"
#include "PckClassLog.h"

#include <functional>
#include <deque>
#include <mutex>


//Get file method
typedef enum { DATA_FROM_FILE, DATA_FROM_PCK } PCK_DATA_SOURCE;

//Get file return value
typedef enum { FD_OK, FD_END, FD_ERR, FD_CANCEL } FETCHDATA_RET;

typedef std::function<FETCHDATA_RET(PCKINDEXTABLE&)> FETCHDATA_FUNC;

class CPckClassWriteOperator;

typedef struct _DataFetchMethod
{
	vector<FILES_TO_COMPRESS>::const_iterator ciFilesList;
	vector<FILES_TO_COMPRESS>::const_iterator ciFilesListEnd;

	CMapViewFileMultiPckRead		*lpFileReadPCK;
	LPPCKINDEXTABLE					lpPckIndexTablePtrSrc;
	uint32_t						dwProcessIndex;
	uint32_t						dwTotalIndexCount;
	int								iStripFlag;
	// The file path in pck corresponding to the node (currently displayed in the interface thread)
	wchar_t							szCurrentNodeString[MAX_PATH_PCK_260];
	//its length
	int								nCurrentNodeStringLen;

}DATA_FETCH_METHOD, *LPDATA_FETCH_METHOD;

typedef struct _ThreadParams
{
	PCK_DATA_SOURCE				pck_data_src;
	DATA_FETCH_METHOD			cDataFetchMethod;
	CMapViewFileMultiPckWrite *	lpFileWrite;
	CPckClassWriteOperator *	lpPckClassThreadWorker;
	LPPCK_ALL_INFOS				lpPckAllInfo;
	LPPCK_RUNTIME_PARAMS		pckParams;

	uint64_t					dwAddressStartAt;

	//int							threadnum;
	//Estimated compressed file size
	uint64_t					qwCompressTotalFileSize;
	//The target number of written data, generally =mt_dwFileCount, when adding = the number of effective files during heavy compression
	uint32_t					dwFileCountOfWriteTarget;
}THREAD_PARAMS, *LPTHREAD_PARAMS;

#define MALLOCED_EMPTY_DATA			(1)

template <typename T>
_inline T * __fastcall mystrcpy(T * dest, const T *src)
{
	while ((*dest = *src))
		dest++, src++;
	return dest;
}


typedef std::function<void()> CompressThreadFunc;
typedef std::function<void()> WriteThreadFunc;

class CPckThreadRunner
{
public:
	CPckThreadRunner(LPTHREAD_PARAMS threadparams);
	virtual ~CPckThreadRunner();
	CPckThreadRunner(CPckThreadRunner const&) = delete;
	CPckThreadRunner& operator=(CPckThreadRunner const&) = delete;

	void start();


private:
	LPTHREAD_PARAMS			m_threadparams = nullptr;
	LPPCK_RUNTIME_PARAMS	m_lpPckParams = nullptr;
	CPckClassWriteOperator * m_lpPckClassBase;

	std::mutex				m_LockCompressedflag;

	std::mutex				m_LockReadFileMap;
#if PCK_DEBUG_OUTPUT
	std::mutex				m_LockThreadID;
	int						m_threadID = 0;		//Thread ID
#endif

private:
#if PCK_DEBUG_OUTPUT
	uint32_t				m_dwCurrentQueuePosPut = 0, m_dwCurrentQueuePosGet = 0;				//Current queue position
#endif

protected:
	uint64_t				mt_dwAddressQueue;												//The location of the written file in the global compression process is only controlled by Queue.
	uint64_t				mt_dwAddressNameQueue;											//The starting position of the compressed file name index of the read pck file

private:
	std::mutex					m_LockQueue, m_LockMaxMemory;

	std::condition_variable		m_cvReadyToPut, m_cvMemoryNotEnough;
	BOOL						m_memoryNotEnoughBlocked = FALSE;

	deque<PCKINDEXTABLE>		m_QueueContent;
	vector<PCKINDEXTABLE_COMPRESS> m_Index_Compress;



private:

	void startThread();

	void CompressThread(FETCHDATA_FUNC GetUncompressedData);
	void WriteThread(LPTHREAD_PARAMS threadparams);

	//Memory usage when compressing
	FETCHDATA_RET	detectMaxToAddMemory(DWORD dwMallocSize);
	FETCHDATA_RET	detectMaxAndAddMemory(LPBYTE &_out_buffer, DWORD dwMallocSize);
	void	freeMaxToSubtractMemory(DWORD dwMallocSize);
	void	freeMaxAndSubtractMemory(LPBYTE &_out_buffer, DWORD dwMallocSize);

	//Compressed data queue

	BOOL	putCompressedDataQueue(PCKINDEXTABLE &lpPckFileIndexToCompress);
	BOOL	getCompressedDataQueue(LPBYTE &lpBuffer, PCKINDEXTABLE_COMPRESS &lpPckIndexTable);

	//Obtain compressed source data in multi-threaded operations
	FETCHDATA_RET		GetUncompressedDataFromFile(LPDATA_FETCH_METHOD lpDataFetchMethod, PCKINDEXTABLE &pckFileIndex);
	FETCHDATA_RET		GetUncompressedDataFromPCK(LPDATA_FETCH_METHOD lpDataFetchMethod, PCKINDEXTABLE &pckFileIndex);

};

