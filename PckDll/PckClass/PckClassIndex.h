#pragma once
#include "PckClassBaseFeatures.h"
#include "PckClassCodepage.h"
#include "PckClassZlib.h"
#include "MapViewFileMultiPck.h"

class CPckClassIndex :
	public virtual CPckClassBaseFeatures
{
public:
	CPckClassIndex();
	~CPckClassIndex();

	//Apply for file index memory based on the number of files
	LPPCKINDEXTABLE		AllocPckIndexTableByFileCount();
	//Convert Ansi characters in lpPckIndexTable->cFileIndex.szFilename to Unicode and write them to lpPckIndexTable->cFileIndex.szwFilename
	void		GenerateUnicodeStringToIndex();
	// Reading of file header, tail and other structures
	BOOL		ReadPckFileIndexes();

protected:

	void*	AllocMemory(size_t	sizeStuct);
	//Recalculate the number of files during reconstruction and remove invalid and duplicate file names.
	DWORD	ReCountFiles();
	//Fill the modified index data into the structure by version and compress it
	LPPCKINDEXTABLE_COMPRESS FillAndCompressIndexData(LPPCKINDEXTABLE_COMPRESS lpPckIndexTableComped, LPPCKFILEINDEX lpPckFileIndexToCompress);

};

