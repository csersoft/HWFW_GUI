#pragma once

#ifndef _INC_STDDEF
#include <stddef.h>
#endif

#ifndef _STDINT
#include <stdint.h>
#endif

#ifndef SPECSTRINGS_H
#include <specstrings.h>
#endif

#ifndef _WINDEF_
#include <WinDef.h>
#endif

#ifndef _WINBASE_
#include <WinBase.h>
#endif

#include "uboot_image.h"

/*
参考:
http://blog.leexiaolan.tk/pwn-huawei-hg8120c-ont-via-maintenance-tool-part-2
http://blog.leexiaolan.tk/pwn-huawei-hg8120c-ont-upgrade-pack-format-part-3
https://github.com/LeeXiaolan/hwfw-tool
*/

#define CHK_FLAGS(v, f)																		(((v) & (f)) == (f))
#define HWNP_HEADER_MAGIC																	0x504e5748																																			//HWNP
#define CHK_OUT_OF_UBOUND(pointer, start, size)						((size_t)(pointer) >= ((size_t)(start) + (size_t)(size)))												//检查上标越界
#define CHK_OUT_OF_UBOUND2(pointer, len, start, size)			(((size_t)(pointer) + (size_t)(len))  > ((size_t)(start) + (size_t)(size)))			//检查上标越界

#define BigLittleSwap32(int32)														((((uint32_t)(int32) & 0xff000000U) >> 24) | \
																													(((uint32_t)(int32) & 0x00ff0000U) >> 8) | \
																													(((uint32_t)(int32) & 0x0000ff00U) << 8) | \
																													(((uint32_t)(int32) & 0x000000ffU) << 24))

#define OffsetOf(TYPE, MEMBER)														((size_t) & ((TYPE *)0)->MEMBER )

//ItemInfo Flags
#define IIFLAG_ID																					0x00000001U
#define IIFLAG_PATH																				0x00000002U
#define IIFLAG_TYPE																				0x00000004U
#define IIFLAG_VERSION																		0x00000008U
#define IIFLAG_POLICY																			0x00000010U
#define IIFLAG_RESERVED																		0x00000020U
#define IIFLAG_ALL																				(IIFLAG_ID | IIFLAG_PATH | IIFLAG_TYPE | IIFLAG_VERSION | IIFLAG_POLICY | IIFLAG_RESERVED)


#define CHKCRC_OK																					0
#define CHKCRC_FAILED																			11
#define CHKCRC_FILECRCERR																	191
#define CHKCRC_HEADERCRCERR																192
#define CHKCRC_ITEMCRCERR																	200

/*
HuaWei 固件结构

 ___________________________________
|																		|
|						Header (文件头)					|
|___________________________________|
|																		|
|		ProductList (支持的产品列表)		|
|___________________________________|
|																		|
|				ItemInfo (项目信息)	* N			|
|===================================|	<---通常是连续的，但是理论上可以不连续
|																		|
|						项目 RAW数据						|
|___________________________________|

*/


//基本文件头
typedef struct _HWNP_BasicFileHeader
{
	//魔法字
	uint32_t									u32Magic;

	//文件大小 (计算公式为.bin的文件大小 - 76)	big-endian
	uint32_t									beu32FileSize;

	//从HWNP_FILEHDR2开始到文件结束的CRC32值 (包含项目数据)
	uint32_t									u32FileCRC32;
} HWNP_BASFILEHDR, *PHWNP_BASFILEHDR;

//文件头2
typedef struct _HWNP_FileHeader2
{
	//头部大小	
	//计算公式有2种:
	//1:	sizeof(HWNP_HEADER) + Header.u16ProductListSize + Header.u32ItemCount * Header.u16ItemInfoSize
	//2:	Header.u16ProductListSize + Header.u32ItemCount * Header.u16ItemInfoSize
	//前一种多了文件头大小
	//新版本固件一般是第一种公式,好像V100版本的固件是第二种计算公式
	//该数值不是计算头部CRC32时的大小
	uint32_t									u32HeaderSize;

	//从HWNP_PAKHDR开始到N个HWNP_ITEMINFO结束的CRC32值 (不包含项目数据)
	uint32_t									u32HeaderCRC32;
} HWNP_FILEHDR2, *PHWNP_FILEHDR2;

//包头
typedef struct _HWNP_PacketHeader
{
	uint32_t									u32ItemCount;
	uint8_t										u8PackTotal;
	uint8_t										u8PackNum;
	uint16_t									u16ProductListSize;

	union {
		uint16_t								u16ItemInfoSize;					//== sizeof(HWNP_ITEMINFO)
		uint32_t								_u32ItemInfoSize;					//== sizeof(HWNP_ITEMINFO)
	};
	
	uint32_t									u32Reserved;
} HWNP_PAKHDR, *PHWNP_PAKHDR;

typedef struct _HWNP_Header
{
	HWNP_BASFILEHDR						BasicFileHeader;
	//file crc32 start comupte
	HWNP_FILEHDR2							FileHeader2;
	//header crc32 start comupte
	HWNP_PAKHDR								PacketHeader;
} HWNP_HEADER, *PHWNP_HEADER;


typedef struct _HWNP_ItemInfo
{
	uint32_t									u32Id;								//原	Index
	uint32_t									u32ItemCRC32;					
	uint32_t									u32Offset;
	uint32_t									u32Size;							//原	Len
	char											chItemPath[256];			//原	Destination
	char											chItemType[16];				//原	Name
	char											chItemVersion[64];
	uint32_t									u32Policy;
	uint32_t									u32Reserved;
} HWNP_ITEMINFO, *PHWNP_ITEMINFO;


enum _WHHDR_ItemType : uint32_t
{
	whType_Kernel = 1U,
	whType_RootFS,
	whType_System,
	whType_MiniSYS
};

typedef struct _WHWH_Header
{
	//魔法字
	uint32_t									u32Magic;
	char											chItemVersion[64];
	__time32_t								u32Time;
	_WHHDR_ItemType						enumType;
	uint32_t									u32RearSize;
	uint32_t									u32RearCRC;
} WHWH_HEADER, *PWHWH_HEADER;



//内存文件是否已更改
BOOL				HWNP_IsChanged();

//获取最后的错误码
int					HWNP_GetLastError();

//获取状态值: -1为已正确打开固件文件
int					HWNP_GetState();

//获取头部大小的计算方式
int					HWNP_GetHeaderSizeType();

//设置头部大小的计算方式
int					HWNP_SetHeaderSizeType(__in int nNewType);

//更新信息 (CRC, 项目偏移, 项目大小 等)
void				HWNP_Update();

//释放一切资源
void				HWNP_Release();

//打开固件文件
int					HWNP_OpenFirmware(__in LPCWSTR lpFilePath);

//获取项目数量
int					HWNP_GetItemCount(__out uint32_t *lpu32Count);

//获取项目索引通过项目Id
int					HWNP_GetItemIndexById(__in uint32_t u32Id, __out uint32_t *lpu32Index);

//获取项目信息
int					HWNP_GetItemInfoByIndex(__in uint32_t u32Index, __out PHWNP_ITEMINFO lpItemInfo);

//获取项目数据大小
int					HWNP_GetItemDataSizeByIndex(__in uint32_t u32Index, __out uint32_t *lpu32DataSize);

//获取项目数据 (仅用于访问, 勿写入)
int					HWNP_GetItemDataPointerByIndex(__in uint32_t u32Index, __out LPCVOID *lppDataPointer);

//获取固件头部
int					HWNP_GetFirmwareHeader(__out PHWNP_HEADER lpHeader);

//获取产品支持列表大小
int					HWNP_GetProductListSize(__out uint16_t *lpu16PLSize);

//获取产品支持列表
int					HWNP_GetProductList(__inout LPCH lpchProdList, __in uint16_t u16PLSize);

//设置产品支持列表
int					HWNP_SetProductList(__in LPCCH lpchProdList, __in uint16_t u16PLSize);

//设置项目数据
int					HWNP_SetItemData(__in uint32_t u32Index, __in LPCVOID lpNewData, __in uint32_t u32DataSize);

//设置项目信息
int					HWNP_SetItemInfo(__in uint32_t u32Index, __in uint32_t u32Mask, __in uint32_t u32Id, __in LPCCH lpchPath, __in LPCCH lpchType, __in LPCCH lpchVersion, __in uint32_t u32Policy, __in uint32_t u32Reserved);

//获取项目最后的Id (未使用的id)
uint32_t		HWNP_GetLastItemId();

//检查是否有重复的Id
BOOL				HWNP_CheckDuplicate();

//校验CRC (0: 没有问题, 191:文件CRC不正确, 192: 头部CRC不正确, 200+: 项目CRC不正确)
uint32_t		HWNP_CheckCRC32();

//按照顺序,重新为项目Id排序
void				HWNP_SortItems(__in BOOL blUpdate);

//添加新项目
int					HWNP_AddItem(__in uint32_t u32Id, __in LPCVOID lpNewData, __in uint32_t u32DataSize, __in LPCCH lpchPath, __in LPCCH lpchType, __in LPCCH lpchVersion, __in uint32_t u32Policy, __in uint32_t u32Reserved);

//删除项目
int					HWNP_DeleteItem(__in uint32_t u32Index);

//保存到文件
int					HWNP_Save();

//保存到其它文件
int					HWNP_SaveAs(__in LPCWSTR lpFilePath);
