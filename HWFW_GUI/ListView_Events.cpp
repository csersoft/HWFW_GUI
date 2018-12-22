#include "stdafx.h"
#include "DlgMain.h"
#include "HWFW_GUI.hpp"

static WNDPROC wfnLV = NULL;
static int nLastItem = -1;

static LRESULT CALLBACK ListView_WndProc(HWND hCtrl, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  if (!wfnLV)
    return DefWindowProc(hCtrl, Msg, wParam, lParam);

  if (Msg == WM_DROPFILES)
  {
    WCHAR wsFile[MAX_PATH];

    if (DragQueryFile((HDROP)wParam, 0, wsFile, MAX_PATH))
      OpenFirmware(wsFile);

    DragFinish((HDROP)wParam);
    return 0;
  }

  return CallWindowProc(wfnLV, hCtrl, Msg, wParam, lParam);
}

void ListView_SetProc(HWND hCtrl)
{
  wfnLV = (WNDPROC)SetWindowLongPtr(hCtrl, GWL_WNDPROC, (LONG)&ListView_WndProc);
}

void ListView_RightClick(LPNMITEMACTIVATE lpnmItemActivate)
{
  if (lpnmItemActivate->iItem == -1) return;
  //if (lpnmItemActivate->lParam == NULL) return;

  LVITEMW lvi;
  PLVS lpLVS;
  POINT pt;

  nLastItem = lpnmItemActivate->iItem;
  //ListView_SetHotItem(lpnmItemActivate->hdr.hwndFrom, lpnmItemActivate->iItem);

  lvi.mask = LVIF_PARAM;
  lvi.iItem = lpnmItemActivate->iItem;
  lvi.iSubItem = lpnmItemActivate->iSubItem;

  ListView_GetItem(lpnmItemActivate->hdr.hwndFrom, &lvi);
  lpLVS = (PLVS)lvi.lParam;

  if (lpLVS == NULL) return;
  
  switch (lpLVS->ltType)
  {
  case LT_ITEMINFO:
  GetCursorPos(&pt);
  TrackPopupMenu(hmItemInfo, TPM_LEFTALIGN, pt.x, pt.y, 0, hMainDlg, NULL);
  break;
  }

}

int ListView_AddColumn(HWND hCtrl, int nWidth, int nIndex, LPWSTR lpText)
{
  LVCOLUMNW lvc;

  ZeroMemory(&lvc, sizeof(LVCOLUMNW));
  lvc.mask = LVCF_TEXT | LVCF_WIDTH;

  lvc.pszText = lpText;
  lvc.cx = nWidth;
  return ListView_InsertColumnW(hCtrl, nIndex, &lvc);
}

int ListView_AddItemA(HWND hCtrl, int iItem, int iSubItem, LPSTR lpText, LVI_TYPE ltType, DWORD dwFlags, DWORD dwUserData, LPARAM lParam)
{
  LVITEMA lvi;
  int nResult;

  ZeroMemory(&lvi, sizeof(LVITEMA));

  lvi.iItem = iItem;
  lvi.iSubItem = iSubItem;
  lvi.pszText = lpText;

  if (iSubItem == 0)
  {
    PLVS lpLVS = (PLVS)calloc(1, sizeof(LVS));

    if (!lpLVS) return -1;

    lpLVS->ltType = ltType;
    lpLVS->dwFlags = dwFlags;
    lpLVS->dwUserData = dwUserData;
    lpLVS->lParam = lParam;

    lvi.lParam = (LPARAM)lpLVS;
    lvi.mask = LVIF_TEXT | LVIF_PARAM;
    nResult = ListView_InsertItemA(hCtrl, &lvi);
  }
  else
  {
    lvi.mask = LVIF_TEXT;
    nResult = ListView_SetItemA(hCtrl, &lvi);
  }

  return nResult;
}

int ListView_AddItemW(HWND hCtrl, int iItem, int iSubItem, LPWSTR lpText, LVI_TYPE ltType, DWORD dwFlags, DWORD dwUserData, LPARAM lParam)
{
  LVITEMW lvi;
  int nResult;

  ZeroMemory(&lvi, sizeof(LVITEMW));

  lvi.iItem = iItem;
  lvi.iSubItem = iSubItem;
  lvi.pszText = lpText;
  
  if (iSubItem == 0)
  {
    PLVS lpLVS = (PLVS)calloc(1, sizeof(LVS));

    if (!lpLVS) return -1;

    lpLVS->ltType = ltType;
    lpLVS->dwFlags = dwFlags;
    lpLVS->dwUserData = dwUserData;
    lpLVS->lParam = lParam;

    lvi.lParam = (LPARAM)lpLVS;
    lvi.mask = LVIF_TEXT | LVIF_PARAM;
    nResult = ListView_InsertItemW(hCtrl, &lvi);
  }
  else
  {
    lvi.mask = LVIF_TEXT;
    nResult = ListView_SetItemW(hCtrl, &lvi);
  }

  return nResult;
}

int ListView_CurHotItem()
{
  return nLastItem;
}