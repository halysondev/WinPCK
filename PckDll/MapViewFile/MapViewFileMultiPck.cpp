#include "MapViewFileMultiPck.h"


CMapViewFileMultiPck::CMapViewFileMultiPck()
{}

CMapViewFileMultiPck::~CMapViewFileMultiPck()
{}

void CMapViewFileMultiPck::GetPkxPath(LPSTR lpszDest, LPCSTR lpszBaseName, UINT uiNum)
{
    if (uiNum == 0) {
        sprintf(lpszDest, "%s.pkx", lpszBaseName);
        return;
    }

    sprintf(lpszDest, "%s.pkx%i", lpszBaseName, uiNum);
}

void CMapViewFileMultiPck::GetPkxPath(LPWSTR lpszDest, LPCWSTR lpszBaseName, UINT uiNum)
{
    if (uiNum == 0) {
        swprintf(lpszDest, MAX_PATH, L"%ls.pkx", lpszBaseName);
        return;
    }

    swprintf(lpszDest, MAX_PATH, L"%ls.pkx%i", lpszBaseName, uiNum);
}
