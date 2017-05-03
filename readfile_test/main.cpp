#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

int main()
{
  while(1)
  {
    cout << "Click me\n";
    string input;
    cin >> input;

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



    rfile.open("b.txt");
    if(rfile.is_open())
    {
      while(getline(rfile, line))
      {

      }
      rfile.close();
    }
    else
    {
      cout << "Unable to open file";
    }
    //read large from folder
    rfile.open("files/large.txt");
    if(rfile.is_open())
    {
      string dummy;
      int counter = 0;
      while(getline(rfile, line))
      {
        dummy = line;
        counter++;
        if(counter % 500 == 0 )
        {
          cout << dummy << "\n";
        }
      }
      rfile.close();
    }
    else
    {
      cout << "Unable to open file";
    }


    //Read zip
    ifstream flarge("somezip.zip", ios::ate | ios::binary);
    if(flarge.is_open())
    {
      cout << "large file is open\n";
      ifstream::pos_type pos = flarge.tellg();
      vector<char> buffer(pos);
      flarge.seekg(0, ios::beg);
      flarge.read(&buffer[0], pos);
    }



    flarge.close();
  }
  return 0;
}
