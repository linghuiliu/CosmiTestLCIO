
#ifndef BmlSlowRunDataBlock_hh
#define BmlSlowRunDataBlock_hh 1

#include "lcio.h"
#include "DaqTypeDataBlock.hh"
//#include "LCGenericObjectImplExt.hh"
#include "UTIL/LCTime.h"
#include "collection_names.hh"
#include <sys/time.h>
#include <map>
#include <cmath>

#ifdef VALUE_CHECK
#  include <cassert>
#endif

using namespace lcio;
using namespace std;


namespace CALICE {

 
  /**The map which contains the measured values of the different data types*/
  //typedef std::map<std::string, std::vector<double> > BmlSlowRunDataDblMap_t;   
  //typedef std::map<std::string, std::vector<int> > BmlSlowRunDataIntMap_t;   
  //typedef std::map<std::string, std::vector<float> > BmlSlowRunDataFloatMap_t;   

  /** The indices of the the stored values.*/
  enum BmlSroRunIntValues {kIntBmlSroRunPosTimeStampymd, kIntBmlSroRunPosTimeStamphms,
			   kIntBmlSroRunPosSextCur, 
                           kIntBmlSroRunPosBendCur=kIntBmlSroRunPosSextCur+2, 
                           kIntBmlSroRunPosColPos=kIntBmlSroRunPosBendCur+2,
                           kIntBmlSroRunPosQuadCur=kIntBmlSroRunPosColPos+2, 
                           kIntBmlSroRunPosTrimCur=kIntBmlSroRunPosQuadCur+2, 
                           kIntBmlSroRunPosH6aExpCount=kIntBmlSroRunPosTrimCur+2,
                           kIntBmlSroRunPosH6bExpCount=kIntBmlSroRunPosH6aExpCount+2,
                           kIntBmlSroRunPosH6cExpCount=kIntBmlSroRunPosH6bExpCount+2,
                           kIntBmlSroRunPosRpExpCount=kIntBmlSroRunPosH6cExpCount+2,
                           kIntBmlSroRunPosScintCount=kIntBmlSroRunPosRpExpCount+2,
                           kIntBmlSroRunIntValues};
  
  enum BmlSroRunDblValues {kDblBmlSroRunPosAbsPosition,
                           kDblBmlSroRunPosT4Position,
                           kDblBmlSroRunPosTargetPosition,
                           kIntBmlSroRunDblValues};

  //for completeness ...
  enum BmlSroRunFloatValues {kIntBmlSroRunFloatValues};
  /** 
   * Interface Class to access the BmlSlowRun Data 
   * Here we handle ahc voltages, temperatures et al.
   * when filling the class the string identifying the measured values
   * has to be given first, a warning is issued if no indentifier
   * is given for a given datatype. On the other hand this
   * concerns just the filler of the class, i.e. the conversion
   * where it is done once for all. 
   * See the print method at the end of the class
   * for the meaning of the fields (as of 31/7/06) 
   * Use the interface methods to retrieve the data fields
   * Note that the implemented functionality is essentially kept for backward compatibility only
   * i.e. that if ever the cern 06/07 data will be converted again the data will be written in the same way
   * into the db as before.
   * In principle the functionality like writing to the db is to be made by the base class.
   * Beamline parameters for datataking period > 2007 should be written into the db using the methods of the 
   * base class.
   * @author: Roman Pöschl LAL/Orsay
   * @date Aug  2006, modified Sept. 2008
   */
  class BmlSlowRunDataBlock : public CALICE::DaqTypeDataBlock {
    

  public: 
    
    /** Constructor */
     BmlSlowRunDataBlock() {
      //Initialze the type map(s)
      setTypes();
      //do some initialization 
      setAbsorberPosition(0.);    
      setT4Position(0.);
      setTargetPosition(0.);
      setTimeStamp(NULL);
      //Reset the output vectors
      _sextapoleCurrents.resize(0);
      _bendCurrents.resize(0);
      _colPositions.resize(0);
      _quadrupoleCurrents.resize(0); 
      _trimCurrents.resize(0);
      _h6aExpCounts.resize(0);
      _h6bExpCounts.resize(0);
      _h6cExpCounts.resize(0);
      _rpExpCounts.resize(0);
      _scintCounts.resize(0);
      //Loop over the map with the defined types and initialize the
      //position in the LCIO tuple and number values to -1
      //initialize the dbl values
      for ( BmlSroTypesDblMap_t::iterator types_iter = _theDblTypes.begin(); types_iter != _theDblTypes.end(); types_iter++) setNumPos( (*types_iter).second.first, -1 , -1); 
      //initialize the int values
      for ( BmlSroTypesIntMap_t::iterator types_iter = _theIntTypes.begin(); types_iter != _theIntTypes.end(); types_iter++) setNumPos( (*types_iter).second.first, -1 , -1); 
      //initialize the float values
      for ( BmlSroTypesFloatMap_t::iterator types_iter = _theFloatTypes.begin(); types_iter != _theFloatTypes.end(); types_iter++) setNumPos( (*types_iter).second.first, -1 , -1); 
      
      
      
    };
    
    
    //Set Parameters of the board by which the data were taken  
    /** 'Copy constructor' needed to interpret LCCollection read from file/database.
     */
    BmlSlowRunDataBlock(LCObject* obj) :  DaqTypeDataBlock(obj, false) { 
      //Reset the output vectors
      _sextapoleCurrents.resize(0);
      _bendCurrents.resize(0);
      _quadrupoleCurrents.resize(0); 
      _trimCurrents.resize(0);
      _h6aExpCounts.resize(0);
      _h6bExpCounts.resize(0);
      _h6cExpCounts.resize(0);
      _rpExpCounts.resize(0);
      _scintCounts.resize(0);
      //Initialize thw type map(s)
      setTypes();
      //... and prepare the output vectors
      prepareOutputVecs();
    } 
    
    
    /** Store the aborber position 
      * device in cern h6b beamline to purify beam, ie. to get rid of electrons
      */
    BmlSlowRunDataBlock & setAbsorberPosition(double abspos) {
      obj()->setDoubleVal(kDblBmlSroRunPosAbsPosition, abspos);
      return *this;
      }
    
    /** Retrieve the absorber position*/
    double getAbsorberPosition() const {
      return getDoubleVal(kDblBmlSroRunPosAbsPosition);
      };  
    
    
    /** Sets the time stamp
     * We store Year, Month, Day and Hour, Minute, Second
     * We store UTC!!!!
     */
    BmlSlowRunDataBlock& setTimeStamp(struct tm* timestamp) 
    {
      
      if(timestamp){
	//Timestamp year month day
	int timestampymd(0);
	timestampymd = timestampymd | ( ( (timestamp->tm_year + TM_YEARSTART) & 0xffff) << YEARSHIFT) 
	  | ( ( (timestamp->tm_mon + 1) & 0xff) << MONTHSHIFT)
	  | ( (  timestamp->tm_mday & 0xff) << DAYSHIFT);  
	
	
	//Timestamp hour minute second
	int timestamphms(0);
	timestamphms = timestamphms | ( (timestamp->tm_hour & 0xff) << HOURSHIFT)
	  | ( (timestamp->tm_min  & 0xff) << MINUTESHIFT)
	  | ( (timestamp->tm_sec  & 0xff) << SECONDSHIFT);  
	
	
	obj()->setIntVal(kIntBmlSroRunPosTimeStampymd, timestampymd);
	obj()->setIntVal(kIntBmlSroRunPosTimeStamphms, timestamphms);
      } else {
	obj()->setIntVal(kIntBmlSroRunPosTimeStampymd, 0);
	obj()->setIntVal(kIntBmlSroRunPosTimeStamphms, 0);
	
      }
      return *this;
    }
    
    
    /**returns the start stamp  
     */
     LCTime getTimeStamp() const
    {      
      int timestampymd(getIntVal(kIntBmlSroRunPosTimeStampymd));
      int timestamphms(getIntVal(kIntBmlSroRunPosTimeStamphms));
      
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
    
    /** Implementation of the method to obtain the timestamp as given by the beamline operators
        here identical to the only timestamp which was stored */
    LCTime getTimeStampBml() {return getTimeStamp();}
    
    /** Implementation of the method to obtain the timestamp as given by the calice daq 
        Here 0 as no timestamp has been stored*/
    LCTime getTimeStampDaq() {return 0;}
    
    /** Store the t4 position 
     * device in cern h6b beamline to produce desired particles out
     * of the secondary beam
     */
    BmlSlowRunDataBlock & setT4Position(double t4pos) {
      obj()->setDoubleVal(kDblBmlSroRunPosT4Position, t4pos);
      return *this;
    }
    
    /** Retrieve the absorber position*/
    double getT4Position() const {
      return getDoubleVal(kDblBmlSroRunPosT4Position);
    };  
    
    
    
    /** Store the target position 
        primary target hit by ythe SPS beam 
    */
    BmlSlowRunDataBlock & setTargetPosition(double targetpos) {
      obj()->setDoubleVal(kDblBmlSroRunPosTargetPosition, targetpos);
      return *this;
    }
    
    /** Retrieve the target position*/
    double getTargetPosition() const {
      return getDoubleVal(kDblBmlSroRunPosTargetPosition);
    };  
    
    
    /** Retrieve the sextapole currents
     * The values are stored in the sequence 'Measurement, Reference
     * e.g. _sextapoleCurrents[0] = measured value of sextupole 1 (counting from 1) 
     * e.g. _sextapoleCurrents[1] = reference value of sextupole 1 (counting from 1) 
     */
    //    std::vector<double>& getSextapoleCurrents() { return _sextapoleCurrents;}     
    const std::vector<double>& getSextapoleCurrents() const { return _sextapoleCurrents;}     

    /** Retrieve the bending magnet currents
     * @sa getSextapoleCurrents()    
     */
    //    std::vector<double>& getBendCurrents() { return _bendCurrents;}     
    const std::vector<double>& getBendCurrents() const { return _bendCurrents;}     
    
    /** Retrieve the collimator positions
     * The values are stored in the sequence 'Measurement x, Reference x, Meaurement y, Reference y 
     * e.g. _colPositions[0] = measured value of coll. 1 - x coord. (counting from 1) 
     * e.g. _colPositions[1] = reference value of coll. 1 - x coord. (counting from 1) 
     * e.g. _colPositions[2] = measured value of coll. 1 - y coord. (counting from 1) 
     * e.g. _colPositions[3] = reference value of coll. 1 - y coord. (counting from 1) 
    */
    //    std::vector<double>& getCollimatorPositions() { return _colPositions;}     
    const std::vector<double>& getCollimatorPositions() const { return _colPositions;}     

    /** Retrieve the quadrupole currents
     * @sa getSextapoleCurrents()    
     */
    //    std::vector<double>& getQuadrupoleCurrents() { return _quadrupoleCurrents;}     
    const std::vector<double>& getQuadrupoleCurrents() const { return _quadrupoleCurrents;}     

    /** Retrieve the trim currents
     * @sa getSextapoleCurrents()    
     */
    //  std::vector<double>& getTrimCurrents() { return _trimCurrents;}     
    const std::vector<double>& getTrimCurrents() const { return _trimCurrents;}     

    /** Retrieve the h6a experiment count*/
    //  std::vector<int>& getH6aExperimentCounts() { return _h6aExpCounts;}     
    const std::vector<int>& getH6aExperimentCounts() const { return _h6aExpCounts;}     

    /** Retrieve the h6b experiment count*/
    //  std::vector<int>& getH6bExperimentCounts() { return _h6bExpCounts;}     
    const std::vector<int>& getH6bExperimentCounts() const { return _h6bExpCounts;}     

    /** Retrieve the h6c experiment count*/
    //  std::vector<int>& getH6cExperimentCounts() { return _h6cExpCounts;}     
    const std::vector<int>& getH6cExperimentCounts() const { return _h6cExpCounts;}     

    /** Retrieve the rp experiment count*/
    //  std::vector<int>& getRpExperimentCounts() { return _rpExpCounts;}     
    const std::vector<int>& getRpExperimentCounts() const { return _rpExpCounts;}     

    /** Retrieve the scintillator count*/
    //  std::vector<int>& getScintillatorCounts() { return _scintCounts;}     
    const std::vector<int>& getScintillatorCounts() const { return _scintCounts;}     


    /** These methods receive the vectors with the measured values
     * Mainly to store large arrays of a given datatype but in
     * principle this can be extended to every entry in the interface class
     */
    void setDblArrays(const DaqTypeDataDblMap_t&);
    void setFloatArrays(const DaqTypeDataFloatMap_t&);
    void setIntArrays(const DaqTypeDataUIntMap_t&);

    
    /** Implementation of the method to get the beam energy*/    
    const double getBeamEnergy() {
     return static_cast<double>(fabs(getBendCurrents().at(8))/4.72*1000);
    }

    /** convenient print method*/
    void print(std::ostream&);
    
    
    
    // -------- need to implement abstract methods from LCGenericObject
    
    /** Return the type of the class 
     */
    const std::string getTypeName() const { 
      return"BmlSlowRunDataBlock" ;
    } 
    
    /** Return a brief description of the data members 
     */
    const std::string getDataDescription() const {
      return
    "i:timestamp_year_month_day,i:timestamp_hour_minute_second,d:absorberposition,d:t4position,d:targetposition,i:posbendcurmeas,i:nbendcurmeas,i:poscollimposmeas,i:ncollimposmeas,i:posquadcurmeas,i:nquadcurmeas,i:possextcurmeas,i:nsextcurmeas.i:postrimcurmeas,i:ntrimcurmeas,i:posh6aexpcntmeas,i:nh6aexpcntmes,i:posh6bexpcntmeas,i:nh6bexpcntmeas,i:posh6cexpcntmeas,i:nh6cexpcntmeas,i:posrpexpcntmeas,i:nrpexpcntmeas,i:posscntcntmeas,i:nscntcntmeas,d[nbendcurmeas]:bendcurrents,d[ncollimposmeas]:collimatorpositions,d[nquadcurmeas]:quadrupolecurrents,d[nsextcurmeas]:sextapolecurrents,d[ntrimcurmeas]:trimcurrents,i[nh6aexpcntmes]:h6aexperimentcounts,i[nh6bexpcntmes]:h6bexperimentcounts,i[nh6cexpcntmes]:h6cexperimentcounts,i[nrpexpcntmes]:rpexperimentcounts,i[nscntcntmes]:scintillatorcounts" ;
    }
    
  private:
    
    /**Store the position and number of measured values for a given datatype*/ 
    void setNumPos(int posindex, int pos, int numvals){
      obj()->setIntVal(posindex, pos);
      obj()->setIntVal(posindex+1, numvals);
    }

    /**get the position of the given datatype within the array of values*/
    int getPos(int posindex) const {
      return getIntVal(posindex);
      }
    
    /** Retrieve the number of measured values for a given datatype*/
    int getNum(int posindex) const {
      return getIntVal(posindex+1);
      }

  /** Relation between the datatype its position among the tuple of double vals and the vector the data are readinto*/
      typedef std::map<string, std::pair<int, std::vector<double>* > > BmlSroTypesDblMap_t;
      typedef std::map<string, std::pair<int, std::vector<int>* > > BmlSroTypesIntMap_t;
      typedef std::map<string, std::pair<int, std::vector<float>* > > BmlSroTypesFloatMap_t;

    /** The vector which contains the types and to the position and values of the types 
        doubles */
    BmlSroTypesDblMap_t _theDblTypes;
    /*ints*/  
    BmlSroTypesIntMap_t _theIntTypes;
    /*floats*/
    BmlSroTypesFloatMap_t _theFloatTypes;

    /**This method defines the (general) types of the measured values
       A type not present here will not be treated!!!
     */
    void setTypes();     

  /**A method which prepares the output vectors*/
    void prepareOutputVecs();
    
    /**The output vectors*/
    /*Sextapole Magnets Current */  
    std::vector<double> _sextapoleCurrents;
    /**Bending Magnets currents*/
    std::vector<double> _bendCurrents; 
    /**Collimator Positions */
    std::vector<double> _colPositions; 
    /**Quadrupole Magnets Current*/
    std::vector<double> _quadrupoleCurrents;
    /**Trim Magnets Current*/
    std::vector<double> _trimCurrents;
    

    /*H6A Experiment Counts*/  
    std::vector<int> _h6aExpCounts;
    /*H6B Experiment Counts*/  
    std::vector<int> _h6bExpCounts;
    /*H6C Experiment Counts*/  
    std::vector<int> _h6cExpCounts;
    /*RP Experiment Counts*/  
    std::vector<int> _rpExpCounts;
    /*Scintillator Counts*/  
    std::vector<int> _scintCounts;


    
  }; // class
  
}
#endif 
