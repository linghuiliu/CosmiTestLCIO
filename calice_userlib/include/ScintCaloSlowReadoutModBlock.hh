#ifndef ScintCaloSlowReadoutModBlock_hh
#define ScintCaloSlowReadoutModBlock_hh 1

#include "lcio.h"
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
  typedef std::map<std::string, std::vector<double> > ScintCaloSlowMeasuresDblMap_t;   
  typedef std::map<std::string, std::vector<int> > ScintCaloSlowMeasuresIntMap_t;   
  typedef std::map<std::string, std::vector<float> > ScintCaloSlowMeasuresFloatMap_t;   

  /** The indices of the the stored values.
   */
  /*enum ScintCaloSroModIntValues {kIntSroModModNumber, kIntSroModTimeStampymd, kIntSroModTimeStamphms,
                           kIntSroModLedSetting,
			   kIntSroModCmbWidth, 
                           kIntSroModCmbHeight,
			   kIntSroModPosCmbTemps, 
                           kIntSroModPosCmbVolts=kIntSroModPosCmbTemps+2, 
                           kIntSroModPosCmbVals=kIntSroModPosCmbVolts+2,
                           kIntSroModPosHbabTemps=kIntSroModPosCmbVals+2, 
                           kIntSroModPosHbabVolts=kIntSroModPosHbabTemps+2, 
                           kIntSroModIntValues};
  */ 

  enum ScintCaloSroModIntValues {kIntSroModModNumber, kIntSroModTimeStampymd, kIntSroModTimeStamphms,
				 kIntSroModLedSetting,
				 kIntSroModCmbWidth, 
				 kIntSroModCmbHeight,
				 kIntSroModPosCmbTemps, 
				 kIntSroModPosCmbVolts=kIntSroModPosCmbTemps+2,
                                 kIntSroModPosHbabVolts=kIntSroModPosCmbVolts+6};
  
 
  //double values
  //enum ScintCaloSroModDblValues{kIntSroModDblValues}; 


  //for completeness
  //enum ScintCaloSroModFloatValues{kIntSroModFloatValues};   

  /** 
   * Interface Class to access the ScintCaloSlowReadout Data 
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
  class ScintCaloSlowReadoutModBlock : public LCGenericObjectImplExt {
    

  public: 
    
    /** Constructor*/
    ScintCaloSlowReadoutModBlock() {
      //do some initialization
      setModuleNumber(0);  
      setTimeStamp(NULL);
      setLedSetting(0);
      setCmbWidth(0); 
      setCmbHeight(0);

      //Reset the output vectors
      _cmbTemperatures.resize(0);
      _cmbVoltages.resize(0);
      _hbabVoltages.resize(0);

      //Loop over the map with the defined types and initialize the
      //position in the LCIO tuple and number values to -1
      //initialize the dbl values
      for ( ScintCaloSroTypesDblMap_t::iterator types_iter = _theDblTypes.begin(); types_iter != _theDblTypes.end(); types_iter++) setNumPos( (*types_iter).second.first, -1 , -1); 
      //initialize the int values
      for ( ScintCaloSroTypesIntMap_t::iterator types_iter = _theIntTypes.begin(); types_iter != _theIntTypes.end(); types_iter++) setNumPos( (*types_iter).second.first, -1 , -1); 
      //initialize the float values
      for ( ScintCaloSroTypesFloatMap_t::iterator types_iter = _theFloatTypes.begin(); types_iter != _theFloatTypes.end(); types_iter++) setNumPos( (*types_iter).second.first, -1 , -1); 
      
      
    };
    
    
    /** 'Copy constructor' needed to interpret LCCollection read from file/database.
     */
    ScintCaloSlowReadoutModBlock(LCObject* obj) : LCGenericObjectImplExt(obj) { 
      //Reset the output vectors
      _cmbTemperatures.resize(0);
      _cmbVoltages.resize(0);
      _hbabVoltages.resize(0);
    } 
    
    
    /** Store the module number*/
    ScintCaloSlowReadoutModBlock & setModuleNumber(int modnumber) {
      obj()->setIntVal(kIntSroModModNumber, modnumber);
      return *this;
      }
    
    /** Retrieve the module number*/
    int getModuleNumber() const {
      return getIntVal(kIntSroModModNumber);
      };  
    
    
    /** Sets the time stamp
     * We store Year, Month, Day and Hour, Minute, Second
     * We store UTC!!!!
     */
    ScintCaloSlowReadoutModBlock& setTimeStamp(struct tm* timestamp) 
    {
      
      //Timestamp year month day
      if(timestamp) {
	int timestampymd(0);
	timestampymd = timestampymd | ( ( (timestamp->tm_year + TM_YEARSTART) & 0xffff) << YEARSHIFT) 
	  | ( ( (timestamp->tm_mon + 1) & 0xff) << MONTHSHIFT)
	  | ( (  timestamp->tm_mday & 0xff) << DAYSHIFT);  
	
	
	//Timestamp hour minute second
	int timestamphms(0);
	timestamphms = timestamphms | ( (timestamp->tm_hour & 0xff) << HOURSHIFT)
	  | ( (timestamp->tm_min  & 0xff) << MINUTESHIFT)
	  | ( (timestamp->tm_sec  & 0xff) << SECONDSHIFT);  
	
	
	obj()->setIntVal(kIntSroModTimeStampymd, timestampymd);
	obj()->setIntVal(kIntSroModTimeStamphms, timestamphms);
      } else {
	obj()->setIntVal(kIntSroModTimeStampymd, 0);
	obj()->setIntVal(kIntSroModTimeStamphms, 0);
	
	
      }

      return *this;
    }
    
    
    /**returns the start stamp  
     */
    LCTime getTimeStamp() 
    {
      
      int timestampymd(obj()->getIntVal(kIntSroModTimeStampymd));
      int timestamphms(obj()->getIntVal(kIntSroModTimeStamphms));
      
      //composition of an intermediate LCTime object in order
      //to get everything up to seconds in nanosecs.
      LCTime init(  (timestampymd >> YEARSHIFT) & 0xffff,
		    (timestampymd >> MONTHSHIFT) & 0xff,     
		    (timestampymd >> DAYSHIFT) & 0xff,     
		    (timestamphms >> HOURSHIFT) & 0xff,     
		    (timestamphms >> MINUTESHIFT) & 0xff,     
		    (timestamphms >> SECONDSHIFT) & 0xff );
      LCTime finaltime(init.timeStamp());
      return finaltime;
      
      }
    

    /** Store the LED setting*/
    ScintCaloSlowReadoutModBlock& setLedSetting(int ledsetting) {
      obj()->setIntVal(kIntSroModLedSetting, ledsetting);
      return *this;
      } 

    
    
    /** Retrieve the LED setting*/
    int getLedSetting() const {
      return getIntVal(kIntSroModLedSetting);
    }
    
    /** Store the CMB width*/
    ScintCaloSlowReadoutModBlock & setCmbWidth(int cmbwidth) {
      obj()->setIntVal(kIntSroModCmbWidth, cmbwidth);
      return *this;
    }
    
    /** Retrieve the CMB width*/
    int getCmbWidth() const {
      return getIntVal(kIntSroModCmbWidth);
    }  
    
    /** Store the CMB height*/
    ScintCaloSlowReadoutModBlock & setCmbHeight(int cmbheight) {
      obj()->setIntVal(kIntSroModCmbHeight, cmbheight);
      return *this;
      }  
    
    /** Retrieve the CMB height*/
    int getCmbHeight() const {
      return getIntVal(kIntSroModCmbHeight);
      }  
    
    /** Retrieve the cmb temperatures*/
    std::vector<double>& getCmbTemperatures() { return _cmbTemperatures;}     

    /** Retrieve the cmb voltages*/
    std::vector<double>& getCmbVoltages() { return _cmbVoltages;}     
    
    /** Retrieve the hbab voltages*/
    std::vector<double>& getHbabVoltages() { return _hbabVoltages;}     


    /** These methods obtain the vectors with the measured values
     * Mainly to store large arrays of a given datatype but in
     * principle this can be extended to every entry in the interface class
     */
    void setDblArrays(const ScintCaloSlowMeasuresDblMap_t&);
    void setIntArrays(const ScintCaloSlowMeasuresIntMap_t&);
    void setFloatArrays(const ScintCaloSlowMeasuresFloatMap_t&);
    
    
    /** convenient print method*/
    void print(std::ostream&);
    
    
    
    // -------- need to implement abstract methods from LCGenericObject
    
    /** Return the type of the class 
     */
    virtual const std::string getTypeName() const = 0;


    virtual const std::string getDataDescription() const = 0;    
    
  private:
    
    /**Store the position and number of measured values for a given datatype*/ 
    void setNumPos(int posindex, int pos, int numvals){
      obj()->setIntVal(posindex, pos);
      obj()->setIntVal(posindex+1, numvals);
    }

    /**get the position of the given datatype within the array of values*/
    int getPos(int posindex) {
      return getIntVal(posindex);
      }
    
    /** Retrieve the number of measured values for a given datatype*/
    int getNum(int posindex) const {
      return getIntVal(posindex+1);
      }

  protected:

  /** Relation between the datatype its position among the tuple of double vals and the vector the data are readinto*/
      typedef std::map<string, std::pair<int, std::vector<double>* > > ScintCaloSroTypesDblMap_t;
      typedef std::map<string, std::pair<int, std::vector<int>* > > ScintCaloSroTypesIntMap_t;
      typedef std::map<string, std::pair<int, std::vector<float>* > > ScintCaloSroTypesFloatMap_t;

    /** The vector which contains the types and to the position and values of the types 
        doubles */
    ScintCaloSroTypesDblMap_t _theDblTypes;
    /*ints*/  
    ScintCaloSroTypesIntMap_t _theIntTypes;
    /*floats*/
    ScintCaloSroTypesFloatMap_t _theFloatTypes;

    /**This method defines the (general) types of the measured values
       and is specific to each data type*/
    virtual void setTypes()=0;     
    
    /**A method which prepares the output vectors*/
    void prepareOutputVecs();
    
    /**The output vectors which are common to all data types
      cmb Temperatures */  
    std::vector<double> _cmbTemperatures;
    /**cmb Voltages*/
    std::vector<double> _cmbVoltages; 
    /**hbab voltages*/
    std::vector<double> _hbabVoltages;
    
    /** Variables to indicate the start of the actual data */
    unsigned _startOfDoubles;    
    unsigned _startOfFloats;    
    unsigned _startOfInts;    

  }; // class
  
}
#endif 
