#ifndef AhcAmplitude_h
#define AhcAmplitude_h 1

#include "lcio.h"
#include "IMPL/LCGenericObjectImpl.h"

namespace CALICE
{

  class AhcAmplitude : public IMPL::LCGenericObjectImpl
  {
  public: 
    
    /**Constructor from LCGenericObject
     */
    AhcAmplitude(EVENT::LCGenericObject *genericObject);

    /**Empty constructor
     */
    AhcAmplitude();
    
    /**Constructor from elements
       @param cellID cellID0
       @param amplRawADC raw enery, in ADC counts
       @param amplRawMinusPedestalADC pedestal subtracted raw energy, in ADC counts
       @param amplTemperatureCorrMIP amplitude in MIPs, based on temperature corrected MIP
       @param amplNOTTemperatureCorrMIP amplitude in MIPs, with MIP NOT temperature corrected
       @param amplGeV amplitude in GeV
       @param temperature
     */
    AhcAmplitude(int cellID, 
		 float amplRawADC, 
		 float amplRawMinusPedestalADC,
		 float amplTemperatureCorrMIP, 
		 float amplNOTTemperatureCorrMIP, 
		 float amplGeV,
		 float temperature=0);

    /** Important for memory handling*/
    virtual ~AhcAmplitude() { /* no op*/  }

    void setCellID(const int cellID);
    const int getCellID();

    void setAmplRawADC(const float amplRawADC);
    const float getAmplRawADC();

    void setAmplRawMinusPedestalADC(const float amplRawMinusPedestalADC);
    const float getAmplRawMinusPedestalADC();

    void setAmplTemperatureCorrMIP(const float amplTemperatureCorrMIP);
    const float getAmplTemperatureCorrMIP();

    void setAmplNOTTemperatureCorrMIP(const float amplNOTTemperatureCorrMIP);
    const float getAmplNOTTemperatureCorrMIP();

    void setAmplGeV(const float amplGeV);
    const float getAmplGeV();

    void setTemperature(const float temperature);
    const float getTemperature();

   /**Implementation of the virtual method from LCGenericObject
       @return type name, i.e. name of the class
     */
    const std::string getTypeName() const;

    /**Implementation of the virtual method from LCGenericObject
       @return data description, i.e description of the LCObject elements:
       "i:cellID,f:amplRawADC,f:amplRawMinusPedestalADC,f:amplTemperatureCorrMIP,f:amplNOTTemperatureCorrMIP,f:amplGeV,f:temperature";
     */
    const std::string getDataDescription() const;


    
  private:
    enum Ints {kCellID, kNInts};
    enum Float {kAmplRawADC, kAmplRawMinusPedestalADC, kAmplTemperatureCorrMIP, 
		kAmplNOTTemperatureCorrMIP, kAmplGeV, kTemperature,kNFloats};
    enum Doubless { kNDoubles };


  }; /*end of class*/
  
}/*end of namespace CALICE*/
#endif 

