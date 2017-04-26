#include "MatMem.h"

  template<typename T>
DWORD protectMemory(DWORD address, DWORD prot)
{
  DWORD oldProt;
  VirtualProtect((LPVOID)address, sizeof(T), prot, &oldProt);
  return oldProt;
}

  template<typename T>
T readMemory(DWORD address)
{
  return *((T*)address);
}

  template<typename T>
void writeMemory(DWORD address, T value)
{
  *((T*)address) = value;
}

