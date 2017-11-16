#ifndef CellParameter_hh
#define CellParameter_hh

#include "lcio.h"
#include "UTIL/LCFixedObject.h"
#include <sstream>

#ifdef BOUNDARY_CHECK
#  include <stdexcept>
#endif

namespace CALICE {

  // Float_t, UInt_t, Bool_t
#include <RtypesSubSet.h>
  // sqrt
#include <math.h>
  // FLT_MAX
#ifndef __APPLE__ 
#include <values.h>
#else
#include <limits.h>
#include <float.h>
#endif


  enum ECellParameterFloatType {
    kCellParameterFloatPedestal,
    kCellParameterFloatNoise,
    kCellParameterFloatOldPedestal,
    kCellParameterFloatOldNoise,
    kCellParameterFloatADCMemoryFirst,
    kCellParameterFloatADCMemory2,
    kCellParameterFloatADCMemory3,
    kCellParameterFloatADCMemory4,
    kCellParameterFloatADCMemoryLast,
    kNCellParameterFloats
  };
  enum ECellParameterIntType {
    kCellParameterIntNValues,
    kCellParameterIntSaturationCounter,
    kCellParameterIntHitCounter,
    kCellParameterIntNPedestalChanges,
    kCellParameterIntADCMemoryWritePos,
#ifdef WITH_CELL_PAR_INDEX
    kCellParameterIntModuleIndex,
    kCellParameterIntCellIndex,
#endif
    kNCellParameterInts
  };

  // ATTENTION: some storage cells are multipurpose storage cells
  // You cannot
  //  const unsigned short kCellParameterFloatSum=kCellParameterFloatOldPedestal;  /**< use the same storage cell for the sums as used to store the old pedestals
  //                                                                              (cannot be used after the pedestals are adjusted for the first time).*/
  //  const unsigned short kCellParameterFloatSum2=kCellParameterFloatOldNoise;    /**< use the same storage cell for the sums as used to store the old noise
  //                                                                              (cannot be used after the pedestals are adjusted for the first time).*/
  //  const unsigned short kCellParameterFloatMaxValue=kCellParameterFloatPedestal;/**< use the same storage cell for the maximum values as used to store the pedestal
  //                                                                              (cannot be used after the pedestals have been calculated for the first time).*/
#define kCellParameterFloatMaxValue kCellParameterFloatPedestal
#define kCellParameterFloatSum      kCellParameterFloatOldPedestal
#define kCellParameterFloatSum2     kCellParameterFloatOldNoise

  inline Double_t sqr(Double_t a) {return a*a;}

  /** Information about a pad: Pedestal, noise, etc.
   * This class hold the pedestal, the noise, the number of accumulated hits, a saturation
   * counter and a flag which indicates whether the pad is considered to be dead.
   */
  class CellParameter : public LCFixedObject<kNCellParameterInts,kNCellParameterFloats,0>
  {
  public:
    CellParameter() : LCFixedObject<kNCellParameterInts,kNCellParameterFloats,0>() { resetAll();};

    /** 'Copy constructor' needed to interpret LCCollection read from file/database.
     */
    CellParameter(LCObject* obj) : LCFixedObject<kNCellParameterInts,kNCellParameterFloats,0>(obj) { }

    ~CellParameter() {
    };

    /** Reset the sums which are used to calculate the pedestals and the noise.
     * This function should be called before ADC values are accumulated and added with \ref add
     * to the sums.
     */
    void resetSums() {setNValues(0); setSum(0), setSum2(0);};
    void resetSaturationCounter() {setSaturationCounter(0);};
    void resetHitCounter() {setNHits(0);};
    void resetAll()
    {setMaxValue( -1.* FLT_MAX);setNoise(FLT_MAX);resetHitCounter();resetSaturationCounter();resetSums();setNPedestalChanges(0);};


    /** Set the sum of the adc values (used to calculate the pedestal).
     * After this call the pedestals and the noise are invalid.
     */
    void setSum(Float_t a_sum) {obj()->setFloatVal( kCellParameterFloatSum,a_sum);};

    /** Get the sum of the adc values (used to calculate the pedestal).
     * The result is undefined after the noise and the pedestals have been calculated.
     */
    Float_t getSum() const {return getFloatVal( kCellParameterFloatSum);};


    /** Set the sum of the adc values squared (used to calculate the noise).
     * After this call the pedestals and the noise are invalid.
     */
    void setSum2(Float_t a_sum2) {obj()->setFloatVal( kCellParameterFloatSum2,a_sum2);};

    /** Get the sum of the adc values squared (used to calculate the noise).
     * The result is undefined after the noise and the pedestals have been calculated.
     */
    Float_t getSum2() const {return getFloatVal( kCellParameterFloatSum2);};


    /** Set the number of values added to the sums.
     */
    void  setNValues(UInt_t n_values) {obj()->setIntVal( kCellParameterIntNValues, static_cast<Int_t>(n_values));};

    /** Get the number of values added to the sums.
     */
    UInt_t getNValues() const {return static_cast<UInt_t>(getIntVal( kCellParameterIntNValues));};


    /** Set the pedestal.
     */
    void setPedestal(Float_t a_pedestal) {obj()->setFloatVal( kCellParameterFloatPedestal,a_pedestal);};

    /** Get the pedestal.
     * The result is undefined before the pedestals and the noise are calculated.
     */
    Float_t getPedestal() const {return getFloatVal( kCellParameterFloatPedestal);};


    /** Set the old pedestal.
     * The old pedestal shares the storage with the sum. Thus, after this call the sum is invalid.
     */
    void setOldPedestal(Float_t an_old_pedestal) {obj()->setFloatVal( kCellParameterFloatOldPedestal,an_old_pedestal);};

    /** Get the old pedestal
     * The result is undefined before the pedestals and the noise have been adjusted or calculated.
     * In the latter case the old and the "new" pedestals are the same.
     */
    Float_t getOldPedestal() const  {return getFloatVal( kCellParameterFloatOldPedestal);};


    /** Set the noise.
     */
    void setNoise(Float_t a_noise) {obj()->setFloatVal( kCellParameterFloatNoise,a_noise);};

    /** Get the noise.
     * The result is undefined before the pedestals and the noise are calculated.
     */
    Float_t getNoise() const  {return getFloatVal( kCellParameterFloatNoise);};

    /** Set the old noise.
     * The old noise shares the storage with the sum2. Thus, after this call the sum2 is invalid.
     */
    void setOldNoise(Float_t an_old_noise) {obj()->setFloatVal( kCellParameterFloatOldNoise,an_old_noise);};

    /** Get the old noise.
     * The result is undefined before the pedestals and the noise have been adjusted or calculated.
     * In the latter case the old and the "new" noise are the same.
     */
    Float_t getOldNoise() const  {return getFloatVal( kCellParameterFloatOldNoise);};


    /** set the maximum value for a cell (<b>shares storage with pedestal</b>).
     * The maximum value is stored in the same place as the pedestal.
     * Thus, the maximum value can only be used during the initial accumulation of
     * pedestal/noise statistics.
     *
     * kCellParameterFloatMaxValue and kCellParameterFloatPedestal point to the same cell
     */
    void setMaxValue(Float_t a_max_value) {
      obj()->setFloatVal( kCellParameterFloatMaxValue, a_max_value);
    };

    /** get the maximum value of a cell (shares storage with pedestal)
     * This function only returns the expected result before pedestals/noise
     * are calculated.
     */
    Float_t getMaxValue() const {
      return getFloatVal( kCellParameterFloatMaxValue);
    };

    /** Build sums used to calculate pedestals and noise.
     * The storage for the old pedestals and noise is abused to store the sums
     * @sa CellParameter::sub
     */
    void add(Float_t adc) {
      obj()->setFloatVal( kCellParameterFloatSum,   getFloatVal( kCellParameterFloatSum)  + adc      );
      obj()->setFloatVal( kCellParameterFloatSum2,  getFloatVal( kCellParameterFloatSum2) + adc *adc );
      obj()->setIntVal(   kCellParameterIntNValues, getIntVal(   kCellParameterIntNValues)+ 1        );
    };

    /** Subtract a previously added adc value from the sums to return to the prior condition.
     * @sa CellParameter::add
     */
    void sub(Float_t adc) {
      obj()->setFloatVal( kCellParameterFloatSum,   getFloatVal( kCellParameterFloatSum)  - adc      );
      obj()->setFloatVal( kCellParameterFloatSum2,  getFloatVal( kCellParameterFloatSum2) - adc *adc );
      obj()->setIntVal(   kCellParameterIntNValues, getIntVal(   kCellParameterIntNValues)- 1        );
    }

    /** Caluclate the pedestal and the noise from the sums.
     */
    void calculate() {
      const UInt_t n_values=getNValues();
      if (n_values>1) {
        const Float_t new_pedestal=getSum()/n_values;
        const Float_t new_noise=sqrt((getSum2()-new_pedestal*getSum())/(n_values-1));

        setPedestal(new_pedestal);
        setNoise(new_noise);

        setOldPedestal(new_pedestal);
        setOldNoise(new_noise);

      }
    };

    /** Adjust the pedestal and the noise using the new ADC value.
     * @param adc the new ADC value
     * @param keep_n_minus_one the current pedestal and noise are given a weight of this value + 1
     *        and the new ADC value get the weight 1
     *
     *  The current pedestal and the current noise are memorised (@ref getPedestal and @ref getNoise)
     *  and then they are updated.
     */
    void adjustPedestalNoise(Float_t adc, UInt_t keep_n_minus_one) {
      Float_t old_pedestal=getPedestal();
      Float_t old_noise=getNoise();
      setOldPedestal(old_pedestal);
      setOldNoise(old_noise);

      // 1) first calculate sum of squares and sum from noise and pedestal
      // 2) multiply sum of squares, and sum by (n-1)/n
      // 3) add the new value and the new value squared
      // 4) calculate new pedestal and new noise
      Float_t inv_n=1./(keep_n_minus_one+1);
      Float_t pedestal_sum=old_pedestal*keep_n_minus_one;
      Float_t new_sum2=sqr(old_noise*(keep_n_minus_one))*inv_n + old_pedestal*pedestal_sum + adc*adc;
      pedestal_sum+=adc;
      Float_t new_pedestal = pedestal_sum * inv_n;
      Float_t new_noise=sqrt((new_sum2-new_pedestal*pedestal_sum)/(keep_n_minus_one));

      setPedestal(new_pedestal);
      setNoise(new_noise);
    };


    /** Return kTRUE if the pad is considered to be dead.
     */
    Bool_t isDead() const {return ((UInt_t) getSaturationCounter())==((UInt_t) -1);};

    /** Declare the pad to be dead (or revive pad (is_dead=kFALSE).
     * The status whether a pad is dead is stored in the saturation counter. Thus,
     * a reviving a cell will reset the saturation counter to zero.
     */
    void setDead(Bool_t is_dead=true) {setSaturationCounter((is_dead ? (UInt_t) -1 : 0));};

  protected:
    /** Set the value of the saturation counter.
     * The saturation counter is increased when ever the ADC value is outside of a certain range
     */
    void setSaturationCounter(UInt_t saturation) {obj()->setIntVal( kCellParameterIntSaturationCounter, static_cast<Int_t>(saturation));};

  public:
    /** Get the value of the saturation counter.
     * The saturation counter is increased when ever the ADC value is outside of a certain range.
     * The saturation counter is used to mark cells as dead. Thus, it must not be incremented if
     * a cell is dead.
     */
    UInt_t getSaturationCounter() const {return static_cast<UInt_t>(getIntVal( kCellParameterIntSaturationCounter));};

    /** Increment the saturation counter.
     * The saturation counter is increased when ever the ADC value is outside of a certain range
     */
    void incrementSaturationCounter() {setSaturationCounter(getSaturationCounter()+1);};

  protected:
    /** Set the value of the hit counter.
     * The hit counter is increased when ever the pedestal subtracted ADC value is above a signal or signal-to-noise threshold.
     */
    void setNHits(UInt_t n_hits) {obj()->setIntVal( kCellParameterIntHitCounter, static_cast<Int_t>(n_hits));};
  public:

    /** Get the number of hits.
     * The hit counter is increased when ever the pedestal subtracted ADC value is above a signal or signal-to-noise threshold.
     */
    UInt_t getNHits() const {return static_cast<UInt_t>(getIntVal( kCellParameterIntHitCounter));};

    /** Increment the hit counter.
     * The hit counter is increased when ever the ADC value is outside of a certain range
     */
    void incrementNHits() {setNHits(getNHits()+1);};

    /** Function which returns the number of hits converted into a float.
     * This function can be used together with getNoise and getPedestal in a generic way
     * (e.g. typedef Float_t CellParameter::*GetFloatFunc_t();)
     */
    Float_t getNHitsFloat() const {return (Float_t) getNHits();};


    /** Initialse the array which memorises the last pedestal subtracted values.
     * @todo initialsie with pedestal or zero?
     */
    void initADCMemory() {
      obj()->setIntVal( kCellParameterIntADCMemoryWritePos, kCellParameterFloatADCMemoryLast-kCellParameterFloatADCMemoryFirst);
      Float_t init_value=getPedestal();
      for (UInt_t memory_cell_i=0; memory_cell_i<=kCellParameterFloatADCMemoryLast-kCellParameterFloatADCMemoryFirst; memory_cell_i++) {
        setADCMemoryValue(memory_cell_i,init_value);
      }
    };

    /** Memorise an ADC value in the given cell.
     */
    void setADCMemoryValue(UInt_t memory_cell_i, Float_t an_adc_value)
#ifdef BOUNDARY_CHECK
      throw(std::range_error)
#endif
    {
#ifdef BOUNDARY_CHECK
      if (kCellParameterFloatADCMemoryFirst+memory_cell_i>kCellParameterFloatADCMemoryLast) {
        throw std::range_error("CellParameter::setGroupValue> group index larger than numeber of cells.");
      }
#endif
      obj()->setFloatVal( kCellParameterFloatADCMemoryFirst+memory_cell_i,an_adc_value);
    };

    /** Get a memorised ADC value from the given cell.
     */
    Float_t getADCMemoryValue(UInt_t memory_cell_i) const
#ifdef BOUNDARY_CHECK
      throw(std::range_error)
#endif
    {
#ifdef BOUNDARY_CHECK
      if (kCellParameterFloatADCMemoryFirst+memory_cell_i>kCellParameterFloatADCMemoryLast) {
        throw std::range_error("CellParameter::setGroupValue> group index larger than numeber of cells.");
      }
#endif
      return getFloatVal( kCellParameterFloatADCMemoryFirst+memory_cell_i);
    };


    /** Set the current ADC memory cell.
     */
    void setADCMemoryWritePos(UInt_t memory_cell_i)
#ifdef BOUNDARY_CHECK
      throw(std::range_error)
#endif
    {
#ifdef BOUNDARY_CHECK
      if (kCellParameterFloatADCMemoryFirst+memory_cell_i>kCellParameterFloatADCMemoryLast) {
        throw std::range_error("CellParameter::setGroupValue> group index larger than numeber of cells.");
      }
#endif
      obj()->setIntVal( kCellParameterIntADCMemoryWritePos,memory_cell_i);
    };

    void setNextADCMemory() {
      UInt_t index=getIntVal(kCellParameterIntADCMemoryWritePos);
      index++;
      if (index>kCellParameterFloatADCMemoryLast-kCellParameterFloatADCMemoryFirst) {
        index=0;
      }
      obj()->setIntVal( kCellParameterIntADCMemoryWritePos, index);
    };

    /** Get a memorised ADC value.
     */
    Float_t getADCMemoryValue() const {return getFloatVal( kCellParameterFloatADCMemoryFirst+getIntVal(kCellParameterIntADCMemoryWritePos));};

    /** Memorise an ADC value.
     * The given pedestal subtracted ADC value is memorised at the current write postion
     * and the position counter is advanced.
     */
    void setADCMemoryValue(Float_t pedestal_subtracted_value) {
      obj()->setFloatVal( kCellParameterFloatADCMemoryFirst+getIntVal(kCellParameterIntADCMemoryWritePos),pedestal_subtracted_value);
    };

    /** Get the mean value of a group of pedestal subtracted ADC values (the maximum is removed).
     * The mean value is used to track jumps in the pedestal.
     * If the jump is larger than a certain threshold the pedestal is considered to have changed.
     */
    Float_t getMeanADCValue(Float_t pedestal_subtracted_value) {
      // FIXME: why only write values which are different?
      //    if (getADCMemoryValue()!=pedestal_subtracted_value) {
      setNextADCMemory();
      setADCMemoryValue(pedestal_subtracted_value);

      UInt_t memory_cell_i=kCellParameterFloatADCMemoryFirst;
      Float_t max_val=getADCMemoryValue(memory_cell_i);
      Float_t sum=max_val;
      for (; ++memory_cell_i <= kCellParameterFloatADCMemoryLast; ) {
        Float_t a_value=getADCMemoryValue(memory_cell_i);
        sum+=a_value;
        if (a_value>max_val) max_val=a_value;
      }
      sum-=max_val;
      sum/=--memory_cell_i;
      return sum;
    };

    /** Shift the current and the old pedestal by this value.
     * The pedestal is shifted when a pedestal jump is detected.
     */
    void shiftPedestal(Float_t shift) {
      setPedestal(shift+getPedestal());
      setOldPedestal(shift+getOldPedestal());
      incrementNPedestalChanges();
    };

  protected:
    /** Set the value of the pedestal change counter.
     * The PedestalChange counter is increased when ever the ADC value is outside of a certain range
     */
    void setNPedestalChanges(UInt_t n_pedestal_changes) {obj()->setIntVal( kCellParameterIntNPedestalChanges, static_cast<Int_t>(n_pedestal_changes));};

  public:
    /** Get the value of the pedestal change counter.
     * The PedestalChange counter is increased when ever the ADC value is outside of a certain range
     */
    UInt_t getNPedestalChanges() const {return static_cast<UInt_t>(getIntVal( kCellParameterIntNPedestalChanges));};

  protected:
    /** Increment the pedestal change counter.
     * The PedestalChange counter is increased when ever the ADC value is outside of a certain range
     */
    void incrementNPedestalChanges() {setNPedestalChanges(getNPedestalChanges()+1);};

  public:

#ifdef WITH_CELL_PAR_INDEX
    /** Set the ModuleIndex.
     */
    void setModuleIndex(UInt_t a_module_index) {obj()->setIntVal( kCellParameterIntModuleIndex,(Int_t) a_module_index);};

    /** Get the ModuleIndex.
     * The result is undefined before the ModuleIndexs and the noise are calculated.
     */
    UInt_t getModuleIndex() const {return (UInt_t) getIntVal( kCellParameterIntModuleIndex);};


    /** Set the CellIndex.
     */
    void setCellIndex(UInt_t a_module_index) {obj()->setIntVal( kCellParameterIntCellIndex,(Int_t) a_module_index);};

    /** Get the CellIndex.
     * The result is undefined before the CellIndexs and the noise are calculated.
     */
    UInt_t getCellIndex() const {return (UInt_t) getIntVal( kCellParameterIntCellIndex);};
#endif

    // -------- need to implement abstract methods from LCGenericObject

    /** Return the type of the class
     */
    const std::string getTypeName() const {
      return std::string("CellParameter");
    };

    /** Return a brief description of the data members
     */
    const std::string getDataDescription() const {
      std::stringstream message;
      message << "i[" << kNCellParameterInts << "]: n,sat.,hits,ped. ch.,mem idx.; "
              << "f[" << kNCellParameterFloats << "]: pedestal,noise,old ped., old noise, mem[5]; ";
      return message.str();
    };

  };
}
#endif
