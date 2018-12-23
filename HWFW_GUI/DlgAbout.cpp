#include "stdafx.h"
#include "HWFW_GUI.hpp"

INT_PTR CALLBACK DlgProc_About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
  case WM_INITDIALOG:
  return TRUE;

  case WM_CLOSE:
  EndDialog(hDlg, IDCLOSE);
  break;
  }

  return (INT_PTR)FALSE;
}
