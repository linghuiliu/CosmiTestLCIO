#ifndef __DBInitString_hh__
#define __DBInitString_hh__

#include <stdexcept>
#include <cstring>
#include <sstream>

static const std::string DBInitString() {
#ifndef DB_INIT_STRING
  throw std::invalid_argument( "This code needs to be compiled with -DDB_INIT_STRING=\"<DB init string>\" in order to be functional" );
#else
  std::string ret = DB_INIT_STRING ;
  if ( strcmp( ret.c_str(), "" ) == 0 )
    throw std::runtime_error( "This code has been compiled with -DDB_INIT_STRING=\"\". A valid argument at compile time is mandatory for this code to work" );
  return ret;
#endif
}

#endif
