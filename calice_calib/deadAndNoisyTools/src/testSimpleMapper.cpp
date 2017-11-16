#include "SimpleMapper.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    if (argc == 1) // no arguments given
  {
    std::cout << "Usage: testSimpleMapper AHCConfigFileName"<< std::endl;
    std::cout << "This programme instantiates a SimpleMapper object and prints the mapping." 
	      << std::endl;
    std::cout << "The input parameter is the ahc.cfg file name with the AHC configuration." 
	      << std::endl;
    return 1;
  }


  SimpleMapper mySimpleMapper(argv[1]);
  mySimpleMapper.print();
  return 0;
}
