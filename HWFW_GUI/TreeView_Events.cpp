#include "stdafx.h"
#include "HWFW_GUI.h"

#define LV_CFG_SHOW_DATAINFO              0

static WNDPROC wfnTV = NULL;

extern CHAR chIniPath[MAX_PATH];

static void ListFileHeader(HWND hListView)
{
  int nIndex = 0;
  HWNP_HEADER fwHeader;      //firmware header
  WCHAR wsTemp[128];

  DragAcceptFiles(hListView, FALSE);
  ListView_SetView(hListView, LV_VIEW_DETAILS);
  ListView_SetExtendedListViewStyle(hListView, LVS_EX_DEFAULT);


  ListView_AddColumn(hListView, 96, 0, L"文件偏移");
  ListView_AddColumn(hListView, 256, 1, L"成员数值");
  ListView_AddColumn(hListView, 384, 2, L"成员描述");

  if (HWNP_GetFirmwareHeader(&fwHeader) != 0) return;


  swprintf_s(wsTemp, SF_HEX, OffsetOf(HWNP_HEADER, BasicFileHeader.u32Magic));
  ListView_AddItemW(hListView, ItemIndex(nIndex), 0, wsTemp, LT_NONE, 0, 0, 0);
  swprintf_s(wsTemp, SF_HEX, fwHeader.BasicFileHeader.u32Magic);
  ListView_AddItemW(hListView, ItemIndex(nIndex), 1, wsTemp, LT_NONE, 0, 0, 0);
  ListView_AddItemW(hListView, ItemIndexPlus(nIndex), 2, L"Magic            #  魔法签名  (Little-Endian)", LT_NONE, 0, 0, 0);


  swprintf_s(wsTemp, SF_HEX, OffsetOf(HWNP_HEADER, BasicFileHeader.beu32FileSize));
  ListView_AddItemW(hListView, ItemIndex(nIndex), 0, wsTemp, LT_NONE, 0, 0, 0);
  swprintf_s(wsTemp, SF_HEX_DEC, BigLittleSwap32(fwHeader.BasicFileHeader.beu32FileSize), BigLittleSwap32(fwHeader.BasicFileHeader.beu32FileSize));
  ListView_AddItemW(hListView, ItemIndex(nIndex), 1, wsTemp, LT_NONE, 0, 0, 0);
  ListView_AddItemW(hListView, ItemIndexPlus(nIndex), 2, L"FileSize         #  文件大小  (Big-Endian)", LT_NONE, 0, 0, 0);


  swprintf_s(wsTemp, SF_HEX, OffsetOf(HWNP_HEADER, BasicFileHeader.u32FileCRC32));
  ListView_AddItemW(hListView, ItemIndex(nIndex), 0, wsTemp, LT_NONE, 0, 0, 0);
  swprintf_s(wsTemp, SF_HEX, fwHeader.BasicFileHeader.u32FileCRC32);
  ListView_AddItemW(hListView, ItemIndex(nIndex), 1, wsTemp, LT_NONE, 0, 0, 0);
  ListView_AddItemW(hListView, ItemIndexPlus(nIndex), 2, L"FileCRC32        #  文件CRC32  (Little-Endian)", LT_NONE, 0, 0, 0);


  swprintf_s(wsTemp, SF_HEX, OffsetOf(HWNP_HEADER, FileHeader2.u32HeaderSize));
  ListView_AddItemW(hListView, ItemIndex(nIndex), 0, wsTemp, LT_NONE, 0, 0, 0);
  swprintf_s(wsTemp, SF_HEX_DEC, fwHeader.FileHeader2.u32HeaderSize, fwHeader.FileHeader2.u32HeaderSize);
  ListView_AddItemW(hListView, ItemIndex(nIndex), 1, wsTemp, LT_NONE, 0, 0, 0);
  ListView_AddItemW(hListView, ItemIndexPlus(nIndex), 2, L"HeaderSize       #  头部大小  (Little-Endian)", LT_NONE, 0, 0, 0);


  swprintf_s(wsTemp, SF_HEX, OffsetOf(HWNP_HEADER, FileHeader2.u32HeaderCRC32));
  ListView_AddItemW(hListView, ItemIndex(nIndex), 0, wsTemp, LT_NONE, 0, 0, 0);
  swprintf_s(wsTemp, SF_HEX, fwHeader.FileHeader2.u32HeaderCRC32);
  ListView_AddItemW(hListView, ItemIndex(nIndex), 1, wsTemp, LT_NONE, 0, 0, 0);
  ListView_AddItemW(hListView, ItemIndexPlus(nIndex), 2, L"HeaderCRC32      #  头部CRC32  (Little-Endian)", LT_NONE, 0, 0, 0);


  swprintf_s(wsTemp, SF_HEX, OffsetOf(HWNP_HEADER, PacketHeader.u32ItemCount));
  ListView_AddItemW(hListView, ItemIndex(nIndex), 0, wsTemp, LT_NONE, 0, 0, 0);
  swprintf_s(wsTemp, SF_HEX_DEC, fwHeader.PacketHeader.u32ItemCount, fwHeader.PacketHeader.u32ItemCount);
  ListView_AddItemW(hListView, ItemIndex(nIndex), 1, wsTemp, LT_NONE, 0, 0, 0);
  ListView_AddItemW(hListView, ItemIndexPlus(nIndex), 2, L"ItemCount        #  项目信息计数  (Little-Endian)", LT_NONE, 0, 0, 0);


  swprintf_s(wsTemp, SF_HEX, OffsetOf(HWNP_HEADER, PacketHeader.u8PackTotal));
  ListView_AddItemW(hListView, ItemIndex(nIndex), 0, wsTemp, LT_NONE, 0, 0, 0);
  swprintf_s(wsTemp, SF_HEX8, LOBYTE(fwHeader.PacketHeader.u8PackTotal));
  ListView_AddItemW(hListView, ItemIndex(nIndex), 1, wsTemp, LT_NONE, 0, 0, 0);
  ListView_AddItemW(hListView, ItemIndexPlus(nIndex), 2, L"PackTotal        #  PackTotal  (Little-Endian)", LT_NONE, 0, 0, 0);


  swprintf_s(wsTemp, SF_HEX, OffsetOf(HWNP_HEADER, PacketHeader.u8PackNum));
  ListView_AddItemW(hListView, ItemIndex(nIndex), 0, wsTemp, LT_NONE, 0, 0, 0);
  swprintf_s(wsTemp, SF_HEX8, LOBYTE(fwHeader.PacketHeader.u8PackNum));
  ListView_AddItemW(hListView, ItemIndex(nIndex), 1, wsTemp, LT_NONE, 0, 0, 0);
  ListView_AddItemW(hListView, ItemIndexPlus(nIndex), 2, L"PackNum          #  PackNum  (Little-Endian)", LT_NONE, 0, 0, 0);


  swprintf_s(wsTemp, SF_HEX, OffsetOf(HWNP_HEADER, PacketHeader.u16ProductListSize));
  ListView_AddItemW(hListView, ItemIndex(nIndex), 0, wsTemp, LT_NONE, 0, 0, 0);
  swprintf_s(wsTemp, SF_HEX_DEC16, fwHeader.PacketHeader.u16ProductListSize, fwHeader.PacketHeader.u16ProductListSize);
  ListView_AddItemW(hListView, ItemIndex(nIndex), 1, wsTemp, LT_NONE, 0, 0, 0);
  ListView_AddItemW(hListView, ItemIndexPlus(nIndex), 2, L"ProductListSize  #  产品支持列表大小  (Little-Endian)", LT_NONE, 0, 0, 0);


  swprintf_s(wsTemp, SF_HEX, OffsetOf(HWNP_HEADER, PacketHeader.u16ItemInfoSize));
  ListView_AddItemW(hListView, ItemIndex(nIndex), 0, wsTemp, LT_NONE, 0, 0, 0);
  swprintf_s(wsTemp, SF_HEX_DEC16, fwHeader.PacketHeader.u16ItemInfoSize, fwHeader.PacketHeader.u16ItemInfoSize);
  ListView_AddItemW(hListView, ItemIndex(nIndex), 1, wsTemp, LT_NONE, 0, 0, 0);
  ListView_AddItemW(hListView, ItemIndexPlus(nIndex), 2, L"ItemSize         #  项目信息大小  (Little-Endian)", LT_NONE, 0, 0, 0);


  swprintf_s(wsTemp, SF_HEX, OffsetOf(HWNP_HEADER, PacketHeader.u32Reserved));
  ListView_AddItemW(hListView, ItemIndex(nIndex), 0, wsTemp, LT_NONE, 0, 0, 0);
  swprintf_s(wsTemp, SF_HEX_DEC, fwHeader.PacketHeader.u32Reserved, fwHeader.PacketHeader.u32Reserved);
  ListView_AddItemW(hListView, ItemIndex(nIndex), 1, wsTemp, LT_NONE, 0, 0, 0);
  ListView_AddItemW(hListView, ItemIndexPlus(nIndex), 2, L"Reserved         #  保留  (Little-Endian)", LT_NONE, 0, 0, 0);
}

static void ListProductInfo(HWND hListView)
{
  int nIndex = 0;
  LPCH lpProdList, lpStr, lpEndStr, lpTmp;
  BOOL blInterface = FALSE;
  uint16_t u16Size = 0;
  CHAR chTmp[256], chTmp2[256];

  DragAcceptFiles(hListView, FALSE);
  ListView_SetView(hListView, LV_VIEW_DETAILS);
  ListView_SetExtendedListViewStyle(hListView, LVS_EX_DEFAULT);


  ListView_AddColumn(hListView, 96, 0, L"产品标识");
  ListView_AddColumn(hListView, 512, 1, L"匹配的产品名称");

  HWNP_GetProductListSize(&u16Size);

  if (u16Size == 0) return;
  
  u16Size += 4;
  lpProdList = (LPCH)calloc(u16Size, sizeof(CHAR));

  if (lpProdList == NULL) return;

  HWNP_GetProductList(lpProdList, u16Size);

  lpStr = lpProdList;

  while (*lpStr != 0)
  {
    if (*lpStr == '|')
    {
      lpStr++;
      continue;
    }

    lpEndStr = strchr(lpStr, '|');

    if (lpEndStr == NULL) break;

    strncpy_s(chTmp, lpStr, lpEndStr - lpStr);
    lpTmp = chTmp;

    if (*lpTmp == ';')
    {
      lpTmp++;

      if (blInterface == FALSE)
      {
        blInterface = TRUE;

        ListView_AddItemA(hListView, ItemIndex(nIndex), 0, ";;;", LT_MODELINFO, 0, 0, 0);
        ListView_AddItemA(hListView, ItemIndexPlus(nIndex), 1, ";;;  WEB Interface [WEB界面]  ;;;", LT_MODELINFO, 0, 0, 0);
      }
    }

    if (blInterface)
    {
      ListView_AddItemA(hListView, ItemIndex(nIndex), 0, lpTmp, LT_MODELINFO, 0, 0, 0);

      if (strcmp(lpTmp, "E8C") == 0)
        ListView_AddItemA(hListView, ItemIndexPlus(nIndex), 1, "中国电信界面 [China Telecom WEB Interface]", LT_MODELINFO, 0, 0, 0);
      else if (strcmp(lpTmp, "COMMON") == 0)
        ListView_AddItemA(hListView, ItemIndexPlus(nIndex), 1, "华为原厂界面 [HuaWei WEB Interface]", LT_MODELINFO, 0, 0, 0);
      else if (strcmp(lpTmp, "CHINA") == 0)
        ListView_AddItemA(hListView, ItemIndexPlus(nIndex), 1, "中国联通界面 [China Unicom WEB Interface]", LT_MODELINFO, 0, 0, 0);
      else if (strcmp(lpTmp, "CMCC") == 0)
        ListView_AddItemA(hListView, ItemIndexPlus(nIndex), 1, "中国移动界面 [China Mobile WEB Interface]", LT_MODELINFO, 0, 0, 0);
      else
        ListView_AddItemA(hListView, ItemIndexPlus(nIndex), 1, "未知界面 [Unknown Interface]", LT_MODELINFO, 0, 0, 0);
    }
    else
    {
      GetPrivateProfileStringA("HW_ProductDatabase", lpTmp, "< N/A >", chTmp2, sizeof(chTmp2), chIniPath);

      ListView_AddItemA(hListView, ItemIndex(nIndex), 0, lpTmp, LT_MODELINFO, 0, 0, 0);
      ListView_AddItemA(hListView, ItemIndexPlus(nIndex), 1, chTmp2, LT_MODELINFO, 0, 0, 0);
    }

    lpStr = lpEndStr + 1;
  }

  free(lpProdList);
}


void ListItemInfo(HWND hListView)
{
  //int nIndex = 0;
  uint32_t u32Count;
  HWNP_HEADER hwPakHdr;
  HWNP_ITEMINFO hwItemInfo;
  WCHAR wsTemp[260];

  DragAcceptFiles(hListView, FALSE);
  ListView_SetView(hListView, LV_VIEW_DETAILS);
  ListView_SetExtendedListViewStyle(hListView, LVS_EX_DEFAULT);


  ListView_AddColumn(hListView, 100, 0, L"项目信息偏移");
  ListView_AddColumn(hListView, 60, 1, L"项目Id");
  ListView_AddColumn(hListView, 288, 2, L"项目路径");
  ListView_AddColumn(hListView, 96, 3, L"项目策略");
  ListView_AddColumn(hListView, 96, 4, L"项目CRC32");
#if LV_CFG_SHOW_DATAINFO
  ListView_AddColumn(hListView, 100, 5, L"项目数据偏移");
  ListView_AddColumn(hListView, 200, 6, L"项目数据大小");
  ListView_AddColumn(hListView, 104, 7, L"项目类型");
  ListView_AddColumn(hListView, 256, 8, L"项目版本");
  ListView_AddColumn(hListView, 90, 9, L"保留数据");
#else
  ListView_AddColumn(hListView, 104, 5, L"项目类型");
  ListView_AddColumn(hListView, 256, 6, L"项目版本");
  ListView_AddColumn(hListView, 90, 7, L"保留数据");
#endif


  if (HWNP_GetFirmwareHeader(&hwPakHdr) != 0) return;
  if (HWNP_GetItemCount(&u32Count) != 0) return;

  for (uint32_t u32Index = 0; u32Index < u32Count; u32Index++)
  {
    size_t stOut;

    swprintf_s(wsTemp, SF_HEX, sizeof(HWNP_HEADER) + hwPakHdr.PacketHeader.u16ProductListSize + u32Index * sizeof(HWNP_ITEMINFO));
    ListView_AddItemW(hListView, u32Index, 0, wsTemp, LT_ITEMINFO, 0, u32Index, 0);

    if (HWNP_GetItemInfoByIndex(u32Index, &hwItemInfo) != 0) continue;

    swprintf_s(wsTemp, SF_DEC, hwItemInfo.u32Id);
    ListView_AddItemW(hListView, u32Index, 1, wsTemp, LT_ITEMINFO, 0, 0, 0);

    mbstowcs_s(&stOut, wsTemp, hwItemInfo.chItemPath, sizeof(HWNP_ITEMINFO::chItemPath));
    ListView_AddItemW(hListView, u32Index, 2, wsTemp, LT_ITEMINFO, 0, 0, 0);

    swprintf_s(wsTemp, SF_HEX, hwItemInfo.u32Policy);
    ListView_AddItemW(hListView, u32Index, 3, wsTemp, LT_ITEMINFO, 0, 0, 0);

    swprintf_s(wsTemp, SF_HEX, hwItemInfo.u32ItemCRC32);
    ListView_AddItemW(hListView, u32Index, 4, wsTemp, LT_ITEMINFO, 0, 0, 0);

#if LV_CFG_SHOW_DATAINFO
    swprintf_s(wsTemp, SF_HEX, hwItemInfo.u32Offset);
    ListView_AddItemW(hListView, u32Index, 5, wsTemp, LT_ITEMINFO, 0, 0, 0);

    swprintf_s(wsTemp, SF_HEX_DEC, hwItemInfo.u32Size, hwItemInfo.u32Size);
    ListView_AddItemW(hListView, u32Index, 6, wsTemp, LT_ITEMINFO, 0, 0, 0);

    mbstowcs_s(&stOut, wsTemp, hwItemInfo.chItemType, sizeof(HWNP_ITEMINFO::chItemType));
    ListView_AddItemW(hListView, u32Index, 7, wsTemp, LT_ITEMINFO, 0, 0, 0);

    mbstowcs_s(&stOut, wsTemp, hwItemInfo.chItemVersion, sizeof(HWNP_ITEMINFO::chItemVersion));
    ListView_AddItemW(hListView, u32Index, 8, wsTemp, LT_ITEMINFO, 0, 0, 0);

    swprintf_s(wsTemp, SF_HEX, hwItemInfo.u32Reserved);
    ListView_AddItemW(hListView, u32Index, 9, wsTemp, LT_ITEMINFO, 0, 0, 0);
#else
    mbstowcs_s(&stOut, wsTemp, hwItemInfo.chItemType, sizeof(HWNP_ITEMINFO::chItemType));
    ListView_AddItemW(hListView, u32Index, 5, wsTemp, LT_ITEMINFO, 0, 0, 0);

    mbstowcs_s(&stOut, wsTemp, hwItemInfo.chItemVersion, sizeof(HWNP_ITEMINFO::chItemVersion));
    ListView_AddItemW(hListView, u32Index, 6, wsTemp, LT_ITEMINFO, 0, 0, 0);

    swprintf_s(wsTemp, SF_HEX, hwItemInfo.u32Reserved);
    ListView_AddItemW(hListView, u32Index, 7, wsTemp, LT_ITEMINFO, 0, 0, 0);
#endif
  }
}

void UpdateItemInfo(HWND hListView)
{
  uint32_t u32Count;
  HWNP_HEADER hwPakHdr;
  HWNP_ITEMINFO hwItemInfo;
  WCHAR wsTemp[260];

  ListView_DeleteAllItems(GetDlgItem(hMainDlg, IDC_LV));

  if (HWNP_GetFirmwareHeader(&hwPakHdr) != 0) return;
  if (HWNP_GetItemCount(&u32Count) != 0) return;

  for (uint32_t u32Index = 0; u32Index < u32Count; u32Index++)
  {
    size_t stOut;

    swprintf_s(wsTemp, SF_HEX, sizeof(HWNP_HEADER) + hwPakHdr.PacketHeader.u16ProductListSize + u32Index * sizeof(HWNP_ITEMINFO));
    ListView_AddItemW(hListView, u32Index, 0, wsTemp, LT_ITEMINFO, 0, u32Index, 0);

    if (HWNP_GetItemInfoByIndex(u32Index, &hwItemInfo) != 0) continue;

    swprintf_s(wsTemp, SF_DEC, hwItemInfo.u32Id);
    ListView_AddItemW(hListView, u32Index, 1, wsTemp, LT_ITEMINFO, 0, 0, 0);

    mbstowcs_s(&stOut, wsTemp, hwItemInfo.chItemPath, sizeof(HWNP_ITEMINFO::chItemPath));
    ListView_AddItemW(hListView, u32Index, 2, wsTemp, LT_ITEMINFO, 0, 0, 0);

    swprintf_s(wsTemp, SF_HEX, hwItemInfo.u32Policy);
    ListView_AddItemW(hListView, u32Index, 3, wsTemp, LT_ITEMINFO, 0, 0, 0);

    swprintf_s(wsTemp, SF_HEX, hwItemInfo.u32ItemCRC32);
    ListView_AddItemW(hListView, u32Index, 4, wsTemp, LT_ITEMINFO, 0, 0, 0);

#if LV_CFG_SHOW_DATAINFO
    swprintf_s(wsTemp, SF_HEX, hwItemInfo.u32Offset);
    ListView_AddItemW(hListView, u32Index, 5, wsTemp, LT_ITEMINFO, 0, 0, 0);

    swprintf_s(wsTemp, SF_HEX_DEC, hwItemInfo.u32Size, hwItemInfo.u32Size);
    ListView_AddItemW(hListView, u32Index, 6, wsTemp, LT_ITEMINFO, 0, 0, 0);

    mbstowcs_s(&stOut, wsTemp, hwItemInfo.chItemType, sizeof(HWNP_ITEMINFO::chItemType));
    ListView_AddItemW(hListView, u32Index, 7, wsTemp, LT_ITEMINFO, 0, 0, 0);

    mbstowcs_s(&stOut, wsTemp, hwItemInfo.chItemVersion, sizeof(HWNP_ITEMINFO::chItemVersion));
    ListView_AddItemW(hListView, u32Index, 8, wsTemp, LT_ITEMINFO, 0, 0, 0);

    swprintf_s(wsTemp, SF_HEX, hwItemInfo.u32Reserved);
    ListView_AddItemW(hListView, u32Index, 9, wsTemp, LT_ITEMINFO, 0, 0, 0);
#else
    mbstowcs_s(&stOut, wsTemp, hwItemInfo.chItemType, sizeof(HWNP_ITEMINFO::chItemType));
    ListView_AddItemW(hListView, u32Index, 5, wsTemp, LT_ITEMINFO, 0, 0, 0);

    mbstowcs_s(&stOut, wsTemp, hwItemInfo.chItemVersion, sizeof(HWNP_ITEMINFO::chItemVersion));
    ListView_AddItemW(hListView, u32Index, 6, wsTemp, LT_ITEMINFO, 0, 0, 0);

    swprintf_s(wsTemp, SF_HEX, hwItemInfo.u32Reserved);
    ListView_AddItemW(hListView, u32Index, 7, wsTemp, LT_ITEMINFO, 0, 0, 0);
#endif
  }
}

static LRESULT CALLBACK TreeView_WndProc(HWND hCtrl, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  if (!wfnTV)
    return DefWindowProc(hCtrl, Msg, wParam, lParam);

  if (Msg == WM_DROPFILES)
  {
    WCHAR wsFile[MAX_PATH];

    if (DragQueryFile((HDROP)wParam, 0, wsFile, MAX_PATH))
    {
      OpenFirmware(wsFile);

      LPWSTR lpFileName = wcsrchr(wsFile, '\\');
      WCHAR wsTemp[MAX_PATH];

      if (lpFileName)
      {
        lpFileName++;

        swprintf_s(wsTemp, L"%s  %s  %s  Build:%s [%s]", APP_NAME, APP_VER1, APP_VER2, APP_BUILD_VER, lpFileName);
        SetWindowTextW(hMainDlg, wsTemp);
      }
    }

    DragFinish((HDROP)wParam);
    return 0;
  }

  return CallWindowProc(wfnTV, hCtrl, Msg, wParam, lParam);
}

void TreeView_SetProc(HWND hCtrl)
{
  wfnTV = (WNDPROC)SetWindowLongPtr(hCtrl, GWL_WNDPROC, (LONG)&TreeView_WndProc);
}

void TreeView_SelChanged(LPNMTREEVIEW lpnmTV)
{
  HWND hListView = GetDlgItem(hMainDlg, IDC_LV);
  CleanListView();

  switch (lpnmTV->itemNew.lParam)
  {
  case TT_FILEHDR:
  ListFileHeader(hListView);
  break;

  case TT_MODELINFO:
  ListProductInfo(hListView);
  break;

  case TT_ITEMINFO:
  ListItemInfo(hListView);
  break;
  }
}

