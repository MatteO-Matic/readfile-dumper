#include <fstream>
#include <iostream>
#include <vector>
#include <windows.h>
#include "zlib.h"
#include "unzipper.h"

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

//zlib test
void read5()
{
  FILE * readFile = fopen("somezip.zip", "rb");
  if( readFile == NULL )
  {
    cout << "Couldn't open input file for reading\n";
    return;
  }

  // get size of file
  fseek( readFile, 0, SEEK_END );
  ULONG fileLength = ftell( readFile );
  rewind( readFile );

  // allocate enough mems to hold entire file
  // alternatively, we could "memory map" the
  // file contents using the CreateFileMapping and
  // MapViewOfFile funcs.
  BYTE * dataReadInCompressed = (BYTE*)malloc( fileLength );

  // read in entire file
  fread( dataReadInCompressed, fileLength, 1, readFile );

  // close file
  fclose( readFile );
  readFile = NULL;

  cout << "*\n Some data from compressed file:\n";
  cout << "\n--\n";
  for( int i = 0 ; i < 300; i++ )
  {
    putchar( dataReadInCompressed[i] );
  }
  cout << "\n--\n\n";

}

//zipper read file from a zip
void read6()
{
  //Unzipper unzipper("somezip.zip");
  //std::vector<ZipEntry> entries = unzipper.entries();
  //unzipper.close();

  //print some.txt
  std::vector<unsigned char> unzipped_entry;
  zipper::Unzipper unzipper("somezip.zip");
  unzipper.extractEntryToMemory("some.txt", unzipped_entry);
  unzipper.close();

  std::cout << "Printing some text from somezip->some.txt\n";
  for(std::vector<unsigned char>::iterator it = unzipped_entry.begin();
      it != unzipped_entry.end(); ++it)
  {
    std::cout << *it;
  }
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
    read5();
    read6();
  }
  return 0;
}
