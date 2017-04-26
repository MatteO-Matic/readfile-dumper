#include "ReadHook.h"

//typedef void (WINAPI *)(HWND, LPCSTR, LPCSTR, UINT);

//https://msdn.microsoft.com/en-us/library/windows/desktop/aa365467(v=vs.85).aspx
typedef BOOL (WINAPI* td_ReadFile)(
    HANDLE       hFile,
    LPVOID       lpBuffer,
    DWORD        nNumberOfBytesToRead,
    LPDWORD      lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped
    );

void ReadHook::Initialize()
{
  HANDLE hProcess = GetCurrentProcess();

  /* Init */
  HMODULE kernelHandle = NULL;
  td_ReadFile originalReadFile;
  while (!kernelHandle) //Wait until loaded
  {
    kernelHandle = GetModuleHandle(TEXT("Kernel32.dll"));
    Sleep(500);
  }
  //Get function of ReadFile
  originalReadFile = (td_ReadFile)GetProcAddress(
      kernelHandle,
      "ReadFile");

  if(originalReadFile != NULL)
  {
    std::cout << (void*)originalReadFile << std::endl;
  }

}

void ReadHook::Restore()
{

}
