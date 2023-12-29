//////////////////////////////////////////////////////////////////////
// PckClassMount.cpp: used to parse the data in the pck file of Perfect World Company and display it in the List
// Initialization of related classes, etc.
//
// This program is written by Li Qiufeng/stsm/liqf. The pck structure refers to Ruoshui's pck structure.txt, and
// Refer to the part of its Yi language code and read the pck file list
//
// This code is expected to be open source. Please retain the original author information for any modified release based on this code.
//
// 2015.5.27
//////////////////////////////////////////////////////////////////////
#include "PckClass.h"

BOOL CPckClass::MountPckFile(LPCWSTR	szFile)
{
	try {
		if (!DetectPckVerion(szFile))
			return FALSE;

		if (!ReadPckFileIndexes())
			return FALSE;
		//Set the entryType of the last Index to PCK_ENTRY_TYPE_TAIL_INDEX
		m_PckAllInfo.lpPckIndexTable[m_PckAllInfo.dwFileCount].entryType = PCK_ENTRY_TYPE_TAIL_INDEX;
		return TRUE;
	}
	catch (MyException e) {
		Logger.e(e.what());
		return FALSE;
	}
	
}

void CPckClass::BuildDirTree()
{
	//Convert all ansi text in the read index to Unicode
	GenerateUnicodeStringToIndex();
	//Create a directory tree based on the file names in index
	ParseIndexTableToNode(m_PckAllInfo.lpPckIndexTable);
}
