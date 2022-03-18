#include "stdafx.h"
#include "HWNP.h"


// internal item copy
typedef struct _Internal_ItemCopy
{
  LPVOID                  lpItemData;
  uint32_t                u32DataSize;
  HWNP_ITEMINFO           ItemInfo;
} INT_ITEMCOPY, *PINT_ITEMCOPY;


static HANDLE hFile = INVALID_HANDLE_VALUE;
static int nLastError = 0;
static int nState = 0;
static int nHeaderSizeType = 0;
static BOOL blChanged = FALSE;
static HWNP_HEADER hwHeader;
static LPCH lpProductList = NULL;
static uint32_t u32ItemCount = 0;
static PINT_ITEMCOPY lpItemCopy = NULL;


static inline void HWNP_SwapItemCopy(PINT_ITEMCOPY lpItemCopyA, PINT_ITEMCOPY lpItemCopyB)
{
  INT_ITEMCOPY ItemCopyTmp = *lpItemCopyB;

  *lpItemCopyB = *lpItemCopyA;
  *lpItemCopyA = ItemCopyTmp;
}

static inline void HWNP_UpdateItemInfo()
{
  if (nState != -1) return;

  uint32_t u32DataOffset = sizeof(HWNP_HEADER) + hwHeader.PacketHeader.u16ProductListSize + u32ItemCount * sizeof(HWNP_ITEMINFO);

  for (uint32_t u32Index = 0; u32Index < u32ItemCount; u32Index++)
  {
    lpItemCopy[u32Index].ItemInfo.u32Offset = u32DataOffset;
    lpItemCopy[u32Index].ItemInfo.u32Size = lpItemCopy[u32Index].u32DataSize;
    lpItemCopy[u32Index].ItemInfo.u32ItemCRC32 = crc32_fast(lpItemCopy[u32Index].lpItemData, lpItemCopy[u32Index].u32DataSize);

    u32DataOffset += lpItemCopy[u32Index].u32DataSize;
  }
}

static inline void HWNP_UpdateHeader()
{
  if (nState != -1) return;

  uint32_t u32DataSize = 0;
  uint32_t u32CRC, u32Tmp;

  for (uint32_t u32Index = 0; u32Index < u32ItemCount; u32Index++)
    u32DataSize += lpItemCopy[u32Index].u32DataSize;

  hwHeader.PacketHeader.u32ItemCount = u32ItemCount;
  hwHeader.PacketHeader.u16ItemInfoSize = sizeof(HWNP_ITEMINFO);
  
  //Calculate head size
  if (nHeaderSizeType == 1)
    hwHeader.FileHeader2.u32HeaderSize = hwHeader.PacketHeader.u16ProductListSize + u32ItemCount * sizeof(HWNP_ITEMINFO);
  else
    hwHeader.FileHeader2.u32HeaderSize = sizeof(HWNP_HEADER) + hwHeader.PacketHeader.u16ProductListSize + u32ItemCount * sizeof(HWNP_ITEMINFO);
  
  //Calculate header CRC
  u32CRC = crc32_fast(&hwHeader.PacketHeader, sizeof(HWNP_PAKHDR));
  u32CRC = crc32_fast(lpProductList, hwHeader.PacketHeader.u16ProductListSize, u32CRC);

  for (uint32_t u32Index = 0; u32Index < u32ItemCount; u32Index++)
    u32CRC = crc32_fast(&lpItemCopy[u32Index].ItemInfo, sizeof(HWNP_ITEMINFO), u32CRC);

  hwHeader.FileHeader2.u32HeaderCRC32 = u32CRC;

  //Calculate file size
  u32Tmp = (sizeof(HWNP_HEADER) + hwHeader.PacketHeader.u16ProductListSize + u32ItemCount * sizeof(HWNP_ITEMINFO) + u32DataSize) - 76;
  hwHeader.BasicFileHeader.beu32FileSize = EndianSwap32(u32Tmp);

  //Calculate file CRC
  u32CRC = crc32_fast(&hwHeader.FileHeader2, sizeof(HWNP_FILEHDR2));
  u32CRC = crc32_fast(&hwHeader.PacketHeader, sizeof(HWNP_PAKHDR), u32CRC);
  u32CRC = crc32_fast(lpProductList, hwHeader.PacketHeader.u16ProductListSize, u32CRC);

  for (uint32_t u32Index = 0; u32Index < u32ItemCount; u32Index++)
    u32CRC = crc32_fast(&lpItemCopy[u32Index].ItemInfo, sizeof(HWNP_ITEMINFO), u32CRC);

  for (uint32_t u32Index = 0; u32Index < u32ItemCount; u32Index++)
    u32CRC = crc32_fast(lpItemCopy[u32Index].lpItemData, lpItemCopy[u32Index].u32DataSize, u32CRC);

  hwHeader.BasicFileHeader.u32FileCRC32 = u32CRC;
}

BOOL HWNP_IsChanged()
{
  return blChanged;
}

int HWNP_GetLastError()
{
  return nLastError;
}

int HWNP_GetState()
{
  return nState;
}

int HWNP_GetHeaderSizeType()
{
  return nHeaderSizeType;
}

int HWNP_SetHeaderSizeType(__in int nNewType)
{
  int nRet = nHeaderSizeType;

  nHeaderSizeType = nNewType;
  return nRet;
}

void HWNP_Update()
{
  if (nState != -1) return;

  HWNP_UpdateItemInfo();
  HWNP_UpdateHeader();
  blChanged = TRUE;
}

void HWNP_Release()
{
  if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
  if (lpProductList) free(lpProductList);

  hFile = INVALID_HANDLE_VALUE;
  nLastError = 0;
  nState = 0;
  blChanged = FALSE;
  lpProductList = NULL;
  
  if (lpItemCopy)
  {
    for (uint32_t u32Index = 0; u32Index < u32ItemCount; u32Index++)
    {
      if (lpItemCopy[u32Index].lpItemData)
      {
        free(lpItemCopy[u32Index].lpItemData);
        lpItemCopy[u32Index].lpItemData = NULL;
      }
    }

    free(lpItemCopy);
    lpItemCopy = NULL;
  }

  u32ItemCount = 0;
}

int HWNP_OpenFirmware(__in LPCWSTR lpFilePath)
{
  LPBYTE lpFile;
  PHWNP_ITEMINFO lpItemInfo;
  DWORD dwFileSize, dwTmp;

  HWNP_Release();

  hFile = CreateFileW(lpFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE) return (nLastError = -1);
  
  nState = 1;

  dwFileSize = GetFileSize(hFile, &dwTmp);
  //Do not process files larger than 1GB
  if (dwTmp != 0 || dwFileSize > 0x40000000U) return (nLastError = -2);
  if (dwFileSize < sizeof(HWNP_HEADER)) return (nLastError = -3);

  lpFile = (LPBYTE)malloc(dwFileSize);
  if (lpFile == NULL) return (nLastError = -4);

  SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

  if (ReadFile(hFile, lpFile, dwFileSize, &dwTmp, NULL) == FALSE) return (nLastError = -3);
  if (dwTmp != dwFileSize) return (nLastError = -5);

  nState = 2;

  //get file header
  memcpy_s(&hwHeader, sizeof(HWNP_HEADER), lpFile, sizeof(HWNP_HEADER));
  if (hwHeader.BasicFileHeader.u32Magic != HWNP_HEADER_MAGIC) return (nLastError = -6);

  //Get Product Support List
  if (CHK_OUT_OF_UBOUND2(lpFile + sizeof(HWNP_HEADER), hwHeader.PacketHeader.u16ProductListSize, lpFile, dwFileSize)) return (nLastError = -7);
  lpProductList = (LPCH)malloc(hwHeader.PacketHeader.u16ProductListSize);
  if (lpProductList == NULL) return (nLastError = -8);

  memcpy_s(lpProductList, hwHeader.PacketHeader.u16ProductListSize, lpFile + sizeof(HWNP_HEADER), hwHeader.PacketHeader.u16ProductListSize);

  //get item
  lpItemInfo = (PHWNP_ITEMINFO)(lpFile + sizeof(HWNP_HEADER) + hwHeader.PacketHeader.u16ProductListSize);
  u32ItemCount = hwHeader.PacketHeader.u32ItemCount;
  lpItemCopy = (PINT_ITEMCOPY)calloc(u32ItemCount, sizeof(INT_ITEMCOPY));
  if (lpItemCopy == NULL) return (nLastError = -9);

  for (uint32_t u32Index = 0; u32Index < u32ItemCount; u32Index++)
  {
#define _ITEMINFO_      lpItemInfo[u32Index]
#define _ITEMCOPY_      lpItemCopy[u32Index]

    if (CHK_OUT_OF_UBOUND2(&_ITEMINFO_, sizeof(HWNP_ITEMINFO), lpFile, dwFileSize)) return (nLastError = -10);
    if (CHK_OUT_OF_UBOUND2(lpFile + _ITEMINFO_.u32Offset, _ITEMINFO_.u32Size, lpFile, dwFileSize)) return (nLastError = -11);

    _ITEMCOPY_.lpItemData = malloc(_ITEMINFO_.u32Size);
    if (_ITEMCOPY_.lpItemData == NULL) return (nLastError = -12);

    memcpy_s(_ITEMCOPY_.lpItemData, _ITEMINFO_.u32Size, lpFile + _ITEMINFO_.u32Offset, _ITEMINFO_.u32Size);
    _ITEMCOPY_.u32DataSize = _ITEMINFO_.u32Size;
    _ITEMCOPY_.ItemInfo = _ITEMINFO_;

#undef _ITEMCOPY_
#undef _ITEMINFO_
  }

  nState = -1;
  return 0;
}

int HWNP_GetItemCount(__out uint32_t *lpu32Count)
{
  if (nState != -1) return (nLastError = -101);
  if (lpu32Count == NULL) return (nLastError = -102);

  *lpu32Count = u32ItemCount;
  return 0;
}

int HWNP_GetItemIndexById(__in uint32_t u32Id, __out uint32_t *lpu32Index)
{
  if (nState != -1) return (nLastError = -201);

  for (uint32_t u32Index = 0; u32Index < u32ItemCount; u32Index++)
  {
    if (lpItemCopy[u32Index].ItemInfo.u32Id == u32Id)
    {
      if (lpu32Index) *lpu32Index = u32Index;
      return 0;
    }
  }

  return (nLastError = -202);
}

int HWNP_GetItemInfoByIndex(__in uint32_t u32Index, __out PHWNP_ITEMINFO lpItemInfo)
{
  if (nState != -1) return (nLastError = -301);
  if (u32Index >= u32ItemCount) return (nLastError = -302);
  if (lpItemInfo == NULL) return (nLastError = -303);

  *lpItemInfo = lpItemCopy[u32Index].ItemInfo;
  return 0;
}

int HWNP_GetItemDataSizeByIndex(__in uint32_t u32Index, __out uint32_t *lpu32DataSize)
{
  if (nState != -1) return (nLastError = -401);
  if (u32Index >= u32ItemCount) return (nLastError = -402);
  if (lpu32DataSize == NULL) return (nLastError = -403);

  *lpu32DataSize = lpItemCopy[u32Index].u32DataSize;
  return 0;
}

int HWNP_GetItemDataPointerByIndex(__in uint32_t u32Index, __out LPCVOID *lppDataPointer)
{
  if (nState != -1) return (nLastError = -501);
  if (u32Index >= u32ItemCount) return (nLastError = -502);
  if (lppDataPointer == NULL) return (nLastError = -503);

  *lppDataPointer = (LPCVOID)lpItemCopy[u32Index].lpItemData;
  return 0;
}

int HWNP_GetItemDataTypeByIndex(__in uint32_t u32Index, __out LPDWORD lpDataType)
{
  *lpDataType = 0;

  if (nState != -1) return (nLastError = -1501);
  if (u32Index >= u32ItemCount) return (nLastError = -1502);
  if (lpDataType == NULL) return (nLastError = -1503);
  if (lpItemCopy[u32Index].u32DataSize < sizeof(HW_HDR)) return (nLastError = -1504);

  DWORD dwType = 0;

  if ((*(LPDWORD)lpItemCopy[u32Index].lpItemData) == HWNP_HWHW_MAGIC)
  {
    dwType |= IDT_WHWH;

    if ((lpItemCopy[u32Index].u32DataSize - sizeof(HW_HDR) >= sizeof(UIMG_HDR)) &&
      ((*(LPDWORD)((DWORD)lpItemCopy[u32Index].lpItemData + sizeof(HW_HDR))) == IH_MAGIC_LE))
    {
      dwType |= IDT_UBOOT;
    }
  }

  *lpDataType = dwType;
  return 0;
}

int HWNP_GetFirmwareHeader(__out PHWNP_HEADER lpFirmwareHeader)
{
  if (nState != -1) return (nLastError = -601);
  if (lpFirmwareHeader == NULL) return (nLastError = -602);

  *lpFirmwareHeader = hwHeader;
  return 0;
}

int HWNP_GetProductListSize(__out uint16_t *lpu16PLSize)
{
  if (nState != -1) return (nLastError = -701);
  
  *lpu16PLSize = hwHeader.PacketHeader.u16ProductListSize;
  return 0;
}

int HWNP_GetProductList(__inout LPCH lpchProdList, __in uint16_t u16PLSize)
{
  if (nState != -1) return (nLastError = -801);
  if (lpchProdList == NULL) return (nLastError = -802);

  memcpy_s(lpchProdList, u16PLSize, lpProductList, hwHeader.PacketHeader.u16ProductListSize);
  return 0;
}

int HWNP_SetProductList(__in LPCCH lpchProdList, __in uint16_t u16PLSize)
{
  if (nState != -1) return (nLastError = -901);
  if (lpchProdList == NULL) return (nLastError = -902);

  if (u16PLSize == 0)
  {
    strcpy_s(lpProductList, hwHeader.PacketHeader.u16ProductListSize, lpchProdList);
  }
  else
  {
    if (u16PLSize != hwHeader.PacketHeader.u16ProductListSize)
    {
      LPCH lpDest = (LPCH)malloc(u16PLSize);
      if (lpDest == NULL) return (nLastError = -903);

      free(lpProductList);
      lpProductList = lpDest;
      hwHeader.PacketHeader.u16ProductListSize = u16PLSize;
    }

    memcpy_s(lpProductList, hwHeader.PacketHeader.u16ProductListSize, lpchProdList, u16PLSize);
  }

  HWNP_Update();
  return 0;
}

int HWNP_SetItemData(__in uint32_t u32Index, __in LPCVOID lpNewData, __in uint32_t u32DataSize)
{
  if (nState != -1) return (nLastError = -1001);
  if (u32Index >= u32ItemCount) return (nLastError = -1002);
  if (lpNewData == NULL || u32DataSize == 0) return (nLastError = -1003);

  LPVOID lpData = malloc(u32DataSize);

  if (lpData == NULL) return (nLastError = -1004);

  // release old data
  if (lpItemCopy[u32Index].lpItemData) free(lpItemCopy[u32Index].lpItemData);

  // copy new data
  memcpy_s(lpData, u32DataSize, lpNewData, u32DataSize);

  //set project information
  lpItemCopy[u32Index].lpItemData = lpData;
  lpItemCopy[u32Index].u32DataSize = u32DataSize;
  lpItemCopy[u32Index].ItemInfo.u32Size = u32DataSize;

  HWNP_Update();
  return 0;
}

int HWNP_SetItemInfo(__in uint32_t u32Index, __in uint32_t u32Mask, __in uint32_t u32Id,
  __in LPCCH lpchPath, __in LPCCH lpchType, __in LPCCH lpchVersion, __in uint32_t u32Policy, __in uint32_t u32Reserved)
{
  if (nState != -1) return (nLastError = -1101);
  if (u32Index >= u32ItemCount) return (nLastError = -1102);

  if (CHK_FLAGS(u32Mask, IIFLAG_PATH) && (lpchPath == NULL)) return (nLastError = -1103);
  if (CHK_FLAGS(u32Mask, IIFLAG_TYPE) && (lpchType == NULL)) return (nLastError = -1104);
  if (CHK_FLAGS(u32Mask, IIFLAG_VERSION) && (lpchVersion == NULL)) return (nLastError = -1105);

  if (CHK_FLAGS(u32Mask, IIFLAG_ID)) lpItemCopy[u32Index].ItemInfo.u32Id = u32Id;

  if (CHK_FLAGS(u32Mask, IIFLAG_PATH)) {
    memset(lpItemCopy[u32Index].ItemInfo.chItemPath, 0, sizeof(HWNP_ITEMINFO::chItemPath));
    strcpy_s(lpItemCopy[u32Index].ItemInfo.chItemPath, sizeof(HWNP_ITEMINFO::chItemPath), lpchPath);
  }

  if (CHK_FLAGS(u32Mask, IIFLAG_TYPE)) {
    memset(lpItemCopy[u32Index].ItemInfo.chItemType, 0, sizeof(HWNP_ITEMINFO::chItemType));
    strcpy_s(lpItemCopy[u32Index].ItemInfo.chItemType, sizeof(HWNP_ITEMINFO::chItemType), lpchType);
  }

  if (CHK_FLAGS(u32Mask, IIFLAG_VERSION)) {
    memset(lpItemCopy[u32Index].ItemInfo.chItemVersion, 0, sizeof(HWNP_ITEMINFO::chItemVersion));
    strcpy_s(lpItemCopy[u32Index].ItemInfo.chItemVersion, sizeof(HWNP_ITEMINFO::chItemVersion), lpchVersion);
  }

  if (CHK_FLAGS(u32Mask, IIFLAG_POLICY)) lpItemCopy[u32Index].ItemInfo.u32Policy = u32Policy;
  if (CHK_FLAGS(u32Mask, IIFLAG_RESERVED)) lpItemCopy[u32Index].ItemInfo.u32Reserved = u32Reserved;

  HWNP_Update();
  return 0;
}

uint32_t HWNP_GetLastItemId()
{
  if (nState != -1) return 0;

  BOOL blFound;

  for (uint32_t u32Id = 0; u32Id <= u32ItemCount; u32Id++)
  {
    blFound = FALSE;

    for (uint32_t u32Index = 0; u32Index < u32ItemCount; u32Index++)
    {
      if (strcmp(lpItemCopy[u32Index].ItemInfo.chItemType, "EFS") == 0) continue;

      if (u32Id == lpItemCopy[u32Index].ItemInfo.u32Id)
      {
        blFound = TRUE;
        break;
      }
    }

    if (blFound == FALSE) return u32Id;
  }

  return 0;
}

BOOL HWNP_CheckDuplicate()
{
  if (nState != -1) return FALSE;

  for (uint32_t u32Index = 0; u32Index < u32ItemCount; u32Index++)
  {
    if (strcmp(lpItemCopy[u32Index].ItemInfo.chItemType, "EFS") == 0) continue;

    for (uint32_t u32Index2 = 0; u32Index2 < u32ItemCount; u32Index2++)
    {
      if (u32Index2 == u32Index) continue;

      if (strcmp(lpItemCopy[u32Index2].ItemInfo.chItemType, "EFS") == 0) continue;

      if (lpItemCopy[u32Index2].ItemInfo.u32Id == lpItemCopy[u32Index].ItemInfo.u32Id)
        return TRUE;
    }
  }

  return FALSE;
}

uint32_t HWNP_CheckCRC32()
{
  if (nState != -1) return CHKCRC_FAILED;
  uint32_t u32CRC;

  //Check item CRC
  for (uint32_t u32Index = 0; u32Index < u32ItemCount; u32Index++)
  {
    if (lpItemCopy[u32Index].ItemInfo.u32ItemCRC32 != crc32_fast(lpItemCopy[u32Index].lpItemData, lpItemCopy[u32Index].u32DataSize))
      return CHKCRC_ITEMCRCERR + u32Index;
  }

  //Check header CRC
  u32CRC = crc32_fast(&hwHeader.PacketHeader, sizeof(HWNP_PAKHDR));
  u32CRC = crc32_fast(lpProductList, hwHeader.PacketHeader.u16ProductListSize, u32CRC);

  for (uint32_t u32Index = 0; u32Index < u32ItemCount; u32Index++)
    u32CRC = crc32_fast(&lpItemCopy[u32Index].ItemInfo, sizeof(HWNP_ITEMINFO), u32CRC);

  if (hwHeader.FileHeader2.u32HeaderCRC32 != u32CRC) return CHKCRC_HEADERCRCERR;

  //Check the file CRC
  u32CRC = crc32_fast(&hwHeader.FileHeader2, sizeof(HWNP_FILEHDR2));
  u32CRC = crc32_fast(&hwHeader.PacketHeader, sizeof(HWNP_PAKHDR), u32CRC);
  u32CRC = crc32_fast(lpProductList, hwHeader.PacketHeader.u16ProductListSize, u32CRC);

  for (uint32_t u32Index = 0; u32Index < u32ItemCount; u32Index++)
    u32CRC = crc32_fast(&lpItemCopy[u32Index].ItemInfo, sizeof(HWNP_ITEMINFO), u32CRC);

  for (uint32_t u32Index = 0; u32Index < u32ItemCount; u32Index++)
    u32CRC = crc32_fast(lpItemCopy[u32Index].lpItemData, lpItemCopy[u32Index].u32DataSize, u32CRC);

  if (hwHeader.BasicFileHeader.u32FileCRC32 != u32CRC) return CHKCRC_FILECRCERR;

  return CHKCRC_OK;
}

void HWNP_SortItems(__in BOOL blUpdate)
{
  if (nState != -1) return;

  //Post the EFS project
  for (uint32_t u32Index = 0; u32Index < u32ItemCount - 1; u32Index++)
  {
    if (strcmp(lpItemCopy[u32Index].ItemInfo.chItemType, "EFS") == 0)
    {
      HWNP_SwapItemCopy(&lpItemCopy[u32ItemCount - 1], &lpItemCopy[u32Index]);
      break;
    }
  }

  //reassign Id
  for (uint32_t u32Index = 0; u32Index < u32ItemCount - 1; u32Index++)
  {
    lpItemCopy[u32Index].ItemInfo.u32Id = u32Index;
  }

  if (blUpdate) HWNP_Update();
}

int HWNP_AddItem(__in uint32_t u32Id, __in LPCVOID lpNewData, __in uint32_t u32DataSize, __in LPCCH lpchPath,
  __in LPCCH lpchType, __in LPCCH lpchVersion, __in uint32_t u32Policy, __in uint32_t u32Reserved)
{
  if (nState != -1) return (nLastError = -1201);
  if (lpNewData == NULL || u32DataSize == 0) return (nLastError = -1202);
  if (lpchPath == NULL) return (nLastError = -1203);
  if (lpchType == NULL) return (nLastError = -1204);
  if (lpchVersion == NULL) return (nLastError = -1205);

  PINT_ITEMCOPY lpNewItemCopy = (PINT_ITEMCOPY)malloc((u32ItemCount + 1) * sizeof(INT_ITEMCOPY));
  if (lpNewItemCopy == NULL) return (nLastError = -1206);

  lpNewItemCopy[u32ItemCount].lpItemData = malloc(u32DataSize);
  if (lpNewItemCopy[u32ItemCount].lpItemData == NULL)
  {
    free(lpNewItemCopy);
    return (nLastError = -1207);
  }

  memcpy_s(lpNewItemCopy[u32ItemCount].lpItemData, u32DataSize, lpNewData, u32DataSize);
  lpNewItemCopy[u32ItemCount].u32DataSize = u32DataSize;

  lpNewItemCopy[u32ItemCount].ItemInfo.u32Id = u32Id;
  lpNewItemCopy[u32ItemCount].ItemInfo.u32ItemCRC32 = 0;
  lpNewItemCopy[u32ItemCount].ItemInfo.u32Offset = 0;
  lpNewItemCopy[u32ItemCount].ItemInfo.u32Size = u32DataSize;
  strcpy_s(lpNewItemCopy[u32ItemCount].ItemInfo.chItemPath, sizeof(HWNP_ITEMINFO::chItemPath), lpchPath);
  strcpy_s(lpNewItemCopy[u32ItemCount].ItemInfo.chItemType, sizeof(HWNP_ITEMINFO::chItemType), lpchType);
  strcpy_s(lpNewItemCopy[u32ItemCount].ItemInfo.chItemVersion, sizeof(HWNP_ITEMINFO::chItemVersion), lpchVersion);
  lpNewItemCopy[u32ItemCount].ItemInfo.u32Policy = u32Policy;
  lpNewItemCopy[u32ItemCount].ItemInfo.u32Reserved = u32Reserved;

  memcpy_s(lpNewItemCopy, sizeof(INT_ITEMCOPY) * u32ItemCount, lpItemCopy, sizeof(INT_ITEMCOPY) * u32ItemCount);
  free(lpItemCopy);
  lpItemCopy = lpNewItemCopy;
  u32ItemCount++;

  //Post the EFS project
  for (uint32_t u32Index = 0; u32Index < u32ItemCount - 1; u32Index++)
  {
    if (strcmp(lpItemCopy[u32Index].ItemInfo.chItemType, "EFS") == 0)
    {
      HWNP_SwapItemCopy(&lpItemCopy[u32ItemCount - 1], &lpItemCopy[u32Index]);
      break;
    }
  }

  HWNP_Update();
  return 0;
}

int HWNP_DeleteItem(__in uint32_t u32Index)
{
  if (nState != -1) return (nLastError = -1301);
  if (u32Index >= u32ItemCount) return (nLastError = -1302);

  uint32_t u32NewCount = u32ItemCount - 1;
  PINT_ITEMCOPY lpNewItemCopy = (PINT_ITEMCOPY)malloc(u32NewCount * sizeof(INT_ITEMCOPY));

  if (lpNewItemCopy == NULL) return (nLastError = -1303);

  if (u32Index == 0)
  {
    memcpy_s(lpNewItemCopy, u32NewCount * sizeof(INT_ITEMCOPY), &lpItemCopy[1], u32NewCount * sizeof(INT_ITEMCOPY));
  }
  else if (u32Index == u32NewCount)
  {
    memcpy_s(lpNewItemCopy, u32NewCount * sizeof(INT_ITEMCOPY), lpItemCopy, u32NewCount * sizeof(INT_ITEMCOPY));
  }
  else
  {
    uint32_t u32Tmp = u32NewCount - u32Index;

    memcpy_s(lpNewItemCopy, u32Index * sizeof(INT_ITEMCOPY), lpItemCopy, u32Index * sizeof(INT_ITEMCOPY));
    memcpy_s(&lpNewItemCopy[u32Index], u32Tmp * sizeof(INT_ITEMCOPY), &lpItemCopy[u32Index + 1], u32Tmp * sizeof(INT_ITEMCOPY));
  }

  u32ItemCount = u32NewCount;
  lpItemCopy = lpNewItemCopy;
  HWNP_Update();
  return 0;
}

int HWNP_Save()
{
  if (nState != -1) return (nLastError = -1401);

  uint32_t u32FileSize, u32DataSize = 0;
  DWORD dwOffset = 0, dwTmp;

  HWNP_Update();

  // Calculate data size
  for (uint32_t u32Index = 0; u32Index < u32ItemCount; u32Index++)
    u32DataSize += lpItemCopy[u32Index].u32DataSize;

  //calculate file size
  u32FileSize = sizeof(HWNP_HEADER) + hwHeader.PacketHeader.u16ProductListSize + u32ItemCount * sizeof(HWNP_ITEMINFO) + u32DataSize;

  // reset the file size
  SetFilePointer(hFile, u32FileSize, NULL, FILE_BEGIN);
  SetEndOfFile(hFile);

  //write header
  SetFilePointer(hFile, dwOffset, NULL, FILE_BEGIN);
  WriteFile(hFile, &hwHeader, sizeof(HWNP_HEADER), &dwTmp, NULL);
  dwOffset += sizeof(HWNP_HEADER);

  SetFilePointer(hFile, dwOffset, NULL, FILE_BEGIN);
  WriteFile(hFile, lpProductList, hwHeader.PacketHeader.u16ProductListSize, &dwTmp, NULL);
  dwOffset += hwHeader.PacketHeader.u16ProductListSize;

  //Write project information
  for (uint32_t u32Index = 0; u32Index < u32ItemCount ; u32Index++)
  {
    SetFilePointer(hFile, dwOffset, NULL, FILE_BEGIN);
    WriteFile(hFile, &lpItemCopy[u32Index].ItemInfo, sizeof(HWNP_ITEMINFO), &dwTmp, NULL);
    dwOffset += sizeof(HWNP_ITEMINFO);
  }

  //write item data
  for (uint32_t u32Index = 0; u32Index < u32ItemCount; u32Index++)
  {
    SetFilePointer(hFile, dwOffset, NULL, FILE_BEGIN);
    WriteFile(hFile, lpItemCopy[u32Index].lpItemData, lpItemCopy[u32Index].u32DataSize, &dwTmp, NULL);
    dwOffset += lpItemCopy[u32Index].u32DataSize;
  }

  return 0;
}

int HWNP_SaveAs(__in LPCWSTR lpFilePath)
{
  if (nState != -1) return (nLastError = -1401);

  HANDLE hNewFile;
  uint32_t u32FileSize, u32DataSize = 0;
  DWORD dwOffset = 0, dwTmp;

  hNewFile = CreateFileW(lpFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hNewFile == INVALID_HANDLE_VALUE) return (nLastError = -1402);

  HWNP_Update();

  // Calculate data size
  for (uint32_t u32Index = 0; u32Index < u32ItemCount; u32Index++)
    u32DataSize += lpItemCopy[u32Index].u32DataSize;

  //calculate file size
  u32FileSize = sizeof(HWNP_HEADER) + hwHeader.PacketHeader.u16ProductListSize + u32ItemCount * sizeof(HWNP_ITEMINFO) + u32DataSize;

  // reset the file size
  SetFilePointer(hNewFile, u32FileSize, NULL, FILE_BEGIN);
  SetEndOfFile(hNewFile);

  //write header
  SetFilePointer(hNewFile, dwOffset, NULL, FILE_BEGIN);
  WriteFile(hNewFile, &hwHeader, sizeof(HWNP_HEADER), &dwTmp, NULL);
  dwOffset += sizeof(HWNP_HEADER);

  SetFilePointer(hNewFile, dwOffset, NULL, FILE_BEGIN);
  WriteFile(hNewFile, lpProductList, hwHeader.PacketHeader.u16ProductListSize, &dwTmp, NULL);
  dwOffset += hwHeader.PacketHeader.u16ProductListSize;

  //Write project information
  for (uint32_t u32Index = 0; u32Index < u32ItemCount; u32Index++)
  {
    SetFilePointer(hNewFile, dwOffset, NULL, FILE_BEGIN);
    WriteFile(hNewFile, &lpItemCopy[u32Index].ItemInfo, sizeof(HWNP_ITEMINFO), &dwTmp, NULL);
    dwOffset += sizeof(HWNP_ITEMINFO);
  }

  //write item data
  for (uint32_t u32Index = 0; u32Index < u32ItemCount; u32Index++)
  {
    SetFilePointer(hNewFile, dwOffset, NULL, FILE_BEGIN);
    WriteFile(hNewFile, lpItemCopy[u32Index].lpItemData, lpItemCopy[u32Index].u32DataSize, &dwTmp, NULL);
    dwOffset += lpItemCopy[u32Index].u32DataSize;
  }

  CloseHandle(hNewFile);
  return 0;
}

int HWNP_CalibrationImageHeaderCrc32(PUIMG_HDR lpImageHdr) {
  UIMG_HDR hdrTmp;

  if (lpImageHdr == NULL) return -1;
  if (lpImageHdr->ih_magic != IH_MAGIC_LE) return -2;

  hdrTmp = *lpImageHdr;
  hdrTmp.ih_hcrc = 0;

  lpImageHdr->ih_hcrc = EndianSwap32(crc32_fast(&hdrTmp, sizeof(UIMG_HDR)));
  return 0;
}
