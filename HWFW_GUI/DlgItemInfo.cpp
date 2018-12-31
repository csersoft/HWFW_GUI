#include "stdafx.h"
#include "HWFW_GUI.hpp"

static PDLGIIS lpDlgIIS = NULL;
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

  if (lpDlgIIS == NULL) return;

  if (lpDlgIIS->dtType == DT_EDIT)
  {
    if ((HWNP_GetItemDataTypeByIndex(lpDlgIIS->u32Index, &dwType) == 0) && (CHK_FLAGS(dwType, IDT_WHWH)))
      EnableWindow(GetDlgItem(hDlg, IDC_BTN_ADF), TRUE);
    else
      EnableWindow(GetDlgItem(hDlg, IDC_BTN_ADF), FALSE);

  }
  else if (lpDlgIIS->dtType == DT_ADD)
  {
    EnableWindow(GetDlgItem(hDlg, IDC_BTN_ADF), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDC_BTN_EXP), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDC_BTN_IMP), FALSE);
  }

  swprintf_s(wsTemp, SF_DEC, hwItemInfo.u32Id);
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_ID), wsTemp);
  
  if (lpDlgIIS->dtType == DT_EDIT)
  {
    swprintf_s(wsTemp, SF_HEX, hwItemInfo.u32ItemCRC32);
    SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_CRC), wsTemp);

    swprintf_s(wsTemp, SF_HEX, hwItemInfo.u32Offset);
    SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_OFFSET), wsTemp);

    swprintf_s(wsTemp, SF_HEX, hwItemInfo.u32Size);
    SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_SIZE), wsTemp);
  }
  else if (lpDlgIIS->dtType == DT_ADD)
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
    if (lpDlgIIS != NULL) return (INT_PTR)FALSE;
    if (lParam == NULL) return (INT_PTR)FALSE;

    int nResult;
    WCHAR wsTemp[260];

    lpDlgIIS = (PDLGIIS)lParam;

    SNDMSG(GetDlgItem(hDlg, IDC_EDIT_PATH), EM_SETLIMITTEXT, sizeof(HWNP_ITEMINFO::chItemPath), 0);
    ComboBox_LimitText(GetDlgItem(hDlg, IDC_CB_TYPE), sizeof(HWNP_ITEMINFO::chItemType));
    SNDMSG(GetDlgItem(hDlg, IDC_EDIT_VER), EM_SETLIMITTEXT, sizeof(HWNP_ITEMINFO::chItemVersion), 0);

    ComboBox_AddStringW(GetDlgItem(hDlg, IDC_CB_TYPE), L"UPGRDCHECK");
    ComboBox_AddStringW(GetDlgItem(hDlg, IDC_CB_TYPE), L"MODULE");
    ComboBox_AddStringW(GetDlgItem(hDlg, IDC_CB_TYPE), L"FLASH_CONFIG");
    ComboBox_AddStringW(GetDlgItem(hDlg, IDC_CB_TYPE), L"UBOOT");
    ComboBox_AddStringW(GetDlgItem(hDlg, IDC_CB_TYPE), L"MINISYS");
    ComboBox_AddStringW(GetDlgItem(hDlg, IDC_CB_TYPE), L"KERNEL");
    ComboBox_AddStringW(GetDlgItem(hDlg, IDC_CB_TYPE), L"ROOTFS");
    ComboBox_AddStringW(GetDlgItem(hDlg, IDC_CB_TYPE), L"UPDATEFLAG");
    ComboBox_AddStringW(GetDlgItem(hDlg, IDC_CB_TYPE), L"EFS");
    ComboBox_AddStringW(GetDlgItem(hDlg, IDC_CB_TYPE), L"UNKNOWN");

    if (lpDlgIIS->dtType == DT_EDIT)
    {
      nResult = HWNP_GetItemInfoByIndex(lpDlgIIS->u32Index, &hwItemInfo);

      if (nResult != 0)
      {
        swprintf_s(wsTemp, L"获取项目信息失败!错误码:[%d]", nResult);
        SetWindowTextW(GetDlgItem(hDlg, IDC_LBL_II_STATUS), wsTemp);
        return (INT_PTR)TRUE;
      }

      UpdateView(hDlg);
    }
    else if (lpDlgIIS->dtType == DT_ADD)
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
      if (lpDlgIIS->dtType == DT_EDIT)
      {
        int nResult;
        uint32_t u32DataSize;
        LPCVOID lpData;
        WCHAR wsTmp[MAX_PATH] = { 0 };

        if (GetSaveFilePath(hDlg, wsTmp, MAX_PATH))
        {
          nResult = HWNP_GetItemDataSizeByIndex(lpDlgIIS->u32Index, &u32DataSize);
          if (nResult != 0)
          {
            SetTooltip(GetDlgItem(hDlg, IDC_LBL_II_STATUS), L"获取项目数据大小失败,错误码:[%d]!", nResult);
            break;
          }

          nResult = HWNP_GetItemDataPointerByIndex(lpDlgIIS->u32Index, &lpData);
          if (nResult != 0)
          {
            SetTooltip(GetDlgItem(hDlg, IDC_LBL_II_STATUS), L"获取项目数据失败,错误码:[%d]!", nResult);
            break;
          }

          if (ExportToFile(wsTmp, lpData, u32DataSize))
            SetTooltip(GetDlgItem(hDlg, IDC_LBL_II_STATUS), L"导出项目数据完成.");
          else
            SetTooltip(GetDlgItem(hDlg, IDC_LBL_II_STATUS), L"导出项目数据失败,错误码:[%d]!", GetLastError());
        }
      }
      break;

      case IDC_BTN_IMP:
      if (lpDlgIIS->dtType == DT_EDIT)
      {
        int nResult;
        DWORD dwDataSize;
        LPVOID lpData;
        WCHAR wsTmp[MAX_PATH] = { 0 };

        if (GetOpenFilePath(hDlg, wsTmp, MAX_PATH))
        {
          if (ImportFromFile(wsTmp, &lpData, &dwDataSize) == FALSE)
          {
            SetTooltip(GetDlgItem(hDlg, IDC_LBL_II_STATUS), L"打开文件失败,错误码:[%d]!", GetLastError());
            break;
          }

          nResult = HWNP_SetItemData(lpDlgIIS->u32Index, lpData, (uint32_t)dwDataSize);
          free(lpData);

          if (nResult != 0)
            SetTooltip(GetDlgItem(hDlg, IDC_LBL_II_STATUS), L"导入数据失败,错误码:[%d]!", nResult);
          else
            SetTooltip(GetDlgItem(hDlg, IDC_LBL_II_STATUS), L"导入数据完成.");
          
        }
      }
      break;

      case IDC_BTN_ADF:
      if (lpDlgIIS->dtType == DT_EDIT)
      {
        SaveItemInfo(hDlg);
        DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_ADVFMTDLG), hDlg, &DlgProc_AdvDatFmt, (LPARAM)lpDlgIIS->u32Index);
        UpdateView(hDlg);
      }
      break;

      case IDOK:
      {
        int nResult = 0;
        
        SaveItemInfo(hDlg);

        if (lpDlgIIS->dtType == DT_EDIT)
        {
          nResult = HWNP_SetItemInfo(lpDlgIIS->u32Index, IIFLAG_ALL, hwItemInfo.u32Id, hwItemInfo.chItemPath, hwItemInfo.chItemType,
            hwItemInfo.chItemVersion, hwItemInfo.u32Policy, hwItemInfo.u32Reserved);
        }
        else if (lpDlgIIS->dtType == DT_ADD)
        {
          DWORD dwSize;
          LPVOID lpData;

          if (ImportFromFile(lpDlgIIS->lpFile, &lpData, &dwSize) == FALSE)
          {
            SetTooltip(GetDlgItem(hDlg, IDC_LBL_II_STATUS), L"导入文件失败!");
            break;
          }

          nResult = HWNP_AddItem(hwItemInfo.u32Id, lpData, dwSize, hwItemInfo.chItemPath, hwItemInfo.chItemType,
            hwItemInfo.chItemVersion, hwItemInfo.u32Policy, hwItemInfo.u32Reserved);

          free(lpData);
        }

        if (nResult == 0)
        {
          HWNP_Update();
          EndDialog(hDlg, IDOK);
        }
        else
        {
          SetTooltip(GetDlgItem(hDlg, IDC_LBL_II_STATUS), L"保存项目信息失败,错误码:[%d]!", nResult);
        }
      }
      break;

      case IDCANCEL:
      lpDlgIIS = NULL;
      EndDialog(hDlg, IDCANCEL);
      break;
      }

    }
  }
  break;

  case WM_CLOSE:
  lpDlgIIS = NULL;
  EndDialog(hDlg, IDCLOSE);
  break;
  }

  return (INT_PTR)FALSE;
}
