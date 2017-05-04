#pragma once
#include <windows.h>
#include <vector>
#include <string>

using namespace std;
namespace MatFile
{
  BOOL CFileExists(LPCTSTR szPath);
  void Split(vector<string> &tokens, const string &str, const string &delimiters = " ");
  bool FileExists(const std::string& name);
}
