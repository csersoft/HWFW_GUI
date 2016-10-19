#pragma once

#include "resource.h"
#include "HWNP.h"

enum TVI_TYPE : unsigned int
{
	TT_NONE,
	TT_FILEHDR,
	TT_MODELINFO,
	TT_ITEMINFO,
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
	LVI_TYPE						ltType;
	DWORD								dwFlags;
	DWORD								dwUserData;
	LPARAM							lParam;
} LVS, *PLVS;


typedef struct _DIALOG_ITEMINFO_STRUCT
{
	DLGII_TYPE					dtType;
	uint32_t						u32Index;
	PWCHAR							lpFile;
} DLGIIS, *PDLGIIS;

extern HINSTANCE hInst;
extern HWND hMainDlg;

void CleanListView();

void CleanView();

void OpenFirmware(LPCWSTR lpPath);

void ListItemInfo(HWND hListView);

void TreeView_SetProc(HWND hCtrl);
void TreeView_SelChanged(LPNMTREEVIEW lpnmTV);


void ListView_SetProc(HWND hCtrl);
void ListView_RightClick(LPNMITEMACTIVATE lpnmItemActivate);
int ListView_AddColumn(HWND hCtrl, int nWidth, int nIndex, LPWSTR lpText);
int ListView_AddItem(HWND hCtrl, int iItem, int iSubItem, LPWSTR lpText, LVI_TYPE ltType, DWORD dwFlags, DWORD dwUserData, LPARAM lParam);
int ListView_CurHotItem();

