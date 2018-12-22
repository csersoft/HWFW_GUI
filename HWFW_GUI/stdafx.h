// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <Windows.h>
#include <WindowsX.h>
#include <CommDlg.h>
#include <CommCtrl.h>
#include <ShellAPI.h>

// C 运行时头文件
#include <stdint.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <time.h>

#ifdef NDEBUG
#undef NDEBUG
#include <assert.h>
#define NDEBUG
#else
#include <assert.h>
#endif


#define APP_NAME                L"HWFW_GUI"
#define APP_VER1                L"1.0.7"
#define APP_VER2                L"Alpha"
#define APP_BUILD_VER           L"001"


#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32'   name='Microsoft.Windows.Common-Controls'   version='6.0.0.0'   processorArchitecture='x86'   publicKeyToken='6595b64144ccf1df'   language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32'   name='Microsoft.Windows.Common-Controls'   version='6.0.0.0'   processorArchitecture='*'   publicKeyToken='6595b64144ccf1df'   language='*'\"")
#endif

#define LVS_EX_DEFAULT            (LVS_EX_FULLROWSELECT)

#define TreeView_DlgInsertItem(hwnd, item, lpis) \
  (HTREEITEM)::SendDlgItemMessage((hwnd), (int)(item), TVM_INSERTITEM, 0, (LPARAM)(lpis))

#define TreeView_DlgInsertItemA(hwnd, item, lpis) \
  (HTREEITEM)::SendDlgItemMessageA((hwnd), (int)(item), TVM_INSERTITEMA, 0, (LPARAM)(lpis))

#define TreeView_DlgInsertItemW(hwnd, item, lpis) \
  (HTREEITEM)::SendDlgItemMessageW((hwnd), (int)(item), TVM_INSERTITEMW, 0, (LPARAM)(lpis))

#define ListView_InsertColumnW(hwnd, iCol, pcol) \
  (int)::SendMessageW((hwnd), LVM_INSERTCOLUMNW, (WPARAM)(iCol), (LPARAM)(pcol))

#define TreeView_DlgSetItemA(hwnd, item, pitem) \
  (BOOL)::SendDlgItemMessageA((hwnd), (int)(item), TVM_SETITEMA, 0, (LPARAM)(const TVITEMA *)(pitem))

#define TreeView_DlgSetItemW(hwnd, item, pitem) \
  (BOOL)::SendDlgItemMessageW((hwnd), (int)(item), TVM_SETITEMW, 0, (LPARAM)(const TVITEMW *)(pitem))

#define ListView_InsertItemA(hwnd, pitem)   \
  (int)::SendMessageA((hwnd), LVM_INSERTITEMA, 0, (LPARAM)(pitem))

#define ListView_InsertItemW(hwnd, pitem)   \
  (int)::SendMessageW((hwnd), LVM_INSERTITEMW, 0, (LPARAM)(pitem))

#define ListView_SetItemA(hwnd, pitem) \
  (BOOL)::SendMessageA((hwnd), LVM_SETITEMA, 0, (LPARAM)(pitem))

#define ListView_SetItemW(hwnd, pitem) \
  (BOOL)::SendMessageW((hwnd), LVM_SETITEMW, 0, (LPARAM)(pitem))

#define ListView_GetItemW(hwnd, pitem) \
  (BOOL)::SendMessageW((hwnd), LVM_GETITEMW, 0, (LPARAM)(LVITEMW *)(pitem))

#define ComboBox_AddStringW(hwndCtl, lpsz) \
  ((int)(DWORD)::SendMessageW((hwndCtl), CB_ADDSTRING, 0L, (LPARAM)(LPCWSTR)(lpsz)))

#define ComboBox_InsertStringA(hwndCtl, index, lpsz) \
  ((int)(DWORD)::SendMessageA((hwndCtl), CB_INSERTSTRING, (WPARAM)(int)(index), (LPARAM)(LPCSTR)(lpsz)))

#define ComboBox_InsertStringW(hwndCtl, index, lpsz) \
  ((int)(DWORD)::SendMessageW((hwndCtl), CB_INSERTSTRING, (WPARAM)(int)(index), (LPARAM)(LPCWSTR)(lpsz)))


//String Format
#define SF_DEC                      L"%u"
#define SF_DEC16                    L"%hu"
#define SF_HEX                      L"0x%08X"
#define SF_HEX_DEC                  L"0x%08X  (Dec:%u)"
#define SF_HEX16                    L"0x%04hX"
#define SF_HEX_DEC16                L"0x%04hX  (Dec:%hu)"
#define SF_HEX8                     L"0x%02hX"
#define SF_HEX_DEC8                 L"0x%02hX  (Dec:%hu)"
#define SF_DATE                     L"%04u-%02u-%02u %02u:%02u:%02u"

//Item Index
#define ItemIndex(i)                (i)
#define ItemIndexPlus(i)            (i++)


uint32_t crc32_fast(const void* data, size_t length, uint32_t previousCrc32 = 0);
