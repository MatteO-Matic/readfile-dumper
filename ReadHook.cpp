#include "ReadHook.h"
#include "MatFile.h"
#include "logger.h"

#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
#include <map>

using namespace std;

//https://msdn.microsoft.com/en-us/library/windows/desktop/aa365247(v=vs.85).aspx#maxpath
//Long path is actually 32767

#define LONG_PATH 4096

std::string ReadHook::m_exePath;
HANDLE ReadHook::m_outHandle = NULL;
//<READHANDLE, WRITEHANDLE>
std::map<HANDLE, HANDLE> ReadHook::m_fHandles;

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
  if(!hFile || hFile == INVALID_HANDLE_VALUE)
  {
    OutputDebugString("ReadHook: Invalid handle value");
    return originalReadFile(
        hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
  }

  if(m_fHandles.count(hFile) > 0) //if we have the READHANDLE already
  {
    //Check if handle is still good
    if(m_fHandles.at(hFile) == INVALID_HANDLE_VALUE)
    {
      OutputDebugString("Invalid WRITEHANDLE");
      //Create a new one and append to last one

      return originalReadFile( //temp here, should createfile append?
          hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
    }

    //Readfile
    bool result = originalReadFile(
        hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
    if(!result)
    {
      Logger::getLogger()->Log("File couldn't be read. Error %u", GetLastError());
      return result;
    }

    if(*lpNumberOfBytesRead == 0) //EOF
    {
      OutputDebugString("EOF");
      if(!CloseHandle(m_fHandles.at(hFile)))
      {
        Logger::getLogger()->Log("Writefile handle couldn't be closed. Error %u",  GetLastError());
      }
      m_fHandles.erase(hFile);
      return result;
    }

    //Write read data to handle
    DWORD dwBytesWritten;
    if(!WriteFile(m_fHandles.at( hFile ), lpBuffer, *lpNumberOfBytesRead, &dwBytesWritten, NULL))
    {
      Logger::getLogger()->Log("Buffer not written. Error %u",  GetLastError());
      return result;
    }
  }
  else //We don't have a  WRITEHANDLE, Create new WRITEHANDLE
  {
    Logger::getLogger()->Log("We have no writehandle, Create a new one");
    //Get file path from READHANDLE
    TCHAR fPath[LONG_PATH];
    DWORD dwRet;
    dwRet = GetFinalPathNameByHandle(hFile, fPath, LONG_PATH, NULL);

    if(dwRet < LONG_PATH)
    {
      //Get filename
      vector<string> spline;
      string filename;
      OutputDebugString(fPath);
      MatFile::Split(spline, fPath, "\\");

      if(!spline.empty())
      {
        filename = spline.back();
      }
      OutputDebugString(filename.c_str());

      if(!filename.empty())
      {
        OutputDebugString(filename.c_str());
        Logger::getLogger()->Log("Creating WRITEHANDLE for %u", filename);
        //Check if exefilepath + out + filename
        string newFilePath = m_exePath + "\\_tout\\";
        newFilePath += filename;
        m_fHandles[hFile] =
          CreateFile(newFilePath.c_str(), FILE_APPEND_DATA, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);


        if(m_fHandles.at( hFile ) == INVALID_HANDLE_VALUE)
        {
          OutputDebugString("New file not created.");
          return originalReadFile(
              hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
        }

        //Readfile and write to WRITEHANDLE
        bool result = originalReadFile(
            hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
        if(!result)
        {
          Logger::getLogger()->Log("File couldn't be read. Error %u", GetLastError());
          return result;
        }

        if(*lpNumberOfBytesRead == 0) //EOF
        {
          OutputDebugString("EOF");
          if(!CloseHandle(m_fHandles.at( hFile )))
          {
            Logger::getLogger()->Log("Writefile handle couldn't be closed. Error %u",  GetLastError());
          }
          m_fHandles.erase(hFile);
          return result;
        }

        //Write read data to handle
        DWORD dwBytesWritten;
        if(!WriteFile(m_fHandles.at( hFile ), lpBuffer, *lpNumberOfBytesRead, &dwBytesWritten, NULL))
        {
          Logger::getLogger()->Log("Buffer not written. Error %u",  GetLastError());
          return result;
        }
      }
      else
      {
        Logger::getLogger()->Log("%s is empty", filename);
        return originalReadFile(
              hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
      }
      //Check if we already have a handle open
      //   if(m_outHandle == NULL || m_outHandle == INVALID_HANDLE_VALUE)
      //   {
      //     int counter = 0;
      //     while(MatFile::CFileExists(newFilePath.c_str()))
      //     {
      //       newFilePath += std::to_string(counter);
      //       counter++;
      //     }
      //   }
    }
    else
    {
      OutputDebugString("Path buffer to small.");
    }
  }
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

