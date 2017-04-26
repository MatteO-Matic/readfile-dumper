#pragma once
#include <windows.h>

namespace ReadHook
{
  DWORD protectMemory(DWORD address, DWORD prot);

  template<typename T>
  T readMemory(DWORD address);

  template<typename T>
  void writeMemory(DWORD address, T value);
}
