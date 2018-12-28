#include "stdafx.h"
#include "HWFW_GUI.hpp"

#define MAX_LIST      4096

static HWND hEdit = NULL;
static CHAR chProdList[MAX_LIST];

static inline uint32_t alignList(uint32_t size) {
  if (size % 0x0100 == 0) return size;
  return (size / 0x0100 + 1) * 0x0100;
}

INT_PTR CALLBACK DlgProc_EditProduct(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message)
  {
  case WM_INITDIALOG:
    chProdList[0] = 0;
    HWNP_GetProductList(chProdList, MAX_LIST);

    hEdit = GetDlgItem(hDlg, IDC_EDIT_PROD);
    SendMessageA(hEdit, EM_SETLIMITTEXT, MAX_LIST - 2, 0);
    SetWindowTextA(hEdit, chProdList);
    return TRUE;
  case WM_COMMAND:
  {
    WORD wId = LOWORD(wParam), wNc = HIWORD(wParam);

    switch (wId) {
    case IDOK: 
      memset(chProdList, MAX_LIST, 0);

      GetWindowTextA(hEdit, chProdList, MAX_LIST);

      HWNP_SetProductList(chProdList, (uint16_t)alignList(strlen(chProdList)));
      EndDialog(hDlg, IDOK);
      break;
    case IDCANCEL:
      EndDialog(hDlg, IDCANCEL);
      break;
    }
  }
  break;
  case WM_CLOSE:
    EndDialog(hDlg, IDCLOSE);
    break;
  }


  return (INT_PTR)FALSE;
}