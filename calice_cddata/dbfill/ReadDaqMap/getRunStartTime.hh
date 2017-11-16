#include <RtypesSubSet.h>
#include <UTIL/LCTime.h>
#include <IO/LCReader.h>
#include <EVENT/LCEvent.h>
#include <vector>
#include <string>
#include <stdexcept>

IO::LCReader *get_reader(const std::vector<std::string> &data_dir_name, UInt_t a_run_nr) 
  throw(std::runtime_error);

UTIL::LCTime getRunStartTime(const char *run_times_list_file_name, const std::vector<std::string> &data_dir_name, UInt_t run_nr);


class RunStartList;
class RunStartListImpl;

class RunInfo_t {
  friend class RunStartList;
  friend class RunStartListImpl;
protected:
  RunInfo_t(UInt_t n_events, const UTIL::LCTime &start, const UTIL::LCTime &end) : _nEvents(n_events), _startTime(start), _endTime(end) {};
  
public: 

  RunInfo_t(const RunInfo_t &a) : _nEvents(a.n()), _startTime(a.start()), _endTime(a.end()) {};
  UInt_t n() const {return _nEvents;};
  const UTIL::LCTime &start() const {return _startTime;};
  const UTIL::LCTime &end() const {return _endTime;};

protected:  
  UInt_t _nEvents;
  UTIL::LCTime _startTime;
  UTIL::LCTime _endTime;
};


class RunStartList
{
protected:
  RunStartList() {};
  virtual ~RunStartList() {};

public:
  virtual void init(const char *file_name) = 0;
  virtual const RunInfo_t *getRunInfo(UInt_t run_i) = 0;


  static const RunInfo_t *getRunInfo(const char *file_name, UInt_t run_i) {
    return instance(file_name)->getRunInfo(run_i);
  };
  
  static RunStartList *instance(const char *file_name);

private:
  static RunStartList *__instance;
};
