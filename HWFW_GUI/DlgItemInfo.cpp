#include "stdafx.h"
#include "HWFW_GUI.hpp"

static PDLGIIS lpDlgParam = NULL;
static HWNP_ITEMINFO hwItemInfo;

static void SaveItemInfo(HWND hDlg)
{
  CHAR chTmp[MAX_PATH] = { 0 };

  GetWindowTextA(GetDlgItem(hDlg, IDC_EDIT_ID), chTmp, MAX_PATH);
  if (strlen(chTmp) != 0) hwItemInfo.u32Id = ScanfDec(chTmp);

  GetWindowTextA(GetDlgItem(hDlg, IDC_EDIT_PATH), chTmp, MAX_PATH);
  memset(hwItemInfo.chItemPath, 0, sizeof(HWNP_ITEMINFO::chItemPath));
  if (strlen(chTmp) != 0) strcpy_s(hwItemInfo.chItemPath, chTmp);

  GetWindowTextA(GetDlgItem(hDlg, IDC_CB_TYPE), chTmp, MAX_PATH);
  memset(hwItemInfo.chItemType, 0, sizeof(HWNP_ITEMINFO::chItemType));
  if (strlen(chTmp) != 0) strcpy_s(hwItemInfo.chItemType, chTmp);

  GetWindowTextA(GetDlgItem(hDlg, IDC_EDIT_VER), chTmp, MAX_PATH);
  memset(hwItemInfo.chItemVersion, 0, sizeof(HWNP_ITEMINFO::chItemVersion));
  if (strlen(chTmp) != 0) strcpy_s(hwItemInfo.chItemVersion, chTmp);

  GetWindowTextA(GetDlgItem(hDlg, IDC_EDIT_POLICY), chTmp, MAX_PATH);
  if (strlen(chTmp) != 0) hwItemInfo.u32Policy = ScanfHex(chTmp);

  GetWindowTextA(GetDlgItem(hDlg, IDC_EDIT_UNKDAT), chTmp, MAX_PATH);
  if (strlen(chTmp) != 0) hwItemInfo.u32Reserved = ScanfHex(chTmp);
}

static void UpdateView(HWND hDlg)
{
  DWORD dwType;
  size_t stOut;
  WCHAR wsTemp[260];

  if (lpDlgParam == NULL) return;

  if (lpDlgParam->dtType == DT_EDIT)
  {
    if ((HWNP_GetItemDataTypeByIndex(lpDlgParam->u32Index, &dwType) == 0) && (CHK_FLAGS(dwType, IDT_WHWH)))
      EnableWindow(GetDlgItem(hDlg, IDC_BTN_ADF), TRUE);
    else
      EnableWindow(GetDlgItem(hDlg, IDC_BTN_ADF), FALSE);

  }
  else if (lpDlgParam->dtType == DT_ADD)
  {
    EnableWindow(GetDlgItem(hDlg, IDC_BTN_ADF), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDC_BTN_EXP), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDC_BTN_IMP), FALSE);
  }

  swprintf_s(wsTemp, SF_DEC, hwItemInfo.u32Id);
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_ID), wsTemp);

  if (lpDlgParam->dtType == DT_EDIT)
  {
    swprintf_s(wsTemp, SF_HEX, hwItemInfo.u32ItemCRC32);
    SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_CRC), wsTemp);

    swprintf_s(wsTemp, SF_HEX, hwItemInfo.u32Offset);
    SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_OFFSET), wsTemp);

    swprintf_s(wsTemp, SF_HEX, hwItemInfo.u32Size);
    SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_SIZE), wsTemp);
  }
  else if (lpDlgParam->dtType == DT_ADD)
  {
    SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_CRC), L"N/A");
    SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_OFFSET), L"N/A");
    SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_SIZE), L"N/A");
  }

  mbstowcs_s(&stOut, wsTemp, hwItemInfo.chItemPath, sizeof(HWNP_ITEMINFO::chItemPath));
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_PATH), wsTemp);

  mbstowcs_s(&stOut, wsTemp, hwItemInfo.chItemType, sizeof(HWNP_ITEMINFO::chItemType));
  SetWindowTextW(GetDlgItem(hDlg, IDC_CB_TYPE), wsTemp);

  mbstowcs_s(&stOut, wsTemp, hwItemInfo.chItemVersion, sizeof(HWNP_ITEMINFO::chItemVersion));
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_VER), wsTemp);

  swprintf_s(wsTemp, SF_HEX, hwItemInfo.u32Policy);
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_POLICY), wsTemp);

  swprintf_s(wsTemp, SF_HEX, hwItemInfo.u32Reserved);
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_UNKDAT), wsTemp);

}

INT_PTR CALLBACK DlgProc_ItemInfo(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
  case WM_INITDIALOG:
  {
    if (lpDlgParam != NULL) return (INT_PTR)FALSE;
    if (lParam == NULL) return (INT_PTR)FALSE;

    int nResult;
    WCHAR wsTemp[260];

    lpDlgParam = (PDLGIIS)lParam;

    SNDMSG(GetDlgItem(hDlg, IDC_EDIT_PATH), EM_SETLIMITTEXT, sizeof(HWNP_ITEMINFO::chItemPath), 0);
    ComboBox_LimitText(GetDlgItem(hDlg, IDC_CB_TYPE), sizeof(HWNP_ITEMINFO::chItemType));
    SNDMSG(GetDlgItem(hDlg, IDC_EDIT_VER), EM_SETLIMITTEXT, sizeof(HWNP_ITEMINFO::chItemVersion), 0);

    for (int i = 0; true; i++) {
      if (HW_ItemType_Text[i] == NULL) break;

      ComboBox_AddStringW(GetDlgItem(hDlg, IDC_CB_TYPE), HW_ItemType_Text[i]);
    }

    if (lpDlgParam->dtType == DT_EDIT)
    {
      nResult = HWNP_GetItemInfoByIndex(lpDlgParam->u32Index, &hwItemInfo);

      if (nResult != 0)
      {
        swprintf_s(wsTemp, L"Failed to get project information! Error code: [%d]", nResult);
        SetWindowTextW(GetDlgItem(hDlg, IDC_LBL_II_STATUS), wsTemp);
        return (INT_PTR)TRUE;
      }

      UpdateView(hDlg);
    }
    else if (lpDlgParam->dtType == DT_ADD)
    {
      hwItemInfo.u32Id = HWNP_GetLastItemId();

      strcpy_s(hwItemInfo.chItemPath, "");
      strcpy_s(hwItemInfo.chItemType, "UNKNOWN");
      strcpy_s(hwItemInfo.chItemVersion, "");
      hwItemInfo.u32Policy = 0;
      hwItemInfo.u32Reserved = 0;
      swprintf_s(wsTemp, SF_HEX, hwItemInfo.u32Reserved);

      UpdateView(hDlg);
    }

    return (INT_PTR)TRUE;
  }
  break;

  case WM_SHOWWINDOW:
  {
    if ((wParam) && (lParam == SW_PARENTOPENING)) UpdateView(hDlg);
  }
  break;

  case WM_COMMAND:
  {
    WORD wId = LOWORD(wParam), wNc = HIWORD(wParam);

    if (lParam != 0)
    {
      switch (wId)
      {
      case IDC_BTN_EXP:
        if (lpDlgParam->dtType == DT_EDIT)
        {
          int nResult;
          uint32_t u32DataSize;
          LPCVOID lpData;
          WCHAR wsTmp[MAX_PATH] = { 0 };

          if (GetSaveFilePath(hDlg, wsTmp, MAX_PATH))
          {
            nResult = HWNP_GetItemDataSizeByIndex(lpDlgParam->u32Index, &u32DataSize);
            if (nResult != 0)
            {
              SetTooltip(GetDlgItem(hDlg, IDC_LBL_II_STATUS), L"Failed to get item data size, error code: [%d]!", nResult);
              break;
            }

            nResult = HWNP_GetItemDataPointerByIndex(lpDlgParam->u32Index, &lpData);
            if (nResult != 0)
            {
              SetTooltip(GetDlgItem(hDlg, IDC_LBL_II_STATUS), L"Failed to get item data, error code: [%d]!", nResult);
              break;
            }

            if (ExportToFile(wsTmp, lpData, u32DataSize))
              SetTooltip(GetDlgItem(hDlg, IDC_LBL_II_STATUS), L"Export item data completed.");
            else
              SetTooltip(GetDlgItem(hDlg, IDC_LBL_II_STATUS), L"Failed to export item data, error code: [%d]!", GetLastError());
          }
        }
        break;

      case IDC_BTN_IMP:
        if (lpDlgParam->dtType == DT_EDIT)
        {
          int nResult;
          DWORD dwDataSize;
          LPVOID lpData;
          WCHAR wsTmp[MAX_PATH] = { 0 };

          if (GetOpenFilePath(hDlg, wsTmp, MAX_PATH))
          {
            if (ImportFromFile(wsTmp, &lpData, &dwDataSize) == FALSE)
            {
              SetTooltip(GetDlgItem(hDlg, IDC_LBL_II_STATUS), L"Failed to open file, error code: [%d]!", GetLastError());
              break;
            }

            nResult = HWNP_SetItemData(lpDlgParam->u32Index, lpData, (uint32_t)dwDataSize);
            free(lpData);

            if (nResult != 0)
              SetTooltip(GetDlgItem(hDlg, IDC_LBL_II_STATUS), L"Failed to import data, error code: [%d]!", nResult);
            else
              SetTooltip(GetDlgItem(hDlg, IDC_LBL_II_STATUS), L"Import data completed.");

          }
        }
        break;

      case IDC_BTN_ADF:
        if (lpDlgParam->dtType == DT_EDIT)
        {
          SaveItemInfo(hDlg);
          DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_ADVFMTDLG), hDlg, &DlgProc_AdvDatFmt, (LPARAM)lpDlgParam->u32Index);
          UpdateView(hDlg);
        }
        break;

      case IDOK:
      {
        int nResult = 0;

        SaveItemInfo(hDlg);

        if (lpDlgParam->dtType == DT_EDIT)
        {
          nResult = HWNP_SetItemInfo(lpDlgParam->u32Index, IIFLAG_ALL, hwItemInfo.u32Id, hwItemInfo.chItemPath, hwItemInfo.chItemType,
            hwItemInfo.chItemVersion, hwItemInfo.u32Policy, hwItemInfo.u32Reserved);
        }
        else if (lpDlgParam->dtType == DT_ADD)
        {
          DWORD dwSize;
          LPVOID lpData;

          if (ImportFromFile(lpDlgParam->lpFile, &lpData, &dwSize) == FALSE)
          {
            SetTooltip(GetDlgItem(hDlg, IDC_LBL_II_STATUS), L"Import file failed!");
            break;
          }

          nResult = HWNP_AddItem(hwItemInfo.u32Id, lpData, dwSize, hwItemInfo.chItemPath, hwItemInfo.chItemType,
            hwItemInfo.chItemVersion, hwItemInfo.u32Policy, hwItemInfo.u32Reserved);

          free(lpData);
        }

        if (nResult == 0)
        {
          HWNP_Update();
          //lpDlgIIS = NULL;
          EndDialog(hDlg, IDOK);
        }
        else
        {
          SetTooltip(GetDlgItem(hDlg, IDC_LBL_II_STATUS), L"Failed to save project information, error code: [%d]!", nResult);
        }
      }
      break;

      case IDCANCEL:
        //lpDlgIIS = NULL;
        EndDialog(hDlg, IDCANCEL);
        break;
      }

    }

    return TRUE;
  }
  break;

  case WM_CLOSE:
    //lpDlgIIS = NULL;
    EndDialog(hDlg, IDCLOSE);
    break;

  case WM_DESTROY:
    lpDlgParam = NULL;
    break;
  }

  return (INT_PTR)FALSE;
}
