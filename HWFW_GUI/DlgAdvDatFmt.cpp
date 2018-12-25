#include "stdafx.h"
#include "HWFW_GUI.hpp"

/************************************************************************/
/* WHWH子项目                                                           */
/************************************************************************/
typedef struct {
  BOOL          bIsInit = FALSE;
  BOOL          bIsImg = FALSE;
  HW_HDR        hdrWHWH;
  UIMG_HDR      hdrUIMG;
  LPCVOID       lpcData = NULL;
  LPCVOID       lpcImageData = NULL;
  LPVOID        lpData = NULL;
  LPVOID        lpImageData = NULL;
} SUBITEM_OBJ, *PSUBITEM_OBJ;


static LPCVOID lpItemData = NULL;
static uint32_t u32DataSize = 0;
static uint32_t u32ItemIdx;
static HWND hDlgFmt = NULL;
static PSUBITEM_OBJ lpSubItem = NULL;
static PSUBITEM_OBJ lpCurrentItem = NULL;
static uint32_t nSubItem = 0;

/************************************************************************/
/* 页对齐                                                               */
/************************************************************************/
static inline uint32_t alignPage(uint32_t val) {
  if (val % 0x1000 == 0) return val;
  return (val / 0x1000 + 1) * 0x1000;
}

static void Release()
{
  lpItemData = NULL;
  u32DataSize = 0;

  if (lpSubItem && nSubItem) {
    for (uint32_t i = 0; i < nSubItem; i++) {
      //lpSubItem[i]
    }
  }

  /*
  if (lpSubItem) {
    free(lpSubItem);
    lpSubItem = NULL;
    lpCurrentItem = NULL;
  }

  if (lpData_WHWH)
  {
    free(lpData_WHWH);
    lpData_WHWH = NULL;
  }

  if (lpData_UIMG)
  {
    free(lpData_UIMG);
    lpData_UIMG = NULL;
  }
  */
}

static void EnableWindow_UBootGroup(HWND hDlg, BOOL blEnable)
{
  EnableWindow(GetDlgItem(hDlg, IDC_EDIT_UBHDRCRC), blEnable);
  EnableWindow(GetDlgItem(hDlg, IDC_EDIT_UBDATCRC), blEnable);
  EnableWindow(GetDlgItem(hDlg, IDC_EDIT_UBSIZE), blEnable);
  EnableWindow(GetDlgItem(hDlg, IDC_EDIT_UBTIME), blEnable);
  EnableWindow(GetDlgItem(hDlg, IDC_EDIT_UBTIME_RO), blEnable);
  EnableWindow(GetDlgItem(hDlg, IDC_EDIT_UBLOAD), blEnable);
  EnableWindow(GetDlgItem(hDlg, IDC_EDIT_UBEP), blEnable);
  EnableWindow(GetDlgItem(hDlg, IDC_EDIT_UBNAME), blEnable);
  EnableWindow(GetDlgItem(hDlg, IDC_CB_UBOS), blEnable);
  EnableWindow(GetDlgItem(hDlg, IDC_CB_UBARCH), blEnable);
  EnableWindow(GetDlgItem(hDlg, IDC_CB_UBIMG), blEnable);
  EnableWindow(GetDlgItem(hDlg, IDC_CB_UBCOMP), blEnable);
  EnableWindow(GetDlgItem(hDlg, IDC_BTN_UBEXP), blEnable);
  EnableWindow(GetDlgItem(hDlg, IDC_BTN_UBIMP), blEnable);
  EnableWindow(GetDlgItem(hDlg, IDC_BTN_UBCH), blEnable);
  EnableWindow(GetDlgItem(hDlg, IDC_BTN_UBCD), blEnable);
  EnableWindow(GetDlgItem(hDlg, IDC_BTN_UBSAVE), blEnable);

  EnableWindow(GetDlgItem(hDlg, IDC_GB_UBOOT), blEnable);
}

static void UpdateDataView_WHWH(HWND hDlg)
{
  size_t stOut;
  tm _tm;
  WCHAR wsTemp[260];

  mbstowcs_s(&stOut, wsTemp, lpCurrentItem->hdrWHWH.chItemVersion, sizeof(HW_HDR::chItemVersion));
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_WHVER), wsTemp);

  swprintf_s(wsTemp, SF_HEX, lpCurrentItem->hdrWHWH.u32Time);
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_WHTIME), wsTemp);

  _localtime32_s(&_tm, &lpCurrentItem->hdrWHWH.u32Time);
  swprintf_s(wsTemp, SF_DATE, _tm.tm_year + 1900, _tm.tm_mon + 1, _tm.tm_mday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec);
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_WHTIME_RO), wsTemp);

  if ((lpCurrentItem->hdrWHWH.u32Type >= HW_ItemType::hwType_Invalid) && (lpCurrentItem->hdrWHWH.u32Type < HW_ItemType::hwType_Limit))
    ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_CB_WHTYPE), lpCurrentItem->hdrWHWH.u32Type);
  else
    ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_CB_WHTYPE), 0);

  swprintf_s(wsTemp, SF_HEX, lpCurrentItem->hdrWHWH.u32RearSize);
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_WHSIZE), wsTemp);

  swprintf_s(wsTemp, SF_HEX, lpCurrentItem->hdrWHWH.u32RearCRC);
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_WHCRC), wsTemp);
}

static void UpdateDataView_UIMG(HWND hDlg)
{
  size_t stOut;
  __time32_t _time32;
  tm _tm;
  WCHAR wsTemp[260];

  swprintf_s(wsTemp, SF_HEX, BigLittleSwap32(lpCurrentItem->hdrUIMG.ih_hcrc));
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_UBHDRCRC), wsTemp);

  swprintf_s(wsTemp, SF_HEX, BigLittleSwap32(lpCurrentItem->hdrUIMG.ih_dcrc));
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_UBDATCRC), wsTemp);

  swprintf_s(wsTemp, SF_HEX, BigLittleSwap32(lpCurrentItem->hdrUIMG.ih_size));
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_UBSIZE), wsTemp);

  _time32 = BigLittleSwap32(lpCurrentItem->hdrUIMG.ih_time);
  swprintf_s(wsTemp, SF_HEX, _time32);
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_UBTIME), wsTemp);

  _localtime32_s(&_tm, &_time32);
  swprintf_s(wsTemp, SF_DATE, _tm.tm_year + 1900, _tm.tm_mon + 1, _tm.tm_mday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec);
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_UBTIME_RO), wsTemp);

  mbstowcs_s(&stOut, wsTemp, lpCurrentItem->hdrUIMG.ih_name, sizeof(UIMG_HDR::ih_name));
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_UBNAME), wsTemp);

  swprintf_s(wsTemp, SF_HEX, BigLittleSwap32(lpCurrentItem->hdrUIMG.ih_load));
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_UBLOAD), wsTemp);

  swprintf_s(wsTemp, SF_HEX, BigLittleSwap32(lpCurrentItem->hdrUIMG.ih_ep));
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_UBEP), wsTemp);

  if (lpCurrentItem->hdrUIMG.ih_os < IH_OS::IH_OS_COUNT)
    ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_CB_UBOS), lpCurrentItem->hdrUIMG.ih_os);
  else
    ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_CB_UBOS), 0);

  if (lpCurrentItem->hdrUIMG.ih_arch < IH_ARCH::IH_ARCH_COUNT)
    ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_CB_UBARCH), lpCurrentItem->hdrUIMG.ih_arch);
  else
    ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_CB_UBARCH), 0);

  if (lpCurrentItem->hdrUIMG.ih_type < IH_TYPE::IH_TYPE_COUNT)
    ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_CB_UBIMG), lpCurrentItem->hdrUIMG.ih_type);
  else
    ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_CB_UBIMG), 0);

  if (lpCurrentItem->hdrUIMG.ih_comp < IH_COMP::IH_COMP_COUNT)
    ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_CB_UBCOMP), lpCurrentItem->hdrUIMG.ih_comp);
  else
    ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_CB_UBCOMP), 0);
}

static void ClearView() {
  //HW Group
  SetWindowTextW(GetDlgItem(hDlgFmt, IDC_EDIT_WHVER), L"");
  SetWindowTextW(GetDlgItem(hDlgFmt, IDC_EDIT_WHTIME), L"");
  SetWindowTextW(GetDlgItem(hDlgFmt, IDC_EDIT_WHTIME_RO), L"");
  ComboBox_SetCurSel(GetDlgItem(hDlgFmt, IDC_CB_WHTYPE), 0);
  SetWindowTextW(GetDlgItem(hDlgFmt, IDC_EDIT_WHSIZE), L"");
  SetWindowTextW(GetDlgItem(hDlgFmt, IDC_EDIT_WHCRC), L"");

  //UBoot group
  SetWindowTextW(GetDlgItem(hDlgFmt, IDC_EDIT_UBHDRCRC), L"");
  SetWindowTextW(GetDlgItem(hDlgFmt, IDC_EDIT_UBDATCRC), L"");
  SetWindowTextW(GetDlgItem(hDlgFmt, IDC_EDIT_UBSIZE), L"");
  SetWindowTextW(GetDlgItem(hDlgFmt, IDC_EDIT_UBTIME), L"");
  SetWindowTextW(GetDlgItem(hDlgFmt, IDC_EDIT_UBTIME_RO), L"");
  SetWindowTextW(GetDlgItem(hDlgFmt, IDC_EDIT_UBNAME), L"");
  SetWindowTextW(GetDlgItem(hDlgFmt, IDC_EDIT_UBLOAD), L"");
  SetWindowTextW(GetDlgItem(hDlgFmt, IDC_EDIT_UBEP), L"");
  ComboBox_SetCurSel(GetDlgItem(hDlgFmt, IDC_CB_UBOS), 0);
  ComboBox_SetCurSel(GetDlgItem(hDlgFmt, IDC_CB_UBARCH), 0);
  ComboBox_SetCurSel(GetDlgItem(hDlgFmt, IDC_CB_UBIMG), 0);
  ComboBox_SetCurSel(GetDlgItem(hDlgFmt, IDC_CB_UBCOMP), 0);
}

static BOOL UpdateDataView()
{
  ClearView();

  if (lpCurrentItem == NULL) return FALSE;

  EnableWindow_UBootGroup(hDlgFmt, lpCurrentItem->bIsImg);

  UpdateDataView_WHWH(hDlgFmt);
  if (lpCurrentItem->bIsImg) UpdateDataView_UIMG(hDlgFmt);

  return TRUE;
}

static void SaveHeader_WHWH(HWND hDlg)
{
  if (lpCurrentItem == NULL) return;

  CHAR chTmp[MAX_PATH] = { 0 };

  GetWindowTextA(GetDlgItem(hDlg, IDC_EDIT_WHVER), chTmp, MAX_PATH);
  if (strlen(chTmp) != 0) strcpy_s(lpCurrentItem->hdrWHWH.chItemVersion, chTmp);

  GetWindowTextA(GetDlgItem(hDlg, IDC_EDIT_WHTIME), chTmp, MAX_PATH);
  if (strlen(chTmp) != 0) lpCurrentItem->hdrWHWH.u32Time = ScanfHex(chTmp);

  lpCurrentItem->hdrWHWH.u32Type = (HW_ItemType)ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_CB_WHTYPE));
}

static void SaveHeader_UIMG(HWND hDlg)
{
  if (lpCurrentItem == NULL) return;
  if (lpCurrentItem->bIsImg == FALSE) return;

  CHAR chTmp[MAX_PATH] = { 0 };

  GetWindowTextA(GetDlgItem(hDlg, IDC_EDIT_UBTIME), chTmp, MAX_PATH);
  if (strlen(chTmp) != 0) lpCurrentItem->hdrUIMG.ih_time = BigLittleSwap32(ScanfHex(chTmp));

  GetWindowTextA(GetDlgItem(hDlg, IDC_EDIT_UBLOAD), chTmp, MAX_PATH);
  if (strlen(chTmp) != 0) lpCurrentItem->hdrUIMG.ih_load = BigLittleSwap32(ScanfHex(chTmp));

  GetWindowTextA(GetDlgItem(hDlg, IDC_EDIT_UBEP), chTmp, MAX_PATH);
  if (strlen(chTmp) != 0) lpCurrentItem->hdrUIMG.ih_ep = BigLittleSwap32(ScanfHex(chTmp));

  GetWindowTextA(GetDlgItem(hDlg, IDC_EDIT_UBNAME), chTmp, MAX_PATH);
  if (strlen(chTmp) != 0) strcpy_s(lpCurrentItem->hdrUIMG.ih_name, chTmp);

  lpCurrentItem->hdrUIMG.ih_os = (IH_OS)ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_CB_UBOS));
  lpCurrentItem->hdrUIMG.ih_arch = (IH_ARCH)ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_CB_UBARCH));
  lpCurrentItem->hdrUIMG.ih_type = (IH_TYPE)ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_CB_UBIMG));
  lpCurrentItem->hdrUIMG.ih_comp = (IH_COMP)ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_CB_UBCOMP));
}

static void BtnSave_WHWH(HWND hDlg)
{
  if (lpCurrentItem == NULL) return;
  /*
  if (lpData_WHWH == NULL) return;

  DWORD dwSize = sizeof(HW_HDR) + hdrWHWH.u32RearSize;
  LPVOID lpData = malloc(dwSize);
  int nResult;

  if (lpData == NULL)
  {
    SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"保存WHWH数据失败,内存不足!");
    return;
  }

  SaveHeader_WHWH(hDlg);

  //hdrWHWH.u32RearCRC = crc32_fast(lpData_WHWH, hdrWHWH.u32RearSize);

  memcpy_s(lpData, dwSize, &hdrWHWH, sizeof(HW_HDR));
  memcpy_s(MakePointer32(lpData, sizeof(HW_HDR)), dwSize - sizeof(HW_HDR), lpData_WHWH, hdrWHWH.u32RearSize);

  nResult = HWNP_SetItemData(u32ItemIdx, lpData, dwSize);
  free(lpData);

  if (nResult != 0)
    SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"保存WHWH数据失败,错误码:[%d]!", nResult);
  else
    SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"保存WHWH数据完成.");
  */
}

static void BtnSave_UIMG(HWND hDlg)
{
  /*
  if (bIsImg && lpData_UIMG)
  {
    DWORD dwSize = sizeof(HW_HDR) + sizeof(UIMG_HDR) + BigLittleSwap32(hdrUIMG.ih_size);
    LPVOID lpData = malloc(dwSize);
    int nResult;

    if (lpData == NULL)
    {
      SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"保存UIMG数据失败,内存不足!");
      return;
    }

    //SaveHeader_WHWH(hDlg);
    SaveHeader_UIMG(hDlg);

    //hdrUIMG.ih_dcrc = BigLittleSwap32(crc32_fast(lpData_UIMG, BigLittleSwap32(hdrUIMG.ih_size)));

    {
      UIMG_HDR hdrTmp;

      hdrTmp = hdrUIMG;
      hdrTmp.ih_hcrc = 0;
      hdrUIMG.ih_hcrc = BigLittleSwap32(crc32_fast(&hdrTmp, sizeof(UIMG_HDR)));
    }

    memcpy_s(MakePointer32(lpData, sizeof(HW_HDR)), dwSize - sizeof(HW_HDR), &hdrUIMG, sizeof(UIMG_HDR));
    memcpy_s(MakePointer32(lpData, sizeof(HW_HDR) + sizeof(UIMG_HDR)), dwSize - (sizeof(HW_HDR) + sizeof(UIMG_HDR)), lpData_UIMG, BigLittleSwap32(hdrUIMG.ih_size));

    hdrWHWH.u32RearSize = sizeof(UIMG_HDR) + BigLittleSwap32(hdrUIMG.ih_size);
    hdrWHWH.u32RearCRC = crc32_fast(MakePointer32(lpData, sizeof(HW_HDR)), hdrWHWH.u32RearSize);

    memcpy_s(lpData, dwSize, &hdrWHWH, sizeof(HW_HDR));

    nResult = HWNP_SetItemData(u32ItemIdx, lpData, dwSize);
    free(lpData);

    if (nResult != 0)
      SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"保存UIMG数据失败,错误码:[%d]!", nResult);
    else
      SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"保存UIMG数据完成.");
  }
  */
}

/************************************************************************/
/* 枚举WHWH子项目的数量                                                 */
/************************************************************************/
static uint32_t EnumSubItem() {
  const BYTE *lpData = (const BYTE*)lpItemData;
  const HW_HDR *lpHeader;
  uint32_t result = 0;
  uint32_t offset = 0;

  if (lpData == NULL) return result;

  //判断头长度是否满足
  while (offset + sizeof(HW_HDR) < u32DataSize) {
    lpHeader = (const PHW_HDR)(&lpData[offset]);

    if (lpHeader->u32Magic != HWNP_WHWH_MAGIC) break;

    result++;

    offset += alignPage(sizeof(HW_HDR) + lpHeader->u32RearSize);
  }

  return result;
}

/************************************************************************/
/* 初始化每个子项目元素                                                 */
/************************************************************************/
static uint32_t InitSubItemList() {
  const BYTE *lpData = (const BYTE*)lpItemData;
  const HW_HDR *lpHwHdr;
  const UIMG_HDR *lpImgHdr;
  uint32_t offset = 0, index = 0;
  uint32_t size;

  // 循环初始化每个子项目
  while (offset + sizeof(HW_HDR) < u32DataSize && index < nSubItem) {
#define CURRENT     (lpSubItem[index])
    lpHwHdr = (PHW_HDR)(&lpData[offset]);

    if (lpHwHdr->u32Magic != HWNP_WHWH_MAGIC) break;

    CURRENT.hdrWHWH = *lpHwHdr;
    CURRENT.bIsImg = FALSE;

    lpImgHdr = NULL;

    // 判断是否UImage
    if (lpHwHdr->u32RearSize >= sizeof(UIMG_HDR)) {
      lpImgHdr = (PUIMG_HDR)(MakePointer32(lpHwHdr, sizeof(HW_HDR)));

      if (lpImgHdr->ih_magic == IH_MAGIC_LE) {
        size = BigLittleSwap32(lpImgHdr->ih_size);

        // 判断UImage Data长度 + UImage Header长度是否越界
        if (lpHwHdr->u32RearSize - sizeof(UIMG_HDR) >= size) {
          CURRENT.bIsImg = TRUE;
        }
      }
    }

    // 复制HWHW数据
    // memcpy_s(CURRENT.lpData, lpHwHdr->u32RearSize, MakePointer32(lpHwHdr, sizeof(HW_HDR)), lpHwHdr->u32RearSize);

    CURRENT.lpcData = MakePointer32(lpHwHdr, sizeof(HW_HDR));

    // 如果存在UImage
    if (CURRENT.bIsImg && lpImgHdr) {
      CURRENT.hdrUIMG = *lpImgHdr;

      CURRENT.lpcImageData = MakePointer32(CURRENT.lpcData, sizeof(UIMG_HDR));
    }

    CURRENT.bIsInit = TRUE;
    index++;
    offset += alignPage(sizeof(HW_HDR) + lpHwHdr->u32RearSize);
#undef CURRENT
  }

  return index;
}

static uint32_t UpdateSubItemList() {
  uint32_t index = 0;
  TVINSERTSTRUCTA tvis;

  TreeView_DeleteAllItems(GetDlgItem(hDlgFmt, IDC_TV_SUBITEM));

  ZeroMemory(&tvis, sizeof(TVINSERTSTRUCTA));
  tvis.hParent = TVI_ROOT;
  tvis.hInsertAfter = TVI_LAST;
  tvis.item.mask = TVIF_TEXT | TVIF_PARAM;

  for (index = 0; index < nSubItem; index++) {
#define CURRENT     (lpSubItem[index])
    if (CURRENT.bIsInit == FALSE) break;

    tvis.item.pszText = CURRENT.hdrWHWH.chItemVersion;
    tvis.item.lParam = (LPARAM)index;
    TreeView_DlgInsertItemA(hDlgFmt, IDC_TV_SUBITEM, &tvis);

#undef CURRENT
  }

 

  return 0;

}

static int InitSubItem() {
  uint32_t size;

  if (lpItemData == NULL) return -1;

  size = EnumSubItem();

  if (size == 0) return -2;

  nSubItem = size;
  lpSubItem = (PSUBITEM_OBJ)calloc(size, sizeof(SUBITEM_OBJ));
  lpCurrentItem = &lpSubItem[0];

  InitSubItemList();
  UpdateSubItemList();

  return 0;
}

static INT_PTR InitDlg(HWND hDlg, uint32_t nIndex) {
  DWORD dwType;
  int ret;

  Release();

  ret = HWNP_GetItemDataSizeByIndex(nIndex, &u32DataSize);

  if (ret != 0) {
    SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"获取项目数据大小失败:[%d].", ret);
    return TRUE;
  }


  ret = HWNP_GetItemDataPointerByIndex(nIndex, &lpItemData);

  if (ret != 0) {
    SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"获取项目数据偏移失败:[%d].", ret);
    return TRUE;
  }


  ret = HWNP_GetItemDataTypeByIndex(nIndex, &dwType);

  if (ret != 0) {
    SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"获取项目数据类型失败:[%d].", ret);
    return TRUE;
  }


  if (!(CHK_FLAGS(dwType, IDT_WHWH))) {
    SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"项目数据类型不匹配!");
    return TRUE;
  }

  if (u32DataSize <= sizeof(HW_HDR)) {
    SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"项目数据长度太小!");
    return TRUE;
  }

  if (lpItemData == NULL) {
    SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"项目数据指针不合法!");
    return TRUE;
  }

  ret = InitSubItem();

  if (ret != 0) {
    SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"解析子项目失败:[%d]!", ret);
    return TRUE;
  }

  u32ItemIdx = nIndex;


  /*
  if (CHK_FLAGS(dwType, IDT_UBOOT))
    blUIMG = TRUE;
  else
    blUIMG = FALSE;
    */

  SNDMSG(GetDlgItem(hDlg, IDC_EDIT_WHVER), EM_SETLIMITTEXT, sizeof(HW_HEADER::chItemVersion), 0);
  SNDMSG(GetDlgItem(hDlg, IDC_EDIT_WHTIME), EM_SETLIMITTEXT, 10, 0);
  SNDMSG(GetDlgItem(hDlg, IDC_EDIT_UBTIME), EM_SETLIMITTEXT, 10, 0);
  SNDMSG(GetDlgItem(hDlg, IDC_EDIT_UBLOAD), EM_SETLIMITTEXT, 10, 0);
  SNDMSG(GetDlgItem(hDlg, IDC_EDIT_UBEP), EM_SETLIMITTEXT, 10, 0);
  SNDMSG(GetDlgItem(hDlg, IDC_EDIT_UBNAME), EM_SETLIMITTEXT, IH_NMLEN, 0);

  for (int i = 0; i < HW_ItemType::hwType_Limit; i++) {
    ComboBox_InsertStringA(GetDlgItem(hDlg, IDC_CB_WHTYPE), i, HW_ItemType_Text[i]);
  }

  ComboBox_InsertStringA(GetDlgItem(hDlg, IDC_CB_ALIGN), 0, "None");
  ComboBox_InsertStringA(GetDlgItem(hDlg, IDC_CB_ALIGN), 1, "Margin");
  ComboBox_InsertStringA(GetDlgItem(hDlg, IDC_CB_ALIGN), 2, "Padding");

  for (DWORD i = 0; i < IH_OS::IH_OS_COUNT; i++)
  {
    ComboBox_InsertStringA(GetDlgItem(hDlg, IDC_CB_UBOS), i, enum_IH_OS[i]);
  }

  for (DWORD i = 0; i < IH_ARCH::IH_ARCH_COUNT; i++)
  {
    ComboBox_InsertStringA(GetDlgItem(hDlg, IDC_CB_UBARCH), i, enum_IH_ARCH[i]);
  }

  for (DWORD i = 0; i < IH_TYPE::IH_TYPE_COUNT; i++)
  {
    ComboBox_InsertStringA(GetDlgItem(hDlg, IDC_CB_UBIMG), i, enum_IH_TYPE[i]);
  }

  for (DWORD i = 0; i < IH_COMP::IH_COMP_COUNT; i++)
  {
    ComboBox_InsertStringA(GetDlgItem(hDlg, IDC_CB_UBCOMP), i, enum_IH_COMP[i]);
  }

  if (UpdateDataView() == FALSE)
  {
    Release();
    MessageBoxW(hDlg, L"解析高级数据结构出现错误!", L"警告", MB_ICONWARNING | MB_OK);
    EndDialog(hDlg, 0);
    return (INT_PTR)FALSE;
  }

  /*
  if (u32DataSize < sizeof(HW_HDR) + hdrWHWH.u32RearSize)
  {
    //EnableWindow(hDlg, FALSE);
    MessageBoxW(hDlg, L"WHWH数据长度大于项目数据长度!", L"错误", MB_ICONERROR | MB_OK);
    return (INT_PTR)TRUE;
  }
  */

  return (INT_PTR)TRUE;

}

void SubItemChanged(LPNMTREEVIEW lpnmTV)
{
  uint32_t index = (uint32_t)lpnmTV->itemNew.lParam;

  if (index < nSubItem) {
    lpCurrentItem = &lpSubItem[index];
  }
  else {
    lpCurrentItem = NULL;
  }

  UpdateDataView();
}

/*
  Advanced data format dialog proc
*/
INT_PTR CALLBACK DlgProc_AdvDatFmt(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
  case WM_INITDIALOG:
    hDlgFmt = hDlg;
    return InitDlg(hDlg, (uint32_t)lParam);

  case WM_SHOWWINDOW:
    if ((wParam) && (lParam == SW_PARENTOPENING)) UpdateDataView();
    break;

  case WM_COMMAND:
  {
    WORD wId = LOWORD(wParam), wNc = HIWORD(wParam);

    if (lParam != 0)
    {
      switch (wId)
      {
      case IDC_BTN_WHCHK:   //检查WHWH数据CRC32
      {
        uint32_t u32CRC;

        if (lpCurrentItem) {
          u32CRC = crc32_fast(lpCurrentItem->lpcData, lpCurrentItem->hdrWHWH.u32RearSize);

          if (u32CRC == lpCurrentItem->hdrWHWH.u32RearCRC)
            SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"WHWH数据CRC32检查正确.");
          else
            SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"WHWH数据CRC32检查不正确, 计算结果:0x%08X!", u32CRC);
        }
        else {
          SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"无效的子项目!");
        }

      }
      break;

      case IDC_BTN_UBCH:    //检查UIMG头部CRC32
        if (lpCurrentItem && lpCurrentItem->bIsImg)
        {
          uint32_t u32CRC;
          UIMG_HDR hdrTmp;

          hdrTmp = lpCurrentItem->hdrUIMG;
          hdrTmp.ih_hcrc = 0;

          u32CRC = crc32_fast(&hdrTmp, sizeof(UIMG_HDR));

          if (u32CRC == BigLittleSwap32(lpCurrentItem->hdrUIMG.ih_hcrc))
            SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"UIMG头部CRC32检查正确.");
          else
            SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"UIMG头部CRC32检查不正确, 计算结果:%08X (LE)!", u32CRC);
        }
        break;

      case IDC_BTN_UBCD:    //检查UIMG数据CRC32
        if (lpCurrentItem && lpCurrentItem->bIsImg)
        {
          uint32_t u32CRC;

          u32CRC = crc32_fast(lpCurrentItem->lpcImageData, BigLittleSwap32(lpCurrentItem->hdrUIMG.ih_size));

          if (u32CRC == BigLittleSwap32(lpCurrentItem->hdrUIMG.ih_dcrc))
            SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"UIMG数据CRC32检查正确.");
          else
            SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"UIMG数据CRC32检查不正确, 计算结果:%08X (LE)!", u32CRC);
        }
        break;

      case IDC_BTN_WHEXP:   //导出WHWH数据
      {
        if (lpCurrentItem) {
          WCHAR wsTmp[MAX_PATH] = { 0 };

          if (GetSaveFilePath(hDlg, wsTmp, MAX_PATH))
          {
            if (ExportToFile(wsTmp, lpCurrentItem->lpcData, lpCurrentItem->hdrWHWH.u32RearSize))
              SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"导出WHWH数据完成.");
            else
              SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"导出WHWH数据失败,错误码:[%d]!", GetLastError());
          }
        }
      }
      break;

      case IDC_BTN_WHIMP:   //导入WHWH数据
      {
        if (lpCurrentItem) {

          LPVOID lpData;
          DWORD dwDataSize;
          WCHAR wsTmp[MAX_PATH] = { 0 };

          if (GetOpenFilePath(hDlg, wsTmp, MAX_PATH))
          {
            if (ImportFromFile(wsTmp, &lpData, &dwDataSize) == FALSE)
            {
              SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"打开文件失败,错误码:[%d]!", GetLastError());
              break;
            }

            SaveHeader_WHWH(hDlg);

            if (lpCurrentItem->lpData) free(lpCurrentItem->lpData);

            lpCurrentItem->lpData = lpData;
            lpCurrentItem->hdrWHWH.u32RearSize = dwDataSize;
            lpCurrentItem->hdrWHWH.u32RearCRC = crc32_fast(lpData, dwDataSize);

            SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"导入WHWH数据完成.");
            UpdateDataView();
          }
        }
      }
      break;

      case IDC_BTN_UBEXP:   //导出UIMG数据
        if (lpCurrentItem && lpCurrentItem->bIsImg)
        {
          WCHAR wsTmp[MAX_PATH] = { 0 };

          if (GetSaveFilePath(hDlg, wsTmp, MAX_PATH))
          {
            if (ExportToFile(wsTmp, lpCurrentItem->lpcImageData, BigLittleSwap32(lpCurrentItem->hdrUIMG.ih_size)))
              SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"导出UIMG数据完成.");
            else
              SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"导出UIMG数据失败,错误码:[%d]!", GetLastError());
          }
        }
        break;

      case IDC_BTN_UBIMP:   //导入UIMG数据
        if (lpCurrentItem && lpCurrentItem->bIsImg)
        {
          LPVOID lpData;
          DWORD dwDataSize;
          WCHAR wsTmp[MAX_PATH] = { 0 };

          /*
          if (GetOpenFilePath(hDlg, wsTmp, MAX_PATH))
          {
            if (ImportFromFile(wsTmp, &lpData, &dwDataSize) == FALSE)
            {
              SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"打开文件失败,错误码:[%d]!", GetLastError());
              break;
            }

            SaveHeader_WHWH(hDlg);
            SaveHeader_UIMG(hDlg);


            lpCurrentItem->lpImageData = lpData;

            hdrUIMG.ih_size = BigLittleSwap32(dwDataSize);
            hdrUIMG.ih_dcrc = BigLittleSwap32(crc32_fast(lpData, dwDataSize));

            {
              UIMG_HDR hdrTmp;

              hdrTmp = hdrUIMG;
              hdrTmp.ih_hcrc = 0;
              hdrUIMG.ih_hcrc = BigLittleSwap32(crc32_fast(&hdrTmp, sizeof(UIMG_HDR)));
            }

            //同步WHWH数据
            dwDataSize += sizeof(UIMG_HDR);
            lpData = malloc(dwDataSize);
            if (lpData == NULL)
            {
              SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"同步WHWH数据失败,内存不足!");
              UpdateDataView(hDlg);
              break;
            }

            memcpy_s(lpData, dwDataSize, &hdrUIMG, sizeof(UIMG_HDR));
            memcpy_s(MakePointer32(lpData, sizeof(UIMG_HDR)), dwDataSize - sizeof(UIMG_HDR), lpData_UIMG, BigLittleSwap32(hdrUIMG.ih_size));

            if (lpData_WHWH) free(lpData_WHWH);
            lpData_WHWH = lpData;
            hdrWHWH.u32RearSize = dwDataSize;
            hdrWHWH.u32RearCRC = crc32_fast(lpData, dwDataSize);


            SetTooltip(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"导入UIMG数据完成.");
            UpdateDataView(hDlg);

           }
          */
        }
        break;

      case IDC_BTN_WHSAVE:    //保存WHWH 头部 + 数据
        BtnSave_WHWH(hDlg);
        break;

      case IDC_BTN_UBSAVE:    //保存UIMG 头部 + 数据
        BtnSave_UIMG(hDlg);
        break;

      case IDC_BTN_ADF_SAVE:    //保存WHWH 头部 + UIMG 头部 + 数据
      {
        if (lpCurrentItem && lpCurrentItem->bIsImg && lpCurrentItem->lpImageData)
        {
          //SNDMSG(hDlg, WM_COMMAND, MAKEWPARAM(IDC_BTN_WHSAVE, BN_CLICKED), NULL);
          SaveHeader_WHWH(hDlg);
          BtnSave_UIMG(hDlg);
        }
        else
        {
          BtnSave_WHWH(hDlg);
        }

        Release();
        EndDialog(hDlg, IDOK);
      }
      break;

      case IDC_BTN_ADF_BACK:    //返回上级窗口
      {
        Release();
        EndDialog(hDlg, IDCANCEL);
      }
      break;
      }

    }
    break;
  }

  case WM_NOTIFY:
  {
    if (lParam == 0) break;

    LPNMHDR lpNm = (LPNMHDR)lParam;

    switch (lpNm->code)
    {
    case TVN_SELCHANGED: {
      SubItemChanged((LPNMTREEVIEW)lParam);
      break;
    }
    case NM_CUSTOMDRAW: {
      if (wParam == IDC_TV_SUBITEM)
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
      break;
    }
    }

  }
  break;

  case WM_CLOSE:
    Release();
    hDlgFmt = NULL;
    EndDialog(hDlg, IDCLOSE);
    break;
  }

  return (INT_PTR)FALSE;
}
