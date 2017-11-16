#ifndef _ReadLine_H_
#define _ReadLine_H_

#include <RtypesSubSet.h>

#include <iostream>
#include <stdexcept>
#include <vector>
using namespace std;

class ReadLine
{
public:
  ReadLine(UInt_t initial_line_buffer_size,const char *file_name) throw (runtime_error);
  ReadLine(UInt_t initial_line_buffer_size);

  ~ReadLine() {
    if (fIn!=&std::cin) delete fIn;
  };

  Bool_t Rewind();
  Bool_t ReadNextLine();

  Char_t *GetBuffer() {return &(fLineBuffer[0]);};
  UInt_t GetLineNumber() const {return fLineNr;};

private:
  istream *fIn;
  UInt_t fLineNr;

  std::vector<Char_t> fLineBuffer;
  
  static UInt_t gMaxLineBufferSize;
  static UInt_t gMinLineBufferSize;
};

#endif
