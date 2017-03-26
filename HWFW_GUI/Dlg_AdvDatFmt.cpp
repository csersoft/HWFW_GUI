#include "stdafx.h"
#include "HWFW_GUI.h"

static uint32_t u32ItemIdx;
static LPCVOID lpItemData = NULL;
static uint32_t u32DataSize = 0;
static BOOL blUIMG;
static WHWH_HDR hdrWHWH;
static UIMG_HDR hdrUIMG;
static LPVOID lpData_WHWH = NULL;
static LPVOID lpData_UIMG = NULL;

static void Release()
{
  lpItemData = NULL;
  u32DataSize = 0;

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

static void UpdateView_WHWH(HWND hDlg)
{
  size_t stOut;
  tm _tm;
  WCHAR wsTemp[260];

  mbstowcs_s(&stOut, wsTemp, hdrWHWH.chItemVersion, sizeof(WHWH_HDR::chItemVersion));
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_WHVER), wsTemp);

  swprintf_s(wsTemp, SF_HEX, hdrWHWH.u32Time);
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_WHTIME), wsTemp);

  _localtime32_s(&_tm, &hdrWHWH.u32Time);
  swprintf_s(wsTemp, L"%04u/%02u/%02u %02u:%02u:%02u", _tm.tm_year + 1900, _tm.tm_mon + 1, _tm.tm_mday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec);
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_WHTIME_RO), wsTemp);

  if ((hdrWHWH.enumType >= 1) && (hdrWHWH.enumType <= 4))
    ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_CB_WHTYPE), hdrWHWH.enumType);
  else
    ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_CB_WHTYPE), 0);

  swprintf_s(wsTemp, SF_HEX, hdrWHWH.u32RearSize);
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_WHSIZE), wsTemp);

  swprintf_s(wsTemp, SF_HEX, hdrWHWH.u32RearCRC);
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_WHCRC), wsTemp);
}

static void UpdateView_UIMG(HWND hDlg)
{
  size_t stOut;
  __time32_t _time32;
  tm _tm;
  WCHAR wsTemp[260];

  swprintf_s(wsTemp, SF_HEX, BigLittleSwap32(hdrUIMG.ih_hcrc));
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_UBHDRCRC), wsTemp);

  swprintf_s(wsTemp, SF_HEX, BigLittleSwap32(hdrUIMG.ih_dcrc));
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_UBDATCRC), wsTemp);

  swprintf_s(wsTemp, SF_HEX, BigLittleSwap32(hdrUIMG.ih_size));
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_UBSIZE), wsTemp);

  _time32 = BigLittleSwap32(hdrUIMG.ih_time);
  swprintf_s(wsTemp, SF_HEX, _time32);
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_UBTIME), wsTemp);

  _localtime32_s(&_tm, &_time32);
  swprintf_s(wsTemp, L"%04u/%02u/%02u %02u:%02u:%02u", _tm.tm_year + 1900, _tm.tm_mon + 1, _tm.tm_mday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec);
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_UBTIME_RO), wsTemp);

  mbstowcs_s(&stOut, wsTemp, hdrUIMG.ih_name, sizeof(UIMG_HDR::ih_name));
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_UBNAME), wsTemp);

  swprintf_s(wsTemp, SF_HEX, BigLittleSwap32(hdrUIMG.ih_load));
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_UBLOAD), wsTemp);

  swprintf_s(wsTemp, SF_HEX, BigLittleSwap32(hdrUIMG.ih_ep));
  SetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_UBEP), wsTemp);

  if (hdrUIMG.ih_os < IH_OS::IH_OS_COUNT)
    ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_CB_UBOS), hdrUIMG.ih_os);
  else
    ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_CB_UBOS), 0);

  if (hdrUIMG.ih_arch < IH_ARCH::IH_ARCH_COUNT)
    ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_CB_UBARCH), hdrUIMG.ih_arch);
  else
    ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_CB_UBARCH), 0);

  if (hdrUIMG.ih_type < IH_TYPE::IH_TYPE_COUNT)
    ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_CB_UBIMG), hdrUIMG.ih_type);
  else
    ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_CB_UBIMG), 0);

  if (hdrUIMG.ih_comp < IH_COMP::IH_COMP_COUNT)
    ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_CB_UBCOMP), hdrUIMG.ih_comp);
  else
    ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_CB_UBCOMP), 0);
}

static BOOL UpdateView(HWND hDlg)
{
  if (lpData_WHWH == NULL)
  {
    hdrWHWH = *(PWHWH_HDR)lpItemData;

    if (hdrWHWH.u32Magic != HWNP_WHWH_MAGIC) return FALSE;

    lpData_WHWH = malloc(hdrWHWH.u32RearSize);
    if (lpData_WHWH == NULL) return FALSE;
    memcpy_s(lpData_WHWH, hdrWHWH.u32RearSize, MakePointer32(lpItemData, sizeof(WHWH_HDR)), hdrWHWH.u32RearSize);
  }

  if (hdrWHWH.u32RearSize >= sizeof(UIMG_HDR))
  {
    hdrUIMG = *(PUIMG_HDR)(lpData_WHWH);

    if (hdrUIMG.ih_magic == IH_MAGIC_LE)
    {
      if (hdrWHWH.u32RearSize - sizeof(UIMG_HDR) >= BigLittleSwap32(hdrUIMG.ih_size))
      {
        blUIMG = TRUE;
      }
      else
      {
        blUIMG = FALSE;
        MessageBoxW(hDlg, L"UIMG数据长度大于项目数据长度!", L"警告", MB_ICONWARNING | MB_OK);
      }

      /*
      if (u32DataSize - (sizeof(WHWH_HDR) + sizeof(UIMG_HDR)) >= BigLittleSwap32(hdrUIMG.ih_size))
      {
        blUIMG = TRUE;
      }
      else
      {
        blUIMG = FALSE;
        MessageBoxW(hDlg, L"UIMG数据长度大于项目数据长度!", L"警告", MB_ICONWARNING | MB_OK);
      }
      */
    }
    else
      blUIMG = FALSE;
  }
  else
    blUIMG = FALSE;

  EnableWindow_UBootGroup(hDlg, blUIMG);

  if ((blUIMG) && (lpData_UIMG == NULL))
  {
    lpData_UIMG = malloc(BigLittleSwap32(hdrUIMG.ih_size));

    if (lpData_UIMG == NULL)
    {
      blUIMG = FALSE;
      EnableWindow_UBootGroup(hDlg, blUIMG);
    }
    else
    {
      memcpy_s(lpData_UIMG, BigLittleSwap32(hdrUIMG.ih_size), MakePointer32(lpItemData, sizeof(WHWH_HDR) + sizeof(UIMG_HDR)), BigLittleSwap32(hdrUIMG.ih_size));
    }
  }

  UpdateView_WHWH(hDlg);
  if (blUIMG) UpdateView_UIMG(hDlg);

  return TRUE;
}

static void SaveHeader_WHWH(HWND hDlg)
{
  CHAR chTmp[MAX_PATH] = { 0 };

  GetWindowTextA(GetDlgItem(hDlg, IDC_EDIT_WHVER), chTmp, MAX_PATH);
  if (strlen(chTmp) != 0) strcpy_s(hdrWHWH.chItemVersion, chTmp);

  GetWindowTextA(GetDlgItem(hDlg, IDC_EDIT_WHTIME), chTmp, MAX_PATH);
  if (strlen(chTmp) != 0) hdrWHWH.u32Time = ScanfHex(chTmp);

  hdrWHWH.enumType = (_WHHDR_ItemType)ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_CB_WHTYPE));

}

static void SaveHeader_UIMG(HWND hDlg)
{
  CHAR chTmp[MAX_PATH] = { 0 };

  GetWindowTextA(GetDlgItem(hDlg, IDC_EDIT_UBTIME), chTmp, MAX_PATH);
  if (strlen(chTmp) != 0) hdrUIMG.ih_time = BigLittleSwap32(ScanfHex(chTmp));

  GetWindowTextA(GetDlgItem(hDlg, IDC_EDIT_UBLOAD), chTmp, MAX_PATH);
  if (strlen(chTmp) != 0) hdrUIMG.ih_load = BigLittleSwap32(ScanfHex(chTmp));

  GetWindowTextA(GetDlgItem(hDlg, IDC_EDIT_UBEP), chTmp, MAX_PATH);
  if (strlen(chTmp) != 0) hdrUIMG.ih_ep = BigLittleSwap32(ScanfHex(chTmp));

  GetWindowTextA(GetDlgItem(hDlg, IDC_EDIT_UBNAME), chTmp, MAX_PATH);
  if (strlen(chTmp) != 0) strcpy_s(hdrUIMG.ih_name, chTmp);

  hdrUIMG.ih_os = (IH_OS)ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_CB_UBOS));
  hdrUIMG.ih_arch = (IH_ARCH)ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_CB_UBARCH));
  hdrUIMG.ih_type = (IH_TYPE)ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_CB_UBIMG));
  hdrUIMG.ih_comp = (IH_COMP)ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_CB_UBCOMP));
}

static void BtnSave_WHWH(HWND hDlg)
{
  if (lpData_WHWH == NULL) return;

  DWORD dwSize = sizeof(WHWH_HDR) + hdrWHWH.u32RearSize;
  LPVOID lpData = malloc(dwSize);
  int nResult;

  if (lpData == NULL)
  {
    SetSubStatus(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"保存WHWH数据失败,内存不足!");
    return;
  }

  SaveHeader_WHWH(hDlg);

  //hdrWHWH.u32RearCRC = crc32_fast(lpData_WHWH, hdrWHWH.u32RearSize);

  memcpy_s(lpData, dwSize, &hdrWHWH, sizeof(WHWH_HDR));
  memcpy_s(MakePointer32(lpData, sizeof(WHWH_HDR)), dwSize - sizeof(WHWH_HDR), lpData_WHWH, hdrWHWH.u32RearSize);

  nResult = HWNP_SetItemData(u32ItemIdx, lpData, dwSize);
  free(lpData);

  if (nResult != 0)
    SetSubStatus(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"保存WHWH数据失败,错误码:[%d]!", nResult);
  else
    SetSubStatus(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"保存WHWH数据完成.");
}

static void BtnSave_UIMG(HWND hDlg)
{
  if (blUIMG && lpData_UIMG)
  {
    DWORD dwSize = sizeof(WHWH_HDR) + sizeof(UIMG_HDR) + BigLittleSwap32(hdrUIMG.ih_size);
    LPVOID lpData = malloc(dwSize);
    int nResult;

    if (lpData == NULL)
    {
      SetSubStatus(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"保存UIMG数据失败,内存不足!");
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

    memcpy_s(MakePointer32(lpData, sizeof(WHWH_HDR)), dwSize - sizeof(WHWH_HDR), &hdrUIMG, sizeof(UIMG_HDR));
    memcpy_s(MakePointer32(lpData, sizeof(WHWH_HDR) + sizeof(UIMG_HDR)), dwSize - (sizeof(WHWH_HDR) + sizeof(UIMG_HDR)), lpData_UIMG, BigLittleSwap32(hdrUIMG.ih_size));

    hdrWHWH.u32RearSize = sizeof(UIMG_HDR) + BigLittleSwap32(hdrUIMG.ih_size);
    hdrWHWH.u32RearCRC = crc32_fast(MakePointer32(lpData, sizeof(WHWH_HDR)), hdrWHWH.u32RearSize);

    memcpy_s(lpData, dwSize, &hdrWHWH, sizeof(WHWH_HDR));

    nResult = HWNP_SetItemData(u32ItemIdx, lpData, dwSize);
    free(lpData);

    if (nResult != 0)
      SetSubStatus(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"保存UIMG数据失败,错误码:[%d]!", nResult);
    else
      SetSubStatus(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"保存UIMG数据完成.");
  }
}

/*
Advanced data format dialog proc
*/
INT_PTR CALLBACK DlgProc_AdvDatFmt(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
  case WM_INITDIALOG:
  {
    DWORD dwType;

    Release();

    if ((HWNP_GetItemDataSizeByIndex((uint32_t)lParam, &u32DataSize) == 0) &&
      (HWNP_GetItemDataPointerByIndex((uint32_t)lParam, &lpItemData) == 0) &&
      (HWNP_GetItemDataTypeByIndex((uint32_t)lParam, &dwType) == 0) &&
      (CHK_FLAGS(dwType, IDT_WHWH)) &&
      (u32DataSize > sizeof(WHWH_HDR)) &&
      (lpItemData))
    {
      u32ItemIdx = (uint32_t)lParam;

      /*
      if (CHK_FLAGS(dwType, IDT_UBOOT))
        blUIMG = TRUE;
      else
        blUIMG = FALSE;
        */

      SNDMSG(GetDlgItem(hDlg, IDC_EDIT_WHVER), EM_SETLIMITTEXT, sizeof(WHWH_HEADER::chItemVersion), 0);
      SNDMSG(GetDlgItem(hDlg, IDC_EDIT_WHTIME), EM_SETLIMITTEXT, 10, 0);
      SNDMSG(GetDlgItem(hDlg, IDC_EDIT_UBTIME), EM_SETLIMITTEXT, 10, 0);
      SNDMSG(GetDlgItem(hDlg, IDC_EDIT_UBLOAD), EM_SETLIMITTEXT, 10, 0);
      SNDMSG(GetDlgItem(hDlg, IDC_EDIT_UBEP), EM_SETLIMITTEXT, 10, 0);
      SNDMSG(GetDlgItem(hDlg, IDC_EDIT_UBNAME), EM_SETLIMITTEXT, IH_NMLEN, 0);

      ComboBox_InsertStringA(GetDlgItem(hDlg, IDC_CB_WHTYPE), 0, "? Invalid");
      ComboBox_InsertStringA(GetDlgItem(hDlg, IDC_CB_WHTYPE), 1, "1 Kernel");
      ComboBox_InsertStringA(GetDlgItem(hDlg, IDC_CB_WHTYPE), 2, "2 RootFS");
      ComboBox_InsertStringA(GetDlgItem(hDlg, IDC_CB_WHTYPE), 3, "3 System");
      ComboBox_InsertStringA(GetDlgItem(hDlg, IDC_CB_WHTYPE), 4, "4 MiniSYS");

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

      if (u32DataSize < sizeof(WHWH_HDR) + hdrWHWH.u32RearSize)
      {
        //EnableWindow(hDlg, FALSE);
        MessageBoxW(hDlg, L"WHWH数据长度大于项目数据长度!", L"错误", MB_ICONERROR | MB_OK);
        return (INT_PTR)TRUE;
      }

      if (UpdateView(hDlg) == FALSE)
      {
        Release();
        MessageBoxW(hDlg, L"解析高级数据结构出现错误!", L"警告", MB_ICONWARNING | MB_OK);
        EndDialog(hDlg, 0);
        return (INT_PTR)FALSE;
      }

      return (INT_PTR)TRUE;
    }
  }
  break;

  case WM_SHOWWINDOW:
  {
    if ((wParam) && (lParam == SW_PARENTOPENING)) UpdateView(hDlg);
  }
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

        u32CRC = crc32_fast(lpData_WHWH, hdrWHWH.u32RearSize);

        if (u32CRC == hdrWHWH.u32RearCRC)
          SetSubStatus(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"WHWH数据CRC32检查正确.");
        else
          SetSubStatus(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"WHWH数据CRC32检查不正确, 计算结果:0x%08X!", u32CRC);
      }
      break;

      case IDC_BTN_UBCH:    //检查UIMG头部CRC32
      if (blUIMG)
      {
        uint32_t u32CRC;
        UIMG_HDR hdrTmp;

        hdrTmp = hdrUIMG;
        hdrTmp.ih_hcrc = 0;

        u32CRC = crc32_fast(&hdrTmp, sizeof(UIMG_HDR));

        if (u32CRC == BigLittleSwap32(hdrUIMG.ih_hcrc))
          SetSubStatus(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"UIMG头部CRC32检查正确.");
        else
          SetSubStatus(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"UIMG头部CRC32检查不正确, 计算结果:%08X (LE)!", u32CRC);
      }
      break;

      case IDC_BTN_UBCD:    //检查UIMG数据CRC32
      if (blUIMG)
      {
        uint32_t u32CRC;

        u32CRC = crc32_fast(lpData_UIMG, BigLittleSwap32(hdrUIMG.ih_size));

        if (u32CRC == BigLittleSwap32(hdrUIMG.ih_dcrc))
          SetSubStatus(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"UIMG数据CRC32检查正确.");
        else
          SetSubStatus(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"UIMG数据CRC32检查不正确, 计算结果:%08X (LE)!", u32CRC);
      }
      break;

      case IDC_BTN_WHEXP:   //导出WHWH数据
      {
        WCHAR wsTmp[MAX_PATH] = { 0 };

        if (GetSaveFilePath(hDlg, wsTmp, MAX_PATH))
        {
          if (ExportToFile(wsTmp, lpData_WHWH, hdrWHWH.u32RearSize))
            SetSubStatus(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"导出WHWH数据完成.");
          else
            SetSubStatus(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"导出WHWH数据失败,错误码:[%d]!", GetLastError());
        }
      }
      break;

      case IDC_BTN_WHIMP:   //导入WHWH数据
      {
        LPVOID lpData;
        DWORD dwDataSize;
        WCHAR wsTmp[MAX_PATH] = { 0 };

        if (GetOpenFilePath(hDlg, wsTmp, MAX_PATH))
        {
          if (ImportFromFile(wsTmp, &lpData, &dwDataSize) == FALSE)
          {
            SetSubStatus(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"打开文件失败,错误码:[%d]!", GetLastError());
            break;
          }

          SaveHeader_WHWH(hDlg);

          if (lpData_WHWH) free(lpData_WHWH);

          lpData_WHWH = lpData;
          hdrWHWH.u32RearSize = dwDataSize;
          hdrWHWH.u32RearCRC = crc32_fast(lpData, dwDataSize);

          SetSubStatus(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"导入WHWH数据完成.");
          UpdateView(hDlg);
        }
      }
      break;

      case IDC_BTN_UBEXP:   //导出UIMG数据
      if (blUIMG)
      {
        WCHAR wsTmp[MAX_PATH] = { 0 };

        if (GetSaveFilePath(hDlg, wsTmp, MAX_PATH))
        {
          if (ExportToFile(wsTmp, lpData_UIMG, BigLittleSwap32(hdrUIMG.ih_size)))
            SetSubStatus(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"导出UIMG数据完成.");
          else
            SetSubStatus(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"导出UIMG数据失败,错误码:[%d]!", GetLastError());
        }
      }
      break;

      case IDC_BTN_UBIMP:   //导入UIMG数据
      if (blUIMG)
      {
        LPVOID lpData;
        DWORD dwDataSize;
        WCHAR wsTmp[MAX_PATH] = { 0 };

        if (GetOpenFilePath(hDlg, wsTmp, MAX_PATH))
        {
          if (ImportFromFile(wsTmp, &lpData, &dwDataSize) == FALSE)
          {
            SetSubStatus(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"打开文件失败,错误码:[%d]!", GetLastError());
            break;
          }

          SaveHeader_WHWH(hDlg);
          SaveHeader_UIMG(hDlg);

          if (lpData_UIMG) free(lpData_UIMG);

          lpData_UIMG = lpData;

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
            SetSubStatus(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"同步WHWH数据失败,内存不足!");
            UpdateView(hDlg);
            break;
          }

          memcpy_s(lpData, dwDataSize, &hdrUIMG, sizeof(UIMG_HDR));
          memcpy_s(MakePointer32(lpData, sizeof(UIMG_HDR)), dwDataSize - sizeof(UIMG_HDR), lpData_UIMG, BigLittleSwap32(hdrUIMG.ih_size));

          if (lpData_WHWH) free(lpData_WHWH);
          lpData_WHWH = lpData;
          hdrWHWH.u32RearSize = dwDataSize;
          hdrWHWH.u32RearCRC = crc32_fast(lpData, dwDataSize);


          SetSubStatus(GetDlgItem(hDlg, IDC_LBL_ADF_STATUS), L"导入UIMG数据完成.");
          UpdateView(hDlg);
        }
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
        if (blUIMG && lpData_UIMG)
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
  }
  break;

  case WM_CLOSE:
  Release();
  EndDialog(hDlg, IDCLOSE);
  break;
  }

  return (INT_PTR)FALSE;
}