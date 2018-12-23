#pragma once

#ifndef _WINDOWS_
#include <Windows.h>
#endif

#ifndef _INC_WINDOWSX
#include <WindowsX.h>
#endif

#ifndef _INC_COMMCTRL
#include <CommCtrl.h>
#endif

#include "resource.h"
#include "HWNP.h"
#include "HWFW_GUI.hpp"

void ListItemInfo(HWND hListView);
void UpdateItemInfo(HWND hListView);

void TreeView_SetProc(HWND hCtrl);
void TreeView_SelChanged(LPNMTREEVIEW lpnmTV);

void ListView_SetProc(HWND hCtrl);
void ListView_RightClick(LPNMITEMACTIVATE lpnmItemActivate);
int ListView_AddColumn(HWND hCtrl, int nWidth, int nIndex, LPWSTR lpText);
int ListView_AddItemA(HWND hCtrl, int iItem, int iSubItem, LPSTR lpText, LVI_TYPE ltType, DWORD dwFlags, DWORD dwUserData, LPARAM lParam);
int ListView_AddItemW(HWND hCtrl, int iItem, int iSubItem, LPWSTR lpText, LVI_TYPE ltType, DWORD dwFlags, DWORD dwUserData, LPARAM lParam);
int ListView_CurHotItem();
