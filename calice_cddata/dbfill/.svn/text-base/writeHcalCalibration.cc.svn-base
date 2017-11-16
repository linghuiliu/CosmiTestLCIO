//  std headers
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>

//  LCIO headers
#include "lcio.h"
#include "UTIL/LCTime.h"

//  CALICE headers
#include <CalibrationWriter.hh>
#include <GainConstants.hh>
#include <MIPConstants.hh>
#include <InterConstants.hh>

using namespace std;
using namespace CALICE;

//  global variables
bool _gain = false;
bool _mip = false;
bool _ic = false;

bool fillCalibrationWriter(CalibrationWriter* calWriter, const string inputName, const string calType){
  /** @brief Read input file and copy data to CalibrationWriter
   */
  
  //  catch calWriter NULL pointer
  if (!calWriter){
    cout << "No pointer to valid CalibrationWriter object " << calWriter;
    return false;
  }
    
    
  //  open input file and catch missing files
  ifstream fin(inputName.c_str());
  if (!fin.is_open()){
    cout << "Could not open input file " << inputName << endl;
    return false;
  }
    
  //  local variables
  int m,a,c,s;
  float v,e;
  char buffer[256];
    
  //  loop over input file
  while(fin.good()){
    
    //  read one line, skip empty or comment lines
    fin.getline(buffer,sizeof(buffer));
    if (strcmp(buffer,"")==0 || buffer[0]=='#') continue;
    stringstream line(buffer);
    
    //  read module, chip, channel, value, error, status from line
    line >> m >> a >> c >> v >> e >> s;
    
    //  set module type  (magic numbers are evil...)
    short mType = a > 5 ? 1 : 0;   //  even for upper, odd for lower half
    if (m>30) mType+=2;            //  increase type by 2 for coarse modules
    //  build module index  (magic numbers are evil...)
    unsigned int mIndex = (m<<8) + mType;
    
    //  create LCCalibrationObject depending on calType and add to calWriter
    if (_gain){
      GainConstants* theConst = new GainConstants(a,c,v,e);
#ifdef VERBOSE_MODE
      theConst->print(cout);
#endif
      calWriter->putCalibration(mIndex,a,c,theConst);
    }else if (_mip){
      MIPConstants* theConst = new MIPConstants(a,c,v,e);
#ifdef VERBOSE_MODE
      theConst->print(cout);
#endif
      calWriter->putCalibration(mIndex,a,c,theConst);
    }else if (_ic){
      InterConstants* theConst = new InterConstants(a,c,v,e);
#ifdef VERBOSE_MODE
      theConst->print(cout);
#endif
      calWriter->putCalibration(mIndex,a,c,theConst);
    }
  } // loop over input file
  
  fin.close();
  return true;
}



lcio::LCTime readTime(const string& timeString){
  /**  @brief Interprete date/time string "YYYY-MM-DD hh:mm:ss"
   */
  if (timeString == "past") return lcio::LCTime(lccd::LCCDMinusInf);
  if (timeString == "future") return lcio::LCTime(lccd::LCCDPlusInf);
  int y=1970,m=1,d=1,hr=0,min=0,sec=0;
  char cdummy;
  stringstream timeStream(timeString.c_str());
  timeStream >> y >> cdummy >> m >> cdummy >> d >> cdummy >> hr >> cdummy >> min >> cdummy >> sec;
  lcio::LCTime t(y,m,d,hr,min,sec);
  return t;
}


void printHelp(){
  /** @brief Print help text
   */
  cout << endl
       << "  Program  writeHcalCalibration" << endl
       << endl
       << "Read flat file with Hcal MIP, Gain, or Intercalibration and store" << endl
       << "the contents in the respective database folders. The folder has the" << endl
       << "form /cd_calice/Hcal/<cal-type>/<subfolder> where <cal-type> is" << endl
       << "set from the --mip, --gain, or --ic option and <subfolder> is an" << endl
       << "optional parameter. One and only one option --mip, --gain, or --ic" << endl
       << "is required." << endl
       << endl
       << "Usage:" << endl
       << "   writeHcalCalibration [option [value]]" << endl
       << endl
       << "Options:" << endl
       << "  --help    : print this message" << endl
       << "  --mip     : calibration data is MIP calibration [ADC_p/MIP]" << endl
       << "  --gain    : calibration data is gain calibration [ADC_c/pix]" << endl
       << "  --ic      : calibration data is intercalibration [ADC_c/ADC_p]" << endl
       << "  --write   : test-run without DB access without this option" << endl
       << "Options with arguments: " << endl
       << "  --input <file>          : input file. One line per channel with format:" << endl
       << "                              <mod> <chip> <chan> <value> <error> <status>" << endl
       << "                              required, no sensible default" << endl
       << "  --from <time string>    : start of validity range; format: YYYY-MM-DD_hh:mm:ss" << endl
       << "                              default: \"past\" - validity starts at T_min" << endl
       << "  --until <time string>   : end of validity range; format: YYYY-MM-DD_hh:mm:ss" << endl
       << "                              default: \"future\" - validity ends at T_max" << endl
       << "  --db-init <init string> : DB init string" << endl
       << "                              required, no sensible default" << endl
       << "  --subfolder <name>      : DB subfolder" << endl
       << "                              default: \"\"" << endl
       << endl;

}



int main(int argc, char** argv){
  /** @brief Read flat file with Hcal calibration data and write content to database

      @author  Niels.Meyer@desy.de
      @date    23. Nov. 2007
      @version 0.1.0

      @todo    Implement possibility to write DB-like LCIO file
  */

  //  default values for optional parameters
  string dbinit = "";
  bool dbinitSet = false;
    //"flccaldb02.desy.de:calice:calicedb:bh7+4FUw:3306";
  string folder = "/cd_calice/Hcal";
  string subfolder = "";
  string inputFile = "";
  bool inputFileSet = false;
  
  string calType = "unknown";
  string fromTime = "past";
  string untilTime = "future";

  bool writeDB = false;
  
  //  interprete options
  for (int i=1; i!=argc; ++i){ 
    if (strcmp(argv[i],"--gain")==0) {calType = "Gain"; _gain=true;}
    else if (strcmp(argv[i],"--mip")==0) {calType = "MIP"; _mip=true;}
    else if (strcmp(argv[i],"--ic")==0) {calType = "Inter"; _ic=true;}
    else if (strcmp(argv[i],"--write")==0) {writeDB = true;}
    else if (strcmp(argv[i],"--subfolder")==0 && i<argc-1) {subfolder = string("/")+string(argv[++i]);}
    else if (strcmp(argv[i],"--from")==0 && i<argc-1) {fromTime = string(argv[++i]);}
    else if (strcmp(argv[i],"--until")==0 && i<argc-1) {untilTime = string(argv[++i]);}
    else if (strcmp(argv[i],"--db-init")==0 && i<argc-1) {dbinit = string(argv[++i]);dbinitSet = true;}
    else if (strcmp(argv[i],"--input")==0 && i<argc-1) {inputFile = string(argv[++i]);inputFileSet = true;}
    else if (strcmp(argv[i],"--help")==0) {printHelp(); return(0);}
    else {cout << "Ignoring unknown option " << argv[i] << endl;}
  }

  if ((!_mip && !_gain && !_ic) || (_mip&&_gain) || (_mip&&_ic) || (_gain&&_ic)){
    cout << "ERROR - please specify one and only one of options --mip, --gain, or --ic" << endl;
    return 1;
  }

  //  catch missing db init string
  if (!dbinitSet){
    cout << "ERROR - specification of --db-init <db init string> is mendatory" << endl;
    return 1;
  }

  //  catch unspecified input file
  if (!inputFileSet){
    cout << "ERROR - specification of --input <file> is mendatory" << endl;
    return 1;
  }
  
  //  put together complete DB folder and name string for CalibrationWriter
  string completeFolder = folder + string("/") + calType + subfolder;
  string calName = calType + string("Calibration");

  //  interprete time strings
  LCTime validFrom(readTime(fromTime));
  LCTime validUntil(readTime(untilTime));

  //  create CalibrationWriter
  CalibrationWriter* theWriter = new CalibrationWriter(dbinit, completeFolder, calName);

  //  call funtion to read file and fill CalibrationWriter, eventually flush CalibrationWriter to DB
  if (fillCalibrationWriter(theWriter,inputFile,calType)){
    if (writeDB){
      theWriter->flushCalibration(validFrom.timeStamp(), 
				  validUntil.timeStamp() + (validUntil.timeStamp()==lccd::LCCDPlusInf ? 0 : 1), 
				  false);
    }else{
      cout << "Test run only - nothing written to DB" << endl;
    }
  }else{
    cout << "ERROR - Problems with reading input file and/or filling CalibrationWriter. " << endl
	 << "*** " << (writeDB ? "Writing to database " : "Test run ") << "failed." << endl;
  }

  return 0;
}
