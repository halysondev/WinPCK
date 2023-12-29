//////////////////////////////////////////////////////////////////////
// PckClassRebuildFilter.cpp: used to parse filter scripts
//
// This program is written by Li Qiufeng/stsm/liqf, and part of the code is adapted from RapidCRC
//
// This code is open source. Please retain the original author information for any modified release based on this code.
//
// 2018.5.15
//////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include "MapViewFileMultiPck.h"
#include "PckClassRebuildFilter.h"
#include "CharsCodeConv.h"
#include "TextLineSpliter.h"

#define MAX_SCRIPT_SIZE	(10*1024*1024)

CPckClassRebuildFilter::CPckClassRebuildFilter()
{
}

CPckClassRebuildFilter::~CPckClassRebuildFilter()
{
	ResetRebuildFilterInIndexList();
	Logger.OutputVsIde(__FUNCTION__"\r\n");
}

#pragma region Read file and convert to Unicode

BOOL CPckClassRebuildFilter::OpenScriptFileAndConvBufToUcs2(const wchar_t * lpszScriptFile)
{
	char   * lpBufferToRead;
	CMapViewFileRead	cFileRead;
	
	CTextUnitsW	cText2Line;

	//Read all characters from file
	if (nullptr == (lpBufferToRead = (char*)cFileRead.OpenMappingViewAllRead(lpszScriptFile))) 
		return FALSE;

	CTextConv2UCS2 cText2Ucs;
	const wchar_t* lpszUnicodeString = cText2Ucs.GetUnicodeString(lpBufferToRead, cFileRead.GetFileSize());

	cText2Line.SplitLine(lpszUnicodeString, m_ScriptLines, LINE_TRIM_LEFT | LINE_TRIM_RIGHT | LINE_EMPTY_DELETE);

	return TRUE;
}
#pragma endregion


BOOL CPckClassRebuildFilter::ParseOneLine(FILEOP * pFileOp, LPCWSTR lpszLine)
{
	wchar_t szOperator[16] = { 0 };
	//First check if there are any spaces or tabs within the 16 characters
	const wchar_t *lpszCell = lpszLine, *lpszSearch = lpszLine;
	size_t count = 0;

	wchar_t *lpszOperator = szOperator;
	BOOL isValid = FALSE;

	while((16 > (++count)) &&
		(*lpszSearch)) {

		if((' ' == *lpszSearch) || ('\t' == *lpszSearch)) {
			isValid = TRUE;
			break;
		}
		*lpszOperator++ = *lpszSearch++;
	}

	if(!isValid)
		return FALSE;

	//Parse szOperator
	const wchar_t *lpszOpPos = wcsstr(szOperators, szOperator);
	if(NULL == lpszOpPos)
		return FALSE;

	pFileOp->op = SCRIPTOP((lpszOpPos - szOperators) / 8);

	//Filter out remaining spaces and tabs
	while((' ' == *lpszSearch) || ('\t' == *lpszSearch))
		lpszSearch++;

	isValid = FALSE;
	count = 0;

	if((MAX_PATH <= wcslen(lpszSearch)) || (0 == *lpszSearch))
		return FALSE;

	wcscpy(pFileOp->szFilename, lpszSearch);

	////Check if the file name is correct
	//if(OP_CheckFile == pFileOp->op) {

	//	if(0 == _tcsicmp(lpszFileName, pFileOp->szFilename))

	//		return TRUE;
	//	else
	//		return FALSE;
	//}

	return TRUE;
}

//Decompose directories in scripts
void CPckClassRebuildFilter::SepratePaths(FILEOP * pFileOp)
{
	wcscpy(pFileOp->szFilenameBuffer, pFileOp->szFilename);

	pFileOp->lpszSepratedPaths[0] = pFileOp->szFilenameBuffer;

	wchar_t *lpszSearch = pFileOp->szFilenameBuffer;
	int nPathDepthCount = 1;


	while(*lpszSearch) {

		wchar_t *test = wcschr(lpszSearch, L'\\');
		//The presence of \\double slashes is not considered here.
		if((L'\\' == *lpszSearch) || (L'/' == *lpszSearch)) {
			*lpszSearch = 0;
			++lpszSearch;
			pFileOp->lpszSepratedPaths[nPathDepthCount] = lpszSearch;
			++nPathDepthCount;
		}
		++lpszSearch;
	}

}

LPPCK_PATH_NODE CPckClassRebuildFilter::LocationFileIndex(LPWSTR *lpszPaths, LPPCK_PATH_NODE lpNode)
{
	LPCWSTR lpszSearchDir = *lpszPaths;

	if((NULL == lpszSearchDir) || (NULL == lpNode))
		return NULL;

	//Skip directly to the .. directory
	LPPCK_PATH_NODE lpNodeSearch = lpNode->next;

	while(1) {

		if(NULL == lpNodeSearch)
			return NULL;

		if(0 == wcsicmp(lpszSearchDir, lpNodeSearch->szName)) {

			//Whether it has been matched
			if(NULL == *(lpszPaths + 1)) {

				return lpNodeSearch;
			} else {
				return LocationFileIndex(lpszPaths + 1, lpNodeSearch->child);
			}
		}
		lpNodeSearch = lpNodeSearch->next;
	}

	return NULL;
}

void CPckClassRebuildFilter::MarkFilterFlagToFileIndex(LPPCKINDEXTABLE	lpPckIndexTable, SCRIPTOP op)
{
	switch(op) {

	case OP_Protect:
		lpPckIndexTable->isProtected = TRUE;
		break;

	case OP_Delete:
	case OP_Rmdir:
		if((!lpPckIndexTable->isInvalid) && (!lpPckIndexTable->isProtected)) {

			lpPckIndexTable->isToDeDelete = TRUE;
			lpPckIndexTable->isInvalid = TRUE;
		}
		break;
	}
	m_EditedNode.push_back(lpPckIndexTable);
}

//Applies a line of script content to the list of found files
void CPckClassRebuildFilter::MarkFilterFlagToNode(LPPCK_PATH_NODE lpNode, SCRIPTOP op)
{
	lpNode = lpNode->child->next;

	while(NULL != lpNode) {
		//if(NULL == lpNode->child) {
		if (PCK_ENTRY_TYPE_FOLDER & lpNode->entryType) {
			MarkFilterFlagToNode(lpNode, op);
		} else {
			MarkFilterFlagToFileIndex(lpNode->lpPckIndexTable, op);
		}

		lpNode = lpNode->next;
	}

}


#pragma region ApplyScript2IndexList, applies the script content to the file list

//Apply script contents to file list
BOOL CPckClassRebuildFilter::ApplyScript2IndexList(LPPCK_PATH_NODE lpRootNode)
{
	//Whether an error occurred during the parsing process
	BOOL bHasErrorHappend = FALSE;
	m_EditedNode.clear();

	for(size_t i = 0;i< m_FirstFileOp.size();i++){

		FILEOP * pFileOp = &m_FirstFileOp[i];

		if(OP_CheckFile != pFileOp->op) {

			//Decompose directories in scripts
			SepratePaths(pFileOp);

			//Locate file index
			LPPCK_PATH_NODE lpFoundNode = LocationFileIndex(pFileOp->lpszSepratedPaths, lpRootNode->child);

			if(NULL == lpFoundNode) {

				Logger.w(UCSTEXT("Parsed script failed at: %s, skipping..."), pFileOp->szFilename);
				bHasErrorHappend = TRUE;

			} else {
				if(PCK_ENTRY_TYPE_FOLDER & lpFoundNode->entryType){
				//if(NULL != lpFoundNode->child) {

					MarkFilterFlagToNode(lpFoundNode, pFileOp->op);
				} else {
					MarkFilterFlagToFileIndex(lpFoundNode->lpPckIndexTable, pFileOp->op);
				}
			}

		}
	}

	return (!bHasErrorHappend);
}

#pragma endregion

BOOL CPckClassRebuildFilter::ParseScript(const wchar_t * lpszScriptFile)
{

	if (!OpenScriptFileAndConvBufToUcs2(lpszScriptFile)) {
		Logger.w("Failed to read script");
		return FALSE; 
	}

	m_FirstFileOp.push_back(FILEOP{ 0 });
	FILEOP * pFileOp = &m_FirstFileOp.back();

	for (int i = 0; i < m_ScriptLines.size(); i++) {

		//Filter comment lines
		if (L';' != m_ScriptLines[i].at(0)) {
			//A one-line script is divided into two parts, the operation and the file name
			if (ParseOneLine(pFileOp, m_ScriptLines[i].c_str())) {

				m_FirstFileOp.push_back(FILEOP{ 0 });
				pFileOp = &m_FirstFileOp.back();

			}
			else {

				Logger.w("Script parsing failed at line %d: %ls, skipping...", i, m_ScriptLines[i].c_str());

				return FALSE;
			}
		}
	}

	Logger.i("Parsing script successful");
	return TRUE;
}

//Clear the filter information that needs to be read when rebuilding the package
void CPckClassRebuildFilter::ResetRebuildFilterInIndexList()
{
	for(DWORD i = 0;i < m_EditedNode.size();++i) {

		LPPCKINDEXTABLE lpPckIndexTable = m_EditedNode[i];

		if(lpPckIndexTable->isToDeDelete) {
			lpPckIndexTable->isInvalid = FALSE;
		}
		lpPckIndexTable->isProtected = lpPckIndexTable->isToDeDelete = FALSE;
	}
}

//Apply script content
BOOL CPckClassRebuildFilter::Apply(LPPCK_PATH_NODE lpRootNode)
{
	BOOL rtn = FALSE;

	//Apply data to tree
	rtn = ApplyScript2IndexList(lpRootNode);

	if (!rtn) {
		ResetRebuildFilterInIndexList();
		Logger.i("Apply script failed");
	}
	else {
		Logger.i("Apply script successfully");
	}

	return rtn;
}

BOOL CPckClassRebuildFilter::ApplyScript(const wchar_t * lpszScriptFile, LPPCK_PATH_NODE lpRootNode)
{
	if (!ParseScript(lpszScriptFile))
		return FALSE;

	return Apply(lpRootNode);
}

BOOL CPckClassRebuildFilter::TestScript(const wchar_t * lpszScriptFile)
{
	return ParseScript(lpszScriptFile);
}


BOOL CPckClassRebuildFilter::ModelTextureCheck(LPCWSTR lpszFilename)
{
	//Path rules, *\textures\*.dds

	LPCWSTR constTexturePath = L"\\textures\\";
	LPCWSTR constDdsExt = L".dds";
	LPCWSTR constTgaExt = L".tga";

	LPCWSTR lpszTexturePath = wcsstr(lpszFilename, constTexturePath);

	if (nullptr == lpszTexturePath)
		return FALSE;

	lpszTexturePath += wcslen(constTexturePath);

	LPCWSTR subdir = wcschr(lpszTexturePath, L'\\');
	if (nullptr != subdir) {
		if (nullptr != wcschr(subdir+1, L'\\'))
			return FALSE;
	}

	lpszTexturePath += wcslen(lpszTexturePath) - wcslen(constDdsExt);
	if (0 == wcsicmp(lpszTexturePath, constDdsExt))
		return TRUE;

	if (0 == wcsicmp(lpszTexturePath, constTgaExt))
		return TRUE;

	return FALSE;
}

void CPckClassRebuildFilter::StripModelTexture(LPPCKINDEXTABLE lpPckIndexHead, DWORD dwFileCount, LPPCK_PATH_NODE lpRootNode, LPCWSTR lpszPckFilename)
{

	int nDetectOffset = 0;
#if 0
	//Detect whether the folder in the pck directory starts with the pck file name
	//For example, the root directory in gfx.pck is gfx and there is only one
	LPPCK_PATH_NODE lpRootNodeFirstDir = lpRootNode->next;
	int nRootDirCount = 0;
	vector<wstring> sRootDirs;

	while (NULL != lpRootNodeFirstDir) {

		if (PCK_ENTRY_TYPE_FOLDER & lpRootNodeFirstDir->entryType) {

			sRootDirs.push_back(lpRootNodeFirstDir->szName);
			++nRootDirCount;
		}
		lpRootNodeFirstDir = lpRootNodeFirstDir->next;
	}

	//There are less than 3 root directories, take the root directory name and compare the file names
	if (3 > nRootDirCount) {
		wchar_t szFileTitle[MAX_PATH];
		wchar_t* lpszExt = nullptr;

		wcscpy_s(szFileTitle, lpszPckFilename);

		if (nullptr != (lpszExt = wcsrchr(szFileTitle, L'.'))) {

			*lpszExt = 0;
		}

		//Compared
		for (int i = 0; i < sRootDirs.size(); i++) {

			if (nullptr != wcsstr(szFileTitle, sRootDirs[i].c_str())) {

				nDetectOffset = sRootDirs[i].length() + 1;
				break;
			}
		}
	}
#endif

	LPPCKINDEXTABLE lpPckIndexTable = lpPckIndexHead;

	for (DWORD i = 0; i < dwFileCount; i++) {

		if (ModelTextureCheck(lpPckIndexTable->cFileIndex.szwFilename + nDetectOffset)) {
			if (!lpPckIndexTable->isInvalid) {

				lpPckIndexTable->isToDeDelete = TRUE;
				lpPckIndexTable->isInvalid = TRUE;

				m_EditedNode.push_back(lpPckIndexTable);
			}
		}
		lpPckIndexTable++;
	}
}