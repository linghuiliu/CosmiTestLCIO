#ifndef SceSlowReadoutModBlock_hh
#define SceSlowReadoutModBlock_hh 1

#include "lcio.h"
#include "ScintCaloSlowReadoutModBlock.hh"
#include "LCGenericObjectImplExt.hh"
#include "UTIL/LCTime.h"
#include "collection_names.hh"
#include <sys/time.h>
#include <map>

#ifdef VALUE_CHECK
#  include <cassert>
#endif

using namespace lcio;
using namespace std;


namespace CALICE {

 
  /**The map which contains the measured values of the different types*/
  //typedef std::map<std::string, std::vector<double> > SceSlowMeasuresDblMap_t;   
  //typedef std::map<std::string, std::vector<int> > SceSlowMeasuresIntMap_t;   
  //typedef std::map<std::string, std::vector<float> > SceSlowMeasuresFloatMap_t;   

  /** The indices of the the stored values.
   */
  enum SceSroModIntValues {kIntSceSroModPosHbabCurs=kIntSroModPosCmbVolts+2, 
                           kIntSceSroModIntValues=kIntSroModPosHbabVolts+1};
  
  
  //double values
  enum SceSroModDblValues{kIntSceSroModDblValues}; 


  //for completeness
  enum SceSroModFloatValues{kSceIntSroModFloatValues};   

  /** 
   * Interface Class to access the SceSlowReadout Data 
   * Here we handle ahc voltages, temperatures et al.
   * when filling the class the string identifying the measured values
   * has to be given first, a warning is issued if no identifier
   * is given for a given datatype. On the other hand this
   * concerns just the filler of the class, i.e. the conversion
   * where it is done once for all. 
   * See the print method at the end of the class
   * for the meaning of the fields (as of 31/7/06) 
   * Use the interface methods to retrieve the data fields
   * @author: Roman Pöschl LAL/Orsay
   * @date Aug  2006
   */
  class SceSlowReadoutModBlock : public CALICE::ScintCaloSlowReadoutModBlock {
    

  public: 
    
    /** Constructor*/
    SceSlowReadoutModBlock() : ScintCaloSlowReadoutModBlock()
    {
      //Initialze the type map(s)
      setTypes();
      //Reset the output vectors
      _hbabCurrents.resize(0);
    };
    
    
    /** 'Copy constructor' needed to interpret LCCollection read from file/database.
     */
    SceSlowReadoutModBlock(LCObject* obj) : ScintCaloSlowReadoutModBlock(obj) { 
      //Reset the output vectors
      _hbabCurrents.resize(0);
      //Initialize the type map(s)
      setTypes();
      //... and prepare the output vectors
      prepareOutputVecs();
    } 
    
    /** Retrieve the hbab Currents*/
    std::vector<double>& getHbabCurrents() { return _hbabCurrents;}     

    /** convenient print method*/
    void print(std::ostream&);
    
    
    // -------- need to implement abstract methods from LCGenericObject
    
    /** Return the type of the class 
     */
    virtual const std::string getTypeName() const { 
      return"SceSlowReadoutModBlock" ;
    } 
    
    /** Return a brief description of the data members 
     */
    virtual const std::string getDataDescription() const {
      return "i:modulenumber, i:timestamp_year_month_day,i:timestamp_hour_minute_second,i:ledsetting,i:cmbwidth,i:cmbheight,i:poscmbtempmeas,i:ncmbtempmeas,i:poscmbvoltsmeas,i:ncmbvoltsmeas,i:poshbabvoltssmeas,i:nhbabvoltssmeas,i:poshbabcurssmeas,i:nhbabcurssmeas,d[ncmbtempsmeas]:cmbtemperatures,d[ncmbvoltsmeas]:cmbvoltages,d[nhbabvoltmeas]:hbabvoltages,d[nhbabcurmeas]:hbabcurrents" ;
    }
    
  private:
    
    

    /**This method defines the (general) types of the measured values
      A type not present here will not be handled!!!
    */
    void setTypes();     

    
    /**The output vectors*/
    /**hbab temperatures*/
    std::vector<double> _hbabCurrents;
    
    
  }; // class
  
}
#endif 
