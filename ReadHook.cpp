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
    //Save the file
    char* filename = strrchr((const char*)fPath, '\\');

    if(filename != NULL)
    {
      filename++;

      //Check if exefilepath + out + filename
      char* newFilePath = (char*)malloc(
          sizeof(m_exePath)+
          sizeof(filename)+
          sizeof("\\_tout\\"));
      char* newFileInc;

      strcpy(newFilePath, m_exePath);
      strcat(newFilePath, "\\_tout\\");
      strcat(newFilePath, filename);

      int offset = 0;
      bool isOffset = false;
      //while(MatFile::FileExists(newFilePath))
      //{
      //  isOffset = true;
      //  offset++;
      //  newFileInc = (char*)malloc(sizeof(newFilePath)+sizeof(char)*5);

      //  strcpy(newFileInc, newFilePath);
      //  char*soffset;
      //  sprintf(soffset, "%ld", offset);
      //  strcat(newFileInc, soffset);

      //  //Save as newfileinc
      //}

      BOOL result = originalReadFile(
            hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);

      if(*lpNumberOfBytesRead == 0) //EOF
      {
        return result;
      }

      HANDLE fTargetFile = CreateFile(newFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
     // HANDLE fTargetFile = isOffset ?
     //   CreateFile(newFileInc, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL):
     //   CreateFile(newFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

      if(fTargetFile == INVALID_HANDLE_VALUE)
      {
        OutputDebugString("New file not created.");
        return result;
      }

      DWORD dwBytesWritten;


      if(!WriteFile(fTargetFile, lpBuffer, *lpNumberOfBytesRead, &dwBytesWritten, NULL))
      {
        OutputDebugString("Buffer not written.");
        return result;
      }

      CloseHandle(fTargetFile);
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
    //err
  }

  if (MH_CreateHookApiEx(L"Kernel32", "ReadFile", (void*)&ReadHook::ReadFileDetour, &originalReadFile) != MH_OK)
  {
    //err
  }

  if (MH_EnableHook((void*)&ReadFile) != MH_OK)
  {
    //err
  }
}

void ReadHook::Restore()
{

}

