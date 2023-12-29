#pragma once
#include "PckClassIndex.h"

#define INVALID_NODE	( -1 )

class CPckClassNode :
	protected virtual CPckClassIndex
{
public:
	CPckClassNode();
	~CPckClassNode();

	//Perform path analysis on the PckIndex file and put it into Node
	void			ParseIndexTableToNode(LPPCKINDEXTABLE lpMainIndexTable);
	//Find identical nodes
	const PCK_PATH_NODE*	FindFileNode(const PCK_PATH_NODE* lpBaseNode, wchar_t* lpszFile);

	//Delete a node
	virtual VOID	DeleteNode(LPPCK_PATH_NODE lpNode);
	virtual VOID	DeleteNode(LPPCKINDEXTABLE lpIndex);

protected:
	BOOL	RenameNodeEnum(LPPCK_PATH_NODE lpNode, size_t lenNodeRes, char* lpszReplaceString, size_t lenrs, size_t lenrp);
	BOOL	RenameNode(LPPCK_PATH_NODE lpNode, size_t lenNodeRes, char* lpszReplaceString, size_t lenrs, size_t lenrp);

public:
	//Rename a node
	virtual	BOOL	RenameNode(LPPCK_PATH_NODE lpNode, const wchar_t* lpszReplaceString);
	virtual	BOOL	RenameIndex(LPPCK_PATH_NODE lpNode, const wchar_t* lpszReplaceString);
	virtual	BOOL	RenameIndex(LPPCKINDEXTABLE lpIndex, const wchar_t* lpszReplaceString);

protected:
	//Check new file name
	//BOOL			CheckNewFilename();

public:
	//Get node path
	static BOOL	GetCurrentNodeString(wchar_t *szCurrentNodePathString, const PCK_PATH_NODE* lpNode);
protected:
	BOOL	FindDuplicateNodeFromFileList(const PCK_PATH_NODE* lpNodeToInsertPtr, DWORD &_in_out_FileCount);

private:

	LPPCK_PATH_NODE		m_lpRootNode;		//The root node of the PCK file node

	//Perform path analysis on the PckIndex file and put it into Node
	BOOL	AddFileToNode(LPPCKINDEXTABLE	lpPckIndexNode);

};

