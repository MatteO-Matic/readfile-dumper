#include "ReadHook.h"
#include "MatFile.h"
#include "logger.h"

#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

//https://msdn.microsoft.com/en-us/library/windows/desktop/aa365247(v=vs.85).aspx#maxpath
//Long path is actually 32767

#define LONG_PATH 4096

std::string ReadHook::m_exePath;
HANDLE ReadHook::m_outHandle = NULL;
//<READHANDLE, WRITEHANDLE>
std::vector<string> ReadHook::m_files;

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

  //Check if the READFILE handle is ok
  if(!hFile || hFile == INVALID_HANDLE_VALUE)
  {
    Logger::getLogger()->Log("ReadHook: Invalid handle value");
    return originalReadFile(
        hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
  }

  Logger::getLogger()->Log("LP");
  Logger::getLogger()->Log(static_cast<const char*>( lpBuffer ));

  TCHAR fPath[LONG_PATH];
  DWORD dwRet;
  dwRet = GetFinalPathNameByHandle(hFile, fPath, LONG_PATH, NULL);


  if(dwRet < LONG_PATH)
  {
    //Get filename
    vector<string> spline;
    string filename;

    MatFile::Split(spline, fPath, "\\");

    if(!spline.empty())
    {
      filename = spline.back();
    }

    //Check if exefilepath + out + filename
    string newFilePath = m_exePath + "\\_tout\\";
    newFilePath += filename;

    string filepath(fPath);
    Logger::getLogger()->Log(filepath.c_str());

    if(filepath.empty())
    {
      Logger::getLogger()->Log("no filepath");
      return originalReadFile(
          hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
    }

    //Check if it already exists
    if(find(m_files.begin(), m_files.end(), filepath) != m_files.end())
    {
      Logger::getLogger()->Log("Already added %s", filepath.c_str());
      return originalReadFile(
          hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
    }
    else
    {
      //New file, write full file to _out
      //Add to vector
      m_files.push_back(filepath);
      //Get current file pointer
      LARGE_INTEGER fp = MatFile::GetFilePointer(hFile);

      HANDLE hOutFile =
        CreateFile(newFilePath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

      if(hOutFile == INVALID_HANDLE_VALUE)
      {
        Logger::getLogger()->Log("New file was not created. Invalid handle.");
        //Try to close handle
        if(!CloseHandle(hOutFile))
        {
          Logger::getLogger()->Log("Writefile handle couldn't be closed. Error %u",  GetLastError());
        }
        return originalReadFile(
            hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
      }
      bool result = true;
      while(result)
      {
        result = originalReadFile(
            hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);

        if(!result)
        {
          Logger::getLogger()->Log("File couldn't be read. Error %u", GetLastError());
          break;
        }

        if(*lpNumberOfBytesRead == 0) //EOF
        {
          Logger::getLogger()->Log("EOF");
          break;
        }

        //Write read data to handle
        DWORD dwBytesWritten;
        if(!WriteFile(hOutFile, lpBuffer, *lpNumberOfBytesRead, &dwBytesWritten, NULL))
        {
          Logger::getLogger()->Log("Buffer not written. Error %u",  GetLastError());
          break;
        }

      }

      if(!CloseHandle(hOutFile))
      {
        Logger::getLogger()->Log("Writefile handle couldn't be closed. Error %u",  GetLastError());
      }
      //reset READHANDLE filepointer
      LARGE_INTEGER lpNewFp;
      SetFilePointerEx(hFile, fp, &lpNewFp, FILE_BEGIN);
      //Go back to original read function
      return originalReadFile(
          hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
    }
  }
  else
  {
    Logger::getLogger()->Log("Path buffer to small.");
  }
  return originalReadFile(
      hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
}


void ReadHook::Initialize()
{
  // SelfPath
  char buffer[LONG_PATH];
  GetModuleFileName(NULL, buffer, sizeof(buffer));

  string::size_type pos = string(buffer).find_last_of("\\/");
  m_exePath = string(buffer).substr(0, pos);

  //char*ldir= strrchr((const char*)m_exePath, '\\');
  // if(ldir != NULL)
  //   *ldir = '\0';

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
  if (MH_DisableHook((void*)&ReadFile) != MH_OK)
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

