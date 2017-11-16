#ifdef HAVE_CONFIG_H
#  include <config.h> 
#endif 

#include <UTIL/LCTime.h>
#include <IO/LCReader.h>
#include <EVENT/LCEvent.h>
#include <Exceptions.h>
#include <lcio.h>

#include <lccd.h>

#include <IOIMPL/LCFactory.h>
#ifdef USE_LCIO_CALICERAW
#include <LCMetaReader.h>
#endif

#include "parseDateTime.hh"
#include "getRunStartTime.hh"
#include "ReadLine.hh"
#include "TLineIterator.hh"

#include <sstream>
#include <iostream>
#include <map>

IO::LCReader *get_reader(const std::vector<std::string> &data_dir_name, UInt_t a_run_nr) 
  throw(std::runtime_error)
{
#ifdef USE_LCIO_CALICERAW
  // with lcio-caliceraw but witout lcioext:
  IO::LCReader *reader=new IOIMPL::LCMetaReader;
#else
  IO::LCReader *reader= IOIMPL::LCFactory::getInstance()->createLCReader();
#endif

  std::vector<std::string>::const_iterator  directory_iter=data_dir_name.begin();
  for (;directory_iter!=data_dir_name.end();directory_iter++) {
    
    std::stringstream file_name;
    file_name << *directory_iter << "/Run" << a_run_nr;
    try {
      reader->open(file_name.str());
      return reader;
    }
    catch (lcio::IOException &error) {
      std::cerr << "Did not find run " << a_run_nr << " in directory " << *directory_iter << std::endl;
    }
  }
  if (directory_iter==data_dir_name.end()) {
    std::stringstream message;
    message << "Did not find run " << a_run_nr;
    throw std::runtime_error(message.str());
  }
  return 0;
}

UTIL::LCTime run_start_dummy_time(lccd::LCCDMinusInf);

class RunStartListImpl : public RunStartList
{
public:
  RunStartListImpl(const char *file_name) {init(file_name);};

  void init(const char *file_name);

  const RunInfo_t *getRunInfo(UInt_t run_i) {
    std::map<UInt_t, RunInfo_t>::const_iterator run_iter=_runList.find(run_i);
    if ( run_iter == _runList.end()) {
      return 0;
    }
    return &(run_iter->second);
  };

private:
  std::string _fileName;
  static RunStartList *__instance;
  std::map<UInt_t, RunInfo_t> _runList;
};


void RunStartListImpl::init(const char *file_name)
{
  if (file_name == _fileName && _runList.size()!=0) return;
  
  ReadLine run_list_in(200,file_name);
  try {
    while (run_list_in.ReadNextLine()) {
      TLineIterator line_iter(run_list_in.GetBuffer());
      if (line_iter.IsEmpty()) continue;    //skip empty lines
      if (line_iter.IsComment()) continue;  //skip header
      UInt_t run_nr=line_iter.GetUnsignedInteger();
      UInt_t n_events=line_iter.GetUnsignedInteger();
      UTIL::LCTime start( parseDateTime(line_iter.GetWord(true), line_iter.GetWord(true)) );
      UTIL::LCTime end  ( parseDateTime(line_iter.GetWord(true), line_iter.GetWord(true)) );
      _runList.insert(make_pair(run_nr,RunInfo_t(n_events,start,end)));
    }
  }  
  catch( std::runtime_error &err) {
    std::cerr << "Error in " << file_name << ":" << run_list_in.GetLineNumber() << " :: " << err.what() << std::endl;
    exit(-1);
  }
  _fileName=file_name;
}


RunStartList *RunStartList::__instance=0;

RunStartList *RunStartList::instance(const char *file_name) 
{
  if (!__instance) {
    __instance=new RunStartListImpl(file_name);
  }
  else {
    __instance->init(file_name);
  }
  return __instance;
}

UTIL::LCTime getRunStartTime(const char *run_times_list_file_name, const std::vector<std::string> &data_dir_name, UInt_t run_nr)
{
  const RunInfo_t *info=RunStartList::getRunInfo(run_times_list_file_name,run_nr);
  if (info) {
    return info->start();
  }
  else {
    
    IO::LCReader *reader=get_reader(data_dir_name,run_nr);
    UTIL::LCTime run_start(run_start_dummy_time);
    //    try {
    EVENT::LCEvent *evtP=reader->readNextEvent();
    run_start=evtP->getTimeStamp();
      //    }
    //    catch (IO::DataNotAvailableException &) {
    //    }
    //    catch (IO::EndOfDataException &) {
    //    }
    delete reader;
    return run_start;
  }
}
