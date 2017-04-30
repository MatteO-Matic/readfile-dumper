#include "ReadHook.h"
#include <iostream>
#include <fstream>
#include <windows.h>
#include <fileapi.h>

using namespace std;

#define BUFSIZE MAX_PATH

//https://msdn.microsoft.com/en-us/library/windows/desktop/aa365467(v=vs.85).aspx
typedef BOOL (WINAPI *td_ReadFile)(
    HANDLE       hFile,
    LPVOID       lpBuffer,
    DWORD        nNumberOfBytesToRead,
    LPDWORD      lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped
    );

td_ReadFile originalReadFile = NULL;

int readFileCounter = 0;

BOOL WINAPI ReadHook::ReadFileDetour(
    HANDLE       hFile,
    LPVOID       lpBuffer,
    DWORD        nNumberOfBytesToRead,
    LPDWORD      lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped
    )
{
  //Get file path
  TCHAR fPath[BUFSIZE];
  DWORD dwRet;

  if(hFile == INVALID_HANDLE_VALUE)
  {
    OutputDebugStringA("ReadHook: Invalid handle value");
    return originalReadFile(
      hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
  }
  //GetFileInformationByHandleEx(hFile)
  dwRet = GetFinalPathNameByHandleA(hFile, fPath, BUFSIZE, NULL);




 // if(dwRet < BUFSIZE)
 // {
 //   OutputDebugStringA(fPath);
 // }

  return originalReadFile(
      hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
}


void ReadHook::Initialize()
{
  cout << "Initialize readhook\n";
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
