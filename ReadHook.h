#pragma once
#include <windows.h>
#include "MinHook.h"
#include <string>
#include <map>

class ReadHook
{
  #define LONG_PATH 4096

  private:
    static std::string m_exePath;
    static HANDLE m_outHandle;
    static std::map<HANDLE, HANDLE> m_fHandles;

  public:
    void Initialize();
    void Restore();
    static BOOL WINAPI ReadFileDetour(
        HANDLE       hFile,
        LPVOID       lpBuffer,
        DWORD        nNumberOfBytesToRead,
        LPDWORD      lpNumberOfBytesRead,
        LPOVERLAPPED lpOverlapped);

};

  template <typename T>
inline MH_STATUS MH_CreateHookEx(LPVOID pTarget, LPVOID pDetour, T** ppOriginal)
{
  return MH_CreateHook(pTarget, pDetour, reinterpret_cast<LPVOID*>(ppOriginal));
}

  template <typename T>
inline MH_STATUS MH_CreateHookApiEx(
    LPCWSTR pszModule, LPCSTR pszProcName, LPVOID pDetour, T** ppOriginal)
{
  return MH_CreateHookApi(
      pszModule, pszProcName, pDetour, reinterpret_cast<LPVOID*>(ppOriginal));
}

