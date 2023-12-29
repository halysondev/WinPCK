//////////////////////////////////////////////////////////////////////
// ZupClass.h: used to parse the data in the zup file of Perfect World Company and display it in the List
// head File
//
// This program is written by Li Qiufeng/stsm/liqf
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2012.5.23
//////////////////////////////////////////////////////////////////////

#include "PckClass.h"
#include "ZupHeader.h"
#include "DictHash.h"
#include "PckClassLog.h"

#if !defined(_ZUPCLASS_H_)
#define _ZUPCLASS_H_

class CZupClass : public CPckClass
{
public:

	//CZupClass();
	CZupClass(LPPCK_RUNTIME_PARAMS inout);
	virtual ~CZupClass();

	BOOL	Init(LPCWSTR	szFile) override;

	const	LPPCKINDEXTABLE		GetPckIndexTable() override;

	//Rebuild pck file
	virtual BOOL	RebuildPckFile(LPCWSTR lpszScriptFile, LPCWSTR szRebuildPckFile, BOOL bUseRecompress) override { Logger.e(TEXT_NOTSUPPORT);return FALSE; }
	virtual BOOL	StripPck(LPCWSTR lpszStripedPckFile, int flag) override { Logger.e(TEXT_NOTSUPPORT); return FALSE; }

	//Create and update pck files
	virtual BOOL	UpdatePckFile(LPCWSTR szPckFile, const vector<wstring> &lpszFilePath, const PCK_PATH_NODE* lpNodeToInsert) override { Logger.e(TEXT_NOTSUPPORT);return FALSE; }

	//Rename file
	virtual BOOL	RenameFilename() override { Logger.e(TEXT_NOTSUPPORT);return FALSE; }

	//Delete a node
	virtual void	DeleteNode(LPPCK_PATH_NODE lpNode) override { Logger.e(TEXT_NOTSUPPORT);}
	virtual void	DeleteNode(LPPCKINDEXTABLE lpIndex) override { Logger.e(TEXT_NOTSUPPORT); }

	//Rename a node
	virtual	BOOL	RenameNode(LPPCK_PATH_NODE lpNode, const wchar_t* lpszReplaceString) override { Logger.e(TEXT_NOTSUPPORT); return FALSE; }
	virtual	BOOL	RenameIndex(LPPCK_PATH_NODE lpNode, const wchar_t* lpszReplaceString) override { Logger.e(TEXT_NOTSUPPORT); return FALSE; }
	virtual	BOOL	RenameIndex(LPPCKINDEXTABLE lpIndex, const wchar_t* lpszReplaceString) override { Logger.e(TEXT_NOTSUPPORT); return FALSE; }

	//Preview file
	virtual BOOL	GetSingleFileData(const PCKINDEXTABLE* const lpZupFileIndexTable, char *buffer, size_t sizeOfBuffer = 0) override;
protected:
	virtual BOOL	GetSingleFileData(LPVOID lpvoidFileRead, const PCKINDEXTABLE* const lpPckFileIndexTable, char *buffer, size_t sizeOfBuffer = 0) override;

private:

	LPPCKINDEXTABLE				m_lpZupIndexTable;
	CDictHash					*m_lpDictHash;

private:

	void	BuildDirTree();
	BOOL	BuildZupBaseDict();
	void	DecodeFilename(char *_dst, wchar_t *_wdst, char *_src);

	_inline void	DecodeDict(LPZUP_FILENAME_DICT lpZupDict);
	//void	AddDict(char *&lpszStringToAdd);
	void	AddDict(std::string& base_file);

	const PCKINDEXTABLE* GetBaseFileIndex(const PCKINDEXTABLE* lpIndex, const PCKINDEXTABLE* lpZeroBaseIndex);

};

#endif