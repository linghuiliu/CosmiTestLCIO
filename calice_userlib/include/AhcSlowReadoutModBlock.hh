#ifndef AhcSlowReadoutModBlock_hh
#define AhcSlowReadoutModBlock_hh 1

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
  typedef ScintCaloSlowMeasuresDblMap_t AhcSlowMeasuresDblMap_t;   
  typedef ScintCaloSlowMeasuresIntMap_t AhcSlowMeasuresIntMap_t;   
  typedef ScintCaloSlowMeasuresFloatMap_t AhcSlowMeasuresFloatMap_t;   

  /** The indices of the the stored values.
   */
  enum AhcSroModIntValues {kIntAhcSroModPosCmbVals=kIntSroModPosCmbVolts+2, 
                           kIntAhcSroModPosHbabTemps=kIntAhcSroModPosCmbVals+2, 
                           kIntAhcSroModIntValues=kIntSroModPosHbabVolts+1};
  
  
  //double values
  enum AhcSroModDblValues{kIntAhcSroModDblValues}; 


  //for completeness
  enum AhcSroModFloatValues{kAhcIntSroModFloatValues};   

  /** 
   * Interface Class to access the AhcSlowReadout Data 
   * Here we handle ahc voltages, temperatures et al.
   * when filling the class the string identifying the measured values
   * has to be given first, a warning is issued if no identifier
   * is given for a given datatype. On the other hand this
   * concerns just the filler of the class, i.e. the conversion
   * where it is done once for all. 
   * See the print method at the end of the class
   * for the meaning of the fields (as of 31/7/06) 
   * Use the interface methods to retrieve the data fields
   * @author Roman Pöschl LAL/Orsay
   * @date Aug  2006
   */
  class AhcSlowReadoutModBlock : public CALICE::ScintCaloSlowReadoutModBlock {
    

  public: 
    
    /** Constructor*/
    AhcSlowReadoutModBlock() : ScintCaloSlowReadoutModBlock()
    {
      //Initialze the type map(s)
      setTypes();
      //Reset the output vectors
      _cmbValues.resize(0);
      _hbabTemperatures.resize(0);
    };
    
    
    /** 'Copy constructor' needed to interpret LCCollection read from file/database.
     */
    AhcSlowReadoutModBlock(LCObject* obj) : ScintCaloSlowReadoutModBlock(obj) { 
      //Reset the output vectors
      _cmbValues.resize(0);
      _hbabTemperatures.resize(0);
      //Initialize the type map(s)
      setTypes();
      //... and prepare the output vectors
      prepareOutputVecs();
    } 
    
    /** Retrieve the cmb values*/
    std::vector<double>& getCmbValues() { return _cmbValues;}     

    /** Retrieve the hbab temperatures*/
    std::vector<double>& getHbabTemperatures() { return _hbabTemperatures;}     

    /** convenient print method*/
    void print(std::ostream&);
    
    
    // -------- need to implement abstract methods from LCGenericObject
    
    /** Return the type of the class 
     */
    virtual const std::string getTypeName() const { 
      return"AhcSlowReadoutModBlock" ;
    } 
    
    /** Return a brief description of the data members 
     */
    virtual const std::string getDataDescription() const {
      return "i:modulenumber, i:timestamp_year_month_day,i:timestamp_hour_minute_second,i:ledsetting,i:cmbwidth,i:cmbheight,i:poscmbtempmeas,i:ncmbtempmeas,i:poscmbvalsmeas,i:ncmbvalsmeas,i:poscmbvoltsmeas,i:ncmbvoltsmeas,i:poshbabtempsmeas,i:nhbabtempsmeas,i:poshbabtempsmea,d[ncmbtempsmeas]:cmbtemperatures,d[ncmbvoltsmeas]:cmbvoltages,d:[cmbvalsmeas]:cmbvaluess,d[nhbabtempmeas]:hbabtemperatures,d[nhbabvoltmeas]:hbabvoltages" ;
    }
    
  private:
    
    

    /**This method defines the (general) types of the measured values
      A type not present here will not be handled!!!
    */
    void setTypes();     

    
    /**The output vectors*/
    /**cmb values */
    std::vector<double> _cmbValues; 
    /**hbab temperatures*/
    std::vector<double> _hbabTemperatures;
    
    
  }; // class
  
}
#endif 
