#include "stdafx.h"
#include "HWFW_GUI.hpp"

INT_PTR CALLBACK DlgProc_About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
  case WM_INITDIALOG:
    return TRUE;

  case WM_COMMAND:
  {
    WORD wId = LOWORD(wParam), wNc = HIWORD(wParam);

    switch (wNc) {
    case STN_CLICKED:
      if (wId == IDC_STATIC_BLOG) {
        ShellExecute(0, 0, L"https://blog.csersoft.net", 0, 0, SW_SHOW);
      }
      break;
    }
  }
  break;

  case WM_CTLCOLORSTATIC:
    if ((HWND)lParam == GetDlgItem(hDlg, IDC_STATIC_BLOG))
    {
      SetBkMode((HDC)wParam, TRANSPARENT);
      SetTextColor((HDC)wParam, RGB(0, 0, 255));
      return (BOOL)GetSysColorBrush(COLOR_MENU);
    }
    break;

  case WM_CLOSE:
    EndDialog(hDlg, IDCLOSE);
    break;
  }

  return (INT_PTR)FALSE;
}
