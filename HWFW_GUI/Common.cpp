#include "stdafx.h"
#include "HWFW_GUI.hpp"

static uint32_t MemAlignCpy(LPVOID const dest, size_t const destSize, LPCVOID const src, size_t const srcSize, int fill) {
  size_t alignSize, fillSize;

  alignSize = alignPage(srcSize);
  fillSize = alignSize - srcSize;

  memcpy_s(dest, destSize, src, srcSize);

  if (fillSize != 0) {
    memset(MakePointer32(dest, srcSize), fill, fillSize);
  }

  return alignSize;
}