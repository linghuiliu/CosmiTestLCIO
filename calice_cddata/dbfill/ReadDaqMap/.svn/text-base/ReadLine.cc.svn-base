#include <ReadLine.hh>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstring>

UInt_t ReadLine::gMinLineBufferSize=128;
UInt_t ReadLine::gMaxLineBufferSize=1024*1024;

ReadLine::ReadLine(UInt_t initial_line_buffer_size,const char *file_name) throw (runtime_error)
  : fLineNr(0)
{
  fIn=new std::ifstream(file_name);
  if (!fIn || !(*fIn)) {
    std::stringstream message;
    message << "ReadLine::ctor> Failed to open file " << file_name;
    throw runtime_error(message.str());
  }
  if (initial_line_buffer_size<gMinLineBufferSize) initial_line_buffer_size=gMinLineBufferSize;
  fLineBuffer.resize(initial_line_buffer_size);
}


ReadLine::ReadLine(UInt_t initial_line_buffer_size)
  : fLineNr(0)
{
  fIn=&std::cin;
  if (initial_line_buffer_size<gMinLineBufferSize) initial_line_buffer_size=gMinLineBufferSize;
  fLineBuffer.resize(initial_line_buffer_size);
}

Bool_t ReadLine::Rewind()
{
  if (fIn) {
    fIn->seekg(0);
    return fIn->fail();
  }
  return false;
}

Bool_t ReadLine::ReadNextLine()
{
  //  fLineLength=0;
  if (fLineBuffer.size()==0) {
    fLineBuffer.resize(gMinLineBufferSize);
  }
  if (*fIn) {
    // read one line to the temporary buffer. If the buffer is too small to capture one complete line the 
    // buffer is grown automatically.
    fLineNr++;
    UInt_t offset=0;
    UInt_t chunk_size=fLineBuffer.size()/4;
    for(;;) {
      fIn->getline(&(fLineBuffer[offset]),fLineBuffer.size()-offset-1);
      if (fIn->eof()) break;
      if (!fIn->fail()) break;
      if (fLineBuffer.size()+chunk_size>gMaxLineBufferSize) {
	std::cerr << "ReadLine::ReadLine>"
	     << "The file contains an extremely long line (line number " << fLineNr 
	     << ", length > " << fLineBuffer.size()-1
             << "). Verify the file and adapt gMaxLineBufferSize if the file is alright. ABORT." 
	     << std::endl;
	return false;
      }
      fLineBuffer.resize(fLineBuffer.size()+chunk_size);
      offset+=strlen(&(fLineBuffer[offset]));
      fIn->clear();
    }
    //    fLineBuffer[offset]='\0';
    //    fLineLength=offset;
  }
  return !fIn->fail() && !fIn->eof();
}

