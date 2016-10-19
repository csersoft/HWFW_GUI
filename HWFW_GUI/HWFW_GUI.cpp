#include "stdafx.h"
#include "HWFW_GUI.h"


// 全局变量: 
HINSTANCE hInst = NULL;                                // 当前实例
HWND hMainDlg = NULL;
HMENU hmPop = NULL;
HMENU hmItemInfo = NULL;


BOOL WINAPI ScreenToClient(
	__in HWND hWnd,
	__inout LPRECT lpRect)
{
	ScreenToClient(hWnd, (LPPOINT)lpRect);
	return ScreenToClient(hWnd, (LPPOINT)&(lpRect->right));
}

// 此代码模块中包含的函数的前向声明: 
BOOL								InitInstance(HINSTANCE, int);
INT_PTR CALLBACK		DlgProc_Main(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK		DlgProc_ItemInfo(HWND, UINT, WPARAM , LPARAM);
INT_PTR CALLBACK		DlgProc_About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// 执行应用程序初始化: 
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	return 1;
}



//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中

	DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINDLG), NULL, &DlgProc_Main);

	return TRUE;
}

void SetStatus(LPCWSTR lpText, ...)
{
	va_list argptr;
	WCHAR wsTmp[512];

	va_start(argptr, lpText);
	_vstprintf_s(wsTmp, 512, lpText, argptr);
	va_end(argptr);

	SetWindowTextW(GetDlgItem(hMainDlg, IDC_LBL_STATUS), wsTmp);
}

void SetDialogStatus(HWND hCtrl, LPCWSTR lpText, ...)
{
	va_list argptr;
	WCHAR wsTmp[512];

	va_start(argptr, lpText);
	_vstprintf_s(wsTmp, 512, lpText, argptr);
	va_end(argptr);

	SetWindowTextW(hCtrl, wsTmp);
}

BOOL QueryMsg(HWND hOwner, LPCWSTR szText, LPCWSTR szTitle)
{
	if (MessageBox(hOwner, szText, szTitle, MB_ICONQUESTION | MB_YESNO) == IDYES)
		return TRUE;
	else
		return FALSE;
}

BOOL GetOpenFilePath(HWND hOwner, LPWSTR lpFilePath, DWORD dwMax)
{
	OPENFILENAMEW ofn;

	if (!lpFilePath || !dwMax) return FALSE;
	ZeroMemory(&ofn, sizeof(OPENFILENAMEW));

	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = hOwner;
	ofn.lpstrFilter = L"所有文件(*.*)\0*.*\0\0";
	ofn.lpstrFile = lpFilePath;
	ofn.nMaxFile = dwMax;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;
	return GetOpenFileNameW(&ofn);
}

BOOL GetSaveFilePath(HWND hOwner, LPWSTR lpFilePath, DWORD dwMax)
{
	OPENFILENAMEW ofn;

	if (!lpFilePath || !dwMax) return FALSE;
	ZeroMemory(&ofn, sizeof(OPENFILENAMEW));

	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = hOwner;
	ofn.lpstrFilter = L"所有文件(*.*)\0*.*\0\0";
	ofn.lpstrFile = lpFilePath;
	ofn.nMaxFile = dwMax;
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_EXPLORER;
	return GetSaveFileNameW(&ofn);
}

BOOL ExportToFile(LPCWSTR lpFile, LPCVOID lpData, DWORD dwSize)
{
	HANDLE hFile = CreateFileW(lpFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) return FALSE;

	SetFilePointer(hFile, dwSize, NULL, FILE_BEGIN);
	SetEndOfFile(hFile);

	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	WriteFile(hFile, lpData, dwSize, NULL, NULL);

	CloseHandle(hFile);
	return TRUE;
}

void CleanListView()
{
	ListView_DeleteAllItems(GetDlgItem(hMainDlg, IDC_LV));
	while (ListView_DeleteColumn(GetDlgItem(hMainDlg, IDC_LV), 0));
}

void CleanView()
{
	TreeView_DeleteAllItems(GetDlgItem(hMainDlg, IDC_TV));
	CleanListView();
}

int UpdateView()
{
	HTREEITEM htiLastItem;
	TVINSERTSTRUCTW tvis;

	if (HWNP_GetState() != -1) return -1;
	CleanView();

	ZeroMemory(&tvis, sizeof(TVINSERTSTRUCTW));
	tvis.hParent = TVI_ROOT;
	tvis.hInsertAfter = TVI_LAST;
	tvis.item.mask = TVIF_TEXT | TVIF_PARAM;

	tvis.item.pszText = _T("<固件文件头部>");
	tvis.item.lParam = (LPARAM)TT_FILEHDR;
	htiLastItem = TreeView_DlgInsertItemW(hMainDlg, IDC_TV, &tvis);

	tvis.item.pszText = _T("<型号支持信息>");
	tvis.item.lParam = (LPARAM)TT_MODELINFO;
	htiLastItem = TreeView_DlgInsertItemW(hMainDlg, IDC_TV, &tvis);

	tvis.item.pszText = _T("<包含项目信息>");
	tvis.item.lParam = (LPARAM)TT_ITEMINFO;
	htiLastItem = TreeView_DlgInsertItemW(hMainDlg, IDC_TV, &tvis);

	TreeView_DlgSetItemW(hMainDlg, IDC_TV, htiLastItem);

	return 0;
}

void OpenFirmware(LPCWSTR lpPath)
{
	int nResult;

	if ((nResult = HWNP_OpenFirmware(lpPath)) != 0)
	{
		SetStatus(L"打开文件失败!错误码:[%d]", nResult);
		return;
	}

	if ((nResult = UpdateView()) != 0)
	{
		SetStatus(L"更新视图失败!错误码:[%d]", nResult);
		return ;
	}
}

// 主窗口的消息处理程序。
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
		RECT rcDlg, rcTmp, rcTmp2;

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

		wcscpy_s(wsPath, L"firmware.bin");

		SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDI_HWFW_GUI)));
		SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDI_HWFW_GUI)));
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
					OpenFirmware(wsPath);
				else
					SetStatus(L"获取文件路径失败!");
			}
			break;

			case IDM_SAVE:
			{
				int nResult = HWNP_Save();

				if (nResult == 0)
					SetStatus(L"文件保存完成.");
				else
				{
					WCHAR wsTmp[128];

					swprintf_s(wsTmp, L"文件保存失败,错误码:[%d]!", nResult);
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
						SetStatus(L"文件保存完成.");
					else
					{
						swprintf_s(wsTmp, L"文件保存失败,错误码:[%d]!", nResult);
						SetStatus(wsTmp);
					}
				}
				else
					SetStatus(L"获取文件路径失败!");
			}
			break;

			case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUT), hDlg, &DlgProc_About);
			break;

			case IDM_CHKCRC:
			{
				uint32_t u32Result = HWNP_CheckCRC32();

				if (u32Result == CHKCRC_OK)
					SetStatus(L"CRC32校验通过.");
				else if (u32Result == CHKCRC_HEADERCRCERR)
					SetStatus(L"头部CRC32校验失败!");
				else if (u32Result == CHKCRC_FILECRCERR)
					SetStatus(L"文件CRC32校验失败!");
				else if (u32Result >= CHKCRC_ITEMCRCERR)
				{
					WCHAR wsTmp[128];

					swprintf_s(wsTmp, L"项目校验失败,Index:[%u]!", u32Result - CHKCRC_ITEMCRCERR);
					SetStatus(wsTmp);
				}
				else
					SetStatus(L"未知的CRC32校验结果!");
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
						dlgiis.u32Index = (uint32_t)((PLVS)lvi.lParam)->dwUserData;
					}

					DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_ITEMINFO), hDlg, &DlgProc_ItemInfo, (LPARAM)&dlgiis);
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
		}
	}
	break;

	case WM_CLOSE:
	EndDialog(hDlg, IDCLOSE);
	break;

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

INT_PTR CALLBACK DlgProc_ItemInfo(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static PDLGIIS lpDlgIIS = NULL;
	static HWNP_ITEMINFO hwItemInfo;

	switch (message)
	{
	case WM_INITDIALOG:
	{
		if (lpDlgIIS != NULL) return (INT_PTR)FALSE;
		if (lParam == NULL) return (INT_PTR)FALSE;

		int nResult;
		size_t stOut;
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

			swprintf_s(wsTemp, SF_DEC, hwItemInfo.u32Id);
			SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_ID), wsTemp);

			swprintf_s(wsTemp, SF_HEX, hwItemInfo.u32ItemCRC32);
			SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_CRC), wsTemp);

			swprintf_s(wsTemp, SF_HEX, hwItemInfo.u32Offset);
			SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_OFFSET), wsTemp);

			swprintf_s(wsTemp, SF_HEX, hwItemInfo.u32Size);
			SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_SIZE), wsTemp);

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
		else if (lpDlgIIS->dtType == DT_ADD)
		{
			EnableWindow(GetDlgItem(hDlg, IDC_EXPORT), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_IMPORT), FALSE);

			hwItemInfo.u32Id = HWNP_GetLastItemId();
			swprintf_s(wsTemp, SF_DEC, hwItemInfo.u32Id);
			SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_ID), wsTemp);

			SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_CRC), L"N/A");
			SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_OFFSET), L"N/A");
			SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_SIZE), L"N/A");

			strcpy_s(hwItemInfo.chItemPath, "");
			mbstowcs_s(&stOut, wsTemp, hwItemInfo.chItemPath, sizeof(HWNP_ITEMINFO::chItemPath));
			SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_PATH), wsTemp);

			strcpy_s(hwItemInfo.chItemType, "UNKNOWN");
			mbstowcs_s(&stOut, wsTemp, hwItemInfo.chItemType, sizeof(HWNP_ITEMINFO::chItemType));
			SetWindowTextW(GetDlgItem(hDlg, IDC_CB_TYPE), wsTemp);

			strcpy_s(hwItemInfo.chItemVersion, "");
			mbstowcs_s(&stOut, wsTemp, hwItemInfo.chItemVersion, sizeof(HWNP_ITEMINFO::chItemVersion));
			SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_VER), wsTemp);

			hwItemInfo.u32Policy = 0;
			swprintf_s(wsTemp, SF_HEX, hwItemInfo.u32Policy);
			SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_POLICY), wsTemp);

			hwItemInfo.u32Reserved = 0;
			swprintf_s(wsTemp, SF_HEX, hwItemInfo.u32Reserved);
			SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_UNKDAT), wsTemp);
		}

		return (INT_PTR)TRUE;
	}
	break;

	case WM_COMMAND:
	{
		WORD wId = LOWORD(wParam), wNc = HIWORD(wParam);

		if (lParam != 0)
		{
			switch (wId)
			{
			case IDC_EXPORT:
			if (lpDlgIIS->dtType == DT_EDIT)
			{
				int nResult;
				uint32_t u32DataSize;
				LPCVOID lpData;
				WCHAR wsTmp[MAX_PATH];

				if (GetSaveFilePath(hDlg, wsTmp, MAX_PATH))
				{
					nResult = HWNP_GetItemDataSizeByIndex(lpDlgIIS->u32Index, &u32DataSize);
					if (nResult != 0)
					{
						SetDialogStatus(GetDlgItem(hDlg, IDC_LBL_II_STATUS), L"获取项目数据大小失败,错误码:[%d]!", nResult);
						break;
					}

					nResult = HWNP_GetItemDataPointerByIndex(lpDlgIIS->u32Index, &lpData);
					if (nResult != 0)
					{
						SetDialogStatus(GetDlgItem(hDlg, IDC_LBL_II_STATUS), L"获取项目数据失败,错误码:[%d]!", nResult);
						break;
					}

					ExportToFile(wsTmp, lpData, u32DataSize);
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
