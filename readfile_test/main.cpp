#include <windows.h>
#include <iostream>
#include <fstream>

using namespace std;

int main()
{
  ofstream file;
  file.open("a.txt");
  for(int i = 0; i < 10;++i)
  {
    file << i << " :Hello world\n";
  }

  file.close();


  ifstream rfile("a.txt");
  string line;
  if(rfile.is_open())
  {
    while(getline(rfile, line))
    {
      cout << line << '\n';
    }
    rfile.close();
  }
  else
  {
    cout << "Unable to open file";
  }
  return 0;
}
