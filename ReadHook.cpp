#include "ReadHook.h"
#include <iostream>
#include <fstream>

using namespace std;

//https://msdn.microsoft.com/en-us/library/windows/desktop/aa365467(v=vs.85).aspx
typedef BOOL (WINAPI *td_ReadFile)(
    HANDLE       hFile,
    LPVOID       lpBuffer,
    DWORD        nNumberOfBytesToRead,
    LPDWORD      lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped
    );

td_ReadFile originalReadFile = NULL;


BOOL WINAPI ReadHook::ReadFileDetour(
    HANDLE       hFile,
    LPVOID       lpBuffer,
    DWORD        nNumberOfBytesToRead,
    LPDWORD      lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped
    )
{

  OutputDebugStringA((LPCSTR)lpBuffer);
  return originalReadFile(
      hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
}


void ReadHook::Initialize()
{
  CreateDirectory("./out", 0);
  HANDLE hProcess = GetCurrentProcess();

  /* Init */
  HMODULE kernelHandle = NULL;
  while (!kernelHandle) //Wait until loaded
  {
    kernelHandle = GetModuleHandle(TEXT("Kernel32.dll"));
    Sleep(500);
  }
  //Get function of ReadFile
  //originalReadFile = (td_ReadFile*)GetProcAddress(
  //    kernelHandle,
  //    "ReadFile");
  //

  if (MH_Initialize() != MH_OK)
  {
    //err
  }

  if (MH_CreateHookApiEx(L"Kernel32", "ReadFile", (void*)&ReadFileDetour, &originalReadFile) != MH_OK)
  {
    //err
  }

  if (MH_EnableHook((void*)&ReadFile) != MH_OK)
  {
    //err
  }

  // DWORD oldProt = protectMemory(originalReadFile, PAGE_EXECUTE_READWRITE)

  ofstream file;
  file.open("hook.txt");
  file << "Kernel32::ReadFile found\n";
  if(originalReadFile != NULL)
  {
    file << (void*)originalReadFile << std::endl;
  }
  file.close();

}

void ReadHook::Restore()
{

}
