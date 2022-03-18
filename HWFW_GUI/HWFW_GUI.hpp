#pragma once

#ifndef _WINDOWS_
#include <Windows.h>
#endif

#ifndef _INC_WINDOWSX
#include <WindowsX.h>
#endif

#include "resource.h"
#include "HWNP.h"

extern HINSTANCE hInst;
extern HWND hMainDlg;
extern HMENU hmPop;
extern HMENU hmItemInfo;

INT_PTR CALLBACK    DlgProc_Main(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DlgProc_ItemInfo(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DlgProc_AdvDatFmt(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DlgProc_EditProduct(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DlgProc_About(HWND, UINT, WPARAM, LPARAM);

#define STR_FILE_TYPE       _T("All files(*.*)\0*.*\0\0")

enum TVI_TYPE : unsigned int
{
  TT_NONE,
  TT_FILEHDR,               //file header
  TT_MODELINFO,             //Model info (product list)
  TT_ITEMINFO,              //item info
  TT_RAWDATA
};

enum LVI_TYPE : unsigned int
{
  LT_NONE,
  LT_MODELINFO,
  LT_ITEMINFO
};

enum DLGII_TYPE : unsigned int
{
  DT_NONE,
  DT_ADD,
  DT_EDIT
};

typedef struct _LISTVIEW_STRUCT
{
  LVI_TYPE            ltType;
  DWORD               dwFlags;
  DWORD               dwUserData;
  LPARAM              lParam;
} LVS, *PLVS;

typedef struct _DIALOG_ITEMINFO_STRUCT
{
  DLGII_TYPE          dtType;
  uint32_t            u32Index;
  PWCHAR              lpFile;
} DLGIIS, *PDLGIIS;

void SetStatus(LPCWSTR lpText, ...);
void SetTooltip(HWND hCtrl, LPCWSTR lpText, ...);

BOOL QueryMsg(HWND hOwner, LPCWSTR szText, LPCWSTR szTitle);

BOOL GetOpenFilePath(HWND hOwner, LPWSTR lpFilePath, DWORD dwMax);
BOOL GetSaveFilePath(HWND hOwner, LPWSTR lpFilePath, DWORD dwMax);

BOOL ExportToFile(LPCWSTR lpFile, LPCVOID lpData, DWORD dwSize);
BOOL ImportFromFile(LPCWSTR lpFile, LPVOID *lppData, DWORD *lpdwSize);

void CleanListView();
void CleanView();
void OpenFirmware(LPCWSTR lpPath);

uint32_t MemAlignCpy(LPVOID const dest, size_t const destSize, LPCVOID const src, size_t const srcSize, int fill);

static inline BOOL ScreenToClient(__in HWND hWnd, __inout LPRECT lpRect)
{
  ScreenToClient(hWnd, (LPPOINT)&(lpRect->left));
  return ScreenToClient(hWnd, (LPPOINT)&(lpRect->right));
}

static inline DWORD ScanfDec(LPCCH lpString)
{
  if (lpString == NULL) return 0;

  DWORD dwDec;

  sscanf_s(lpString, "%u", &dwDec);

  return dwDec;
}

static inline DWORD ScanfHex(LPCCH lpString)
{
  if (lpString == NULL) return 0;

  DWORD dwHex;

  if (_strnicmp(lpString, "0x", 2) == 0)
  {
    sscanf_s(lpString, "0x%X", &dwHex);
  }
  else
  {
    sscanf_s(lpString, "%X", &dwHex);
  }

  return dwHex;
}

/************************************************************************/
/* page alignment */
/************************************************************************/
static inline uint32_t alignPage(uint32_t val) {
  if (val % 0x1000 == 0) return val;
  return (val / 0x1000 + 1) * 0x1000;
}

static inline int alignPage(int val) {
  if (val % 0x1000 == 0) return val;
  return (val / 0x1000 + 1) * 0x1000;
}
