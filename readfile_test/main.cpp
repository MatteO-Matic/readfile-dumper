#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;


void read1()
{

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
}

void read2()
{
  string line;
  ifstream rfile("b.txt");
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
}

void read3()
{
  //read large from folder
  string line;
  ifstream rfile("files/large.txt");
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
}

//Read a full chunck into buffer
void read4()
{
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

int main()
{
  ofstream file;
  file.open("a.txt");
  for(int i = 0; i < 10;++i)
  {
    file << i << " :Hello world\n";
  }

  file.close();
  while(1)
  {
    cout << "Click me\n";
    string input;
    cin >> input;

    read1();
    read2();
    read3();
    read4();
    read1(); //multiple reads on same file

    return 0;
  }
}
