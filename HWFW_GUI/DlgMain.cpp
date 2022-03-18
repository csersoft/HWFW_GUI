#include "stdafx.h"
#include "DlgMain.h"

CHAR chIniPath[MAX_PATH] = { 0 };

// The message handler for the main window.
INT_PTR CALLBACK DlgProc_Main(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  static LONG lngTv_w, lngTv_h_diff,
    lngLv_w_diff, lngLv_h_diff,
    lngLbl_l, lngLbl_t_diff, lngLbl_w_diff, lngLbl_h;

  static WCHAR wsPath[MAX_PATH];

  switch (message)
  {
  case WM_INITDIALOG:
  {
    typedef BOOL(WINAPI * lpfnChangeWindowMessageFilter)(__in UINT message, __in DWORD dwFlag);

    RECT rcDlg, rcTmp, rcTmp2;
    HMODULE hModUser32 = GetModuleHandleA("user32.dll");
    LPCH lpStr;

    if (hModUser32)
    {
      lpfnChangeWindowMessageFilter fnChangeWindowMessageFilter = (lpfnChangeWindowMessageFilter)GetProcAddress(hModUser32, "ChangeWindowMessageFilter");

      if (fnChangeWindowMessageFilter)
      {
        fnChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
        fnChangeWindowMessageFilter(0x0049, MSGFLT_ADD);
      }
    }

    hMainDlg = hDlg;

    hmPop = LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU_ITEMINFO));

    if (hmPop)
    {
      hmItemInfo = GetSubMenu(hmPop, 0);
    }


    GetClientRect(hDlg, &rcDlg);
    GetWindowRect(GetDlgItem(hDlg, IDC_TV), &rcTmp);

    lngTv_w = rcTmp.right - rcTmp.left;
    lngTv_h_diff = rcDlg.bottom - (rcTmp.bottom - rcTmp.top);


    GetWindowRect(GetDlgItem(hDlg, IDC_LV), &rcTmp);

    lngLv_w_diff = rcDlg.right - (rcTmp.right - rcTmp.left);
    lngLv_h_diff = rcDlg.bottom - (rcTmp.bottom - rcTmp.top);


    GetWindowRect(GetDlgItem(hDlg, IDC_LBL_STATUS), &rcTmp);

    rcTmp2 = rcTmp;
    ScreenToClient(hDlg, &rcTmp2);

    lngLbl_l = rcTmp2.left;
    lngLbl_t_diff = rcDlg.bottom - rcTmp2.top;
    lngLbl_w_diff = rcDlg.right - (rcTmp.right - rcTmp.left);
    lngLbl_h = rcTmp.bottom - rcTmp.top;

    swprintf_s(wsPath, L"%s  %s  %s  Build:%s", APP_NAME, APP_VER1, APP_VER2, APP_BUILD_VER);
    SetWindowTextW(hDlg, wsPath);

    wcscpy_s(wsPath, L"firmware.bin");

    SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDI_HWFW_GUI)));
    SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDI_HWFW_GUI)));

    GetModuleFileNameA(NULL, chIniPath, MAX_PATH);
    lpStr = strrchr(chIniPath, '\\');

    if (lpStr)
    {
      lpStr++;
      strcpy_s(lpStr, MAX_PATH - (lpStr - chIniPath), "HWPDB.ini");
    }

    TreeView_SetProc(GetDlgItem(hDlg, IDC_TV));
  }
  return (INT_PTR)TRUE;

  case WM_COMMAND:
  {
    WORD wId = LOWORD(wParam), wNc = HIWORD(wParam);

    switch (wNc)
    {
    case 0:
    {
      switch (wId)
      {
      case IDM_OPEN:
      {
        CleanView();

        if (GetOpenFilePath(hDlg, wsPath, MAX_PATH))
        {
          OpenFirmware(wsPath);

          LPWSTR lpFileName = wcsrchr(wsPath, '\\');
          WCHAR wsTemp[MAX_PATH];

          if (lpFileName)
          {
            lpFileName++;

            swprintf_s(wsTemp, L"%s  %s  %s  Build:%s [%s]", APP_NAME, APP_VER1, APP_VER2, APP_BUILD_VER, lpFileName);
            SetWindowTextW(hDlg, wsTemp);
          }
        }
        else
          SetStatus(L"Failed to get file path!");
      }
      break;

      case IDM_SAVE:
      {
        int nResult = HWNP_Save();

        if (nResult == 0)
          SetStatus(L"The file is saved.");
        else
        {
          WCHAR wsTmp[128];

          swprintf_s(wsTmp, L"Failed to save the file, error code: [%d]!", nResult);
          SetStatus(wsTmp);
        }
      }
      break;

      case IDM_SAVEAS:
      {
        WCHAR wsTmp[MAX_PATH] = { 0 };

        if (GetSaveFilePath(hDlg, wsTmp, MAX_PATH))
        {
          int nResult = HWNP_SaveAs(wsTmp);

          if (nResult == 0)
            SetStatus(L"The file is saved.");
          else
          {
            swprintf_s(wsTmp, L"Failed to save the file, error code: [%d]!", nResult);
            SetStatus(wsTmp);
          }
        }
        else
          SetStatus(L"Failed to get file path!");
      }
      break;

      case IDM_EXIT:
        HWNP_Release();
        EndDialog(hDlg, 0);
        break;

      case IDM_ABOUT:
        DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUT), hDlg, &DlgProc_About);
        break;

      case IDM_CHKCRC:
      {
        uint32_t u32Result = HWNP_CheckCRC32();

        if (u32Result == CHKCRC_OK)
          SetStatus(L"CRC32 check passed.");
        else if (u32Result == CHKCRC_HEADERCRCERR)
          SetStatus(L"The header CRC32 check failed!");
        else if (u32Result == CHKCRC_FILECRCERR)
          SetStatus(L"File CRC32 check failed!");
        else if (u32Result >= CHKCRC_ITEMCRCERR)
        {
          WCHAR wsTmp[128];

          swprintf_s(wsTmp, L"Item check failed,Index:[%u]!", u32Result - CHKCRC_ITEMCRCERR);
          SetStatus(wsTmp);
        }
        else
          SetStatus(L"Unknown CRC32 check result!");
      }
      break;

      case IDM_EDIT_PL:
        if (HWNP_GetState() == -1) {
          DialogBox(hInst, MAKEINTRESOURCE(IDD_PROD), hDlg, &DlgProc_EditProduct);
        }
        break;

      case IDM_PII_ADD:
      {
        WCHAR wsTmp[MAX_PATH] = { 0 };

        if (GetOpenFilePath(hDlg, wsTmp, MAX_PATH))
        {
          DLGIIS dlgiis;

          dlgiis.dtType = DT_ADD;
          dlgiis.lpFile = wsTmp;
          dlgiis.u32Index = 0;

          DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_ITEMINFO), hDlg, &DlgProc_ItemInfo, (LPARAM)&dlgiis);

          CleanListView();
          ListItemInfo(GetDlgItem(hDlg, IDC_LV));
        }
      }
      break;

      case IDM_PII_EDIT:
      {
        int nItem = ListView_CurHotItem();

        if (nItem != -1)
        {
          DLGIIS dlgiis;

          dlgiis.dtType = DT_EDIT;
          dlgiis.lpFile = NULL;

          {
            LVITEMW lvi;

            lvi.mask = LVIF_PARAM;
            lvi.iItem = nItem;
            lvi.iSubItem = 0;

            ListView_GetItemW(GetDlgItem(hDlg, IDC_LV), &lvi);
            if (lvi.lParam) dlgiis.u32Index = (uint32_t)((PLVS)lvi.lParam)->dwUserData;
            else dlgiis.u32Index = 0;
          }

          DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_ITEMINFO), hDlg, &DlgProc_ItemInfo, (LPARAM)&dlgiis);

          UpdateItemInfo(GetDlgItem(hDlg, IDC_LV));
          ListView_SetItemState(GetDlgItem(hDlg, IDC_LV), nItem, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
        }
      }
      break;

      case IDM_PII_DELETE:
      {
        int nItem = ListView_CurHotItem();

        if (nItem != -1)
        {
          HWNP_DeleteItem((uint32_t)nItem);

          CleanListView();
          ListItemInfo(GetDlgItem(hDlg, IDC_LV));
        }
      }
      break;
      }

      return TRUE;
    }
    break;

    case 1:
      break;

    default:

      break;
    }

  }
  break;

  case WM_NOTIFY:
  {
    if (lParam == 0) break;

    LPNMHDR lpNm = (LPNMHDR)lParam;

    switch (lpNm->code)
    {
    case TVN_SELCHANGED:
    {
      TreeView_SelChanged((LPNMTREEVIEW)lParam);
    }
    break;

    case LVN_DELETEITEM:
    {
      LPNMLISTVIEW lpnmLV = (LPNMLISTVIEW)lParam;

      free((void *)lpnmLV->lParam);
    }
    break;

    case LVN_DELETEALLITEMS:
    {
      SetWindowLongPtr(hDlg, DWL_MSGRESULT, FALSE);
      return TRUE;
    }
    break;

    case NM_RCLICK:
    {
      if (lpNm->idFrom == IDC_LV)
      {
        ListView_RightClick((LPNMITEMACTIVATE)lParam);
      }
    }
    break;

    case NM_CUSTOMDRAW: 
    {
      if (wParam == IDC_TV)
      {
        LPNMTVCUSTOMDRAW lpCustomDraw = (LPNMTVCUSTOMDRAW)lParam;

        switch (lpCustomDraw->nmcd.dwDrawStage)
        {
        case CDDS_PREPAINT:
          SetWindowLongPtr(hDlg, DWLP_MSGRESULT, CDRF_NOTIFYITEMDRAW);
          return TRUE;
        case CDDS_ITEMPREPAINT:
          if (CHK_FLAGS(lpCustomDraw->nmcd.uItemState, CDIS_SELECTED)) {
            lpCustomDraw->clrText = RGB(255, 40, 40);
            lpCustomDraw->clrTextBk = RGB(192, 192, 192);
          }
          else {
            lpCustomDraw->clrText = RGB(20, 20, 20);
            lpCustomDraw->clrTextBk = RGB(242, 242, 242);
          }

          //SetWindowLongPtr(hDlg, DWLP_MSGRESULT, CDRF_SKIPDEFAULT);
          return TRUE;
        }
      }
    }
    break;
    }
  }
  break;

  case WM_CLOSE:
    HWNP_Release();
    EndDialog(hDlg, IDCLOSE);
    break;

  case WM_SIZING:
  {
    LPRECT lpRect = (LPRECT)lParam;

    if (!lpRect) return FALSE;

    if ((lpRect->right - lpRect->left) < 1024) lpRect->right = lpRect->left + 1024;
    if ((lpRect->bottom - lpRect->top) < 480) lpRect->bottom = lpRect->top + 480;
  }
  return (INT_PTR)TRUE;

  case WM_SIZE:

    if (wParam != SIZE_MINIMIZED)
    {
      int intWidth = LOWORD(lParam), intHeight = HIWORD(lParam);

      SetWindowPos(GetDlgItem(hDlg, IDC_TV), NULL, 0, 0, lngTv_w, intHeight - lngTv_h_diff, SWP_NOMOVE | SWP_NOZORDER);

      SetWindowPos(GetDlgItem(hDlg, IDC_LV), NULL,
        0, 0,
        intWidth - lngLv_w_diff, intHeight - lngLv_h_diff, SWP_NOMOVE | SWP_NOZORDER);

      SetWindowPos(GetDlgItem(hDlg, IDC_LBL_STATUS), NULL,
        lngLbl_l, intHeight - lngLbl_t_diff,
        intWidth - lngLbl_w_diff, lngLbl_h, SWP_NOZORDER);
    }

    break;
  }

  return (INT_PTR)FALSE;
}