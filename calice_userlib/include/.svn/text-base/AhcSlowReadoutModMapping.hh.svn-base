#ifndef __AhcSlowReadoutModMapping_hh__
#define __AhcSlowReadoutModMapping_hh__ 1

#include "UTIL/LCFixedObject.h"
#include "EVENT/LCObject.h"
#include <string>



namespace CALICE{



  /**  @brief  Class to store the mapping corrections for the AhcSlowReadoutModBlock.
   *
   *   The slow control assembled the data from different modules into the slow control
   *   data submitted to the DAQ during 2006 and 2007. This bug was only fixed in the
   *   beginning of 2008 data taking at FNAL. This class holds the relation between data
   *   from the module and under which label it was stored.
   *
   *   As the data comes from three different hardware sources (CMB, HV supply and LV)
   *   the record holds three numbers for the sources. If the data for one source is
   *   lost due to mapping errors, the reading will be -1.
   *
   *   @author Benjamin.Lutz@desy.de
   *   @version 1.0
   *   @date   November 2008
   */
  class AhcSlowReadoutModMapping : public UTIL::LCFixedObject<4, 0, 0> {
  public:
    /**  Constructor with initial values
     */
    AhcSlowReadoutModMapping( const int module, const int CMBrecordLabel, const int HVrecordLabel, const int LVrecordLabel) {
      obj()->setIntVal(0,module);
      obj()->setIntVal(1,CMBrecordLabel);
      obj()->setIntVal(2,HVrecordLabel);
      obj()->setIntVal(3,LVrecordLabel);
    }

    /**  Constructor from LCObject
     */
    AhcSlowReadoutModMapping( EVENT::LCObject* object) : UTIL::LCFixedObject<4,0,0>( object ){
    }

    /**  Destructor
     */
    ~AhcSlowReadoutModMapping(){
    }

    void setModule(int module);
    void setCMBlabel(int label);
    void setHVlabel(int label);
    void setLVlabel(int label);

    /**
     * @brief get the module number for which this record is
     */
    int getModule() const;

    /**
     * @brief get the label of the AhcSlowReadoutModBlock holding the data with source CMB
     *
     * @remarks returns -1 when the data was lost due to mapping errors
     */
    int getCMBlabel() const;

    /**
     * @brief get the label of the AhcSlowReadoutModBlock holding the data with source HV supply
     *
     * @remarks returns -1 when the data was lost due to mapping errors
     */
    int getHVlabel() const;

    /**
     * @brief get the label of the AhcSlowReadoutModBlock holding the data with source LV monitoring
     *
     * @remarks returns -1 when the data was lost due to mapping errors
     */
    int getLVlabel() const;

    const std::string getTypeName() const;
    const std::string getDataDescription() const;

  };  // class AhcSlowReadoutModMapping


}  // namespace CALICE

#endif

