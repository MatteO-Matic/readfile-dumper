#include "ReadHook.h"
#include "MatFile.h"
#include <iostream>
#include <fstream>
#include <windows.h>
#include <string.h>

using namespace std;

//https://msdn.microsoft.com/en-us/library/windows/desktop/aa365247(v=vs.85).aspx#maxpath
//Long path is actually 32767

#define LONG_PATH 4096

TCHAR ReadHook::m_exePath[LONG_PATH];
HANDLE ReadHook::m_outHandle = NULL;


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
  if(hFile == INVALID_HANDLE_VALUE)
  {
    OutputDebugString("ReadHook: Invalid handle value");
    return originalReadFile(
        hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
  }

  //Get file path
  TCHAR fPath[LONG_PATH];
  DWORD dwRet;

  dwRet = GetFinalPathNameByHandle(hFile, fPath, LONG_PATH, NULL);

  if(dwRet < LONG_PATH)
  {
    char* filename = strrchr((const char*)fPath, '\\');

    if(filename != NULL)
    {
      filename++; //don't include first \

      //Check if exefilepath + out + filename
      char* newFilePath = (char*)malloc(
          sizeof(m_exePath)+
          sizeof(filename)+
          sizeof("\\_tout\\"));

      strcpy(newFilePath, m_exePath);
      strcat(newFilePath, "\\_tout\\");
      strcat(newFilePath, filename);

      //Create out file
      m_outHandle =
        CreateFile(newFilePath, FILE_APPEND_DATA, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

      if(m_outHandle == INVALID_HANDLE_VALUE)
      {
        OutputDebugString("New file not created.");
        return originalReadFile(
            hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
      }

      //Readfile and append buffer to outfile
      bool result = true;
        result = originalReadFile(
            hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);

        if(!result)
        {
          char*errormsg;
          sprintf(errormsg, "File couldn't be read. Error %u", GetLastError());
          OutputDebugString(errormsg);
          return result;
        }

        if(*lpNumberOfBytesRead == 0) //EOF
        {
          return result;
        }

        DWORD dwBytesWritten;

        //Save the file
        if(!WriteFile(m_outHandle, lpBuffer, *lpNumberOfBytesRead, &dwBytesWritten, NULL))
        {
          char*errormsg;
          sprintf(errormsg, "Buffer not written. Error %u", GetLastError());
          OutputDebugString(errormsg);
          return result;
        }

      CloseHandle(m_outHandle);
      return result;
    }

  }
  else
  {
    OutputDebugString("Path buffer to small.");
  }

  return originalReadFile(
      hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
}


void ReadHook::Initialize()
{
  // SelfPath
  GetModuleFileName(NULL, m_exePath, sizeof(m_exePath));
  char*ldir= strrchr((const char*)m_exePath, '\\');

  if(ldir != NULL)
    *ldir = '\0';

  cout << m_exePath << "\n";

  cout << "Initialize readhook\n";
  CreateDirectory("./_tout", 0);
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
      cout << "Unable to initialize MH\n";
      return;
  }

  if (MH_CreateHookApiEx(L"Kernel32", "ReadFile", (void*)&ReadHook::ReadFileDetour, &originalReadFile) != MH_OK)
  {
    cout << "Unable create api hook Kernel32.ReadFile()\n";
    return;
  }

  if (MH_EnableHook((void*)&ReadFile) != MH_OK)
  {
    cout << "Unable write hook to ReadFile\n";
    return;
  }
}

void ReadHook::Restore()
{
    if (MH_DisableHook(&ReadFile) != MH_OK)
    {
      cout << "Unable to unhook ReadFile\n";
      return;
    }
    if (MH_Uninitialize() != MH_OK)
    {
      cout << "Unable to uninitialize MH";
      return;
    }
}

