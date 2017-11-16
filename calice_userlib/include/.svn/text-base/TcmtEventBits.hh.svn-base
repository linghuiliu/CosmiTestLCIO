#ifndef TcmtEventBits_hh
#define TcmtEventBits_hh 1

#include "BitSet.hh"

namespace CALICE {


  /**
   * Bit set to describe the type of event in the TCMT
   *
   * @author Benjamin.Lutz@desy.de
   * @date November 2009
   * @version 1.0
   */
  class TcmtEventBits : public BitSet {
  protected:
    enum ETcmtEventBitNo { kMinNumberMuonLikeTowers,  /*!< min number of muon like towers reached */
			   kMaxNumberMuonLikeTowers,  /*!< within max number of muon like towers  */
			   kMinNumberMuonLikeLayers,  /*!< min number of muon like layers reached */
			   kMaxNumberTotalHitsMuon,   /*!< within max number of total hits	  */
			   kMinNumberTotalHitsMuon,   /*!< min number of total hits reached	  */
			   kMaxSumEnergyMuon,         /*!< within max energy sum		  */
			   kMinSumEnergyMuon,         /*!< min energy sum reached                 */
			   kNumberOfBits}; // just to count the number of bits

  public:
    /**
     * standard constructor
     *
     * all bits are off
     */
    TcmtEventBits() : BitSet(kNumberOfBits) {}

    /**
     * constructor with initialisaton of the the bits
     *
     * @param[in] value bits to be set
     */
    TcmtEventBits(const int value) : BitSet(kNumberOfBits, value) {}

    /**
     * check if event has at least minimum number of muon like towers
     * @returns state of @ref kMinNumberMuonLikeTowers
     */
    bool hasMinNumberMuonLikeTowers() { return getBit(kMinNumberMuonLikeTowers); }
    /**
     * check if event has more than maximun number of muon like towers
     * @returns state of @ref kMaxNumberMuonLikeTowers
     */
    bool hasMaxNumberMuonLikeTowers() { return getBit(kMaxNumberMuonLikeTowers); }
    /**
     * check if event has at least minimum number of muon like layers
     * @returns state of @ref kMinNumberMuonLikeLayers
     */
    bool hasMinNumberMuonLikeLayers() { return getBit(kMinNumberMuonLikeLayers); }
    /**
     * check if event has at least minimum number hits required by a muon like signal
     * @returns state of @ref kMinNumberTotalHitsMuon
     */
    bool hasMinNumberHitsMuon()       { return getBit(kMinNumberTotalHitsMuon); }
    /**
     * check if event has not more than maximum number hits required by a muon like signal
     * @returns state of @ref kMaxNumberTotalHitsMuon
     */
    bool hasMaxNumberHitsMuon()       { return getBit(kMaxNumberTotalHitsMuon); }
    /**
     * check if event has at least minimum energy sum required by a muon like signal
     * @returns state of @ref kMinSumEnergyMuon
     */
    bool hasMinSumEnergyMuon()        { return getBit(kMinSumEnergyMuon); }
    /**
     * check if event has not more than maximum energy sum required by a muon like signal
     * @returns state of @ref kMaxSumEnergyMuon
     */
    bool hasMaxSumEnergyMuon()        { return getBit(kMaxSumEnergyMuon); }

    /**
     * @returns true if signal fullfills all muon tests
     */
    bool isMuon() {
      return ( getBit(kMinNumberMuonLikeTowers) &&
    	       getBit(kMaxNumberMuonLikeTowers) &&
    	       getBit(kMinNumberMuonLikeLayers) &&
    	       getBit(kMinNumberTotalHitsMuon) &&
    	       getBit(kMaxNumberTotalHitsMuon) &&
    	       getBit(kMinSumEnergyMuon) &&
    	       getBit(kMaxSumEnergyMuon) );
    }

     /**
      * @returns true if signal is lower in energy and hits than a muon
      */
     bool isPedestal() {
       return ( ( ! getBit(kMinNumberTotalHitsMuon) ) &&
     		 getBit(kMaxNumberTotalHitsMuon) &&
     	       ( ! getBit(kMinSumEnergyMuon) ) &&
     		 getBit(kMaxSumEnergyMuon) );
     }

     /**
      * @returns true if signal is higher than pedestal (muon AND other signals)
      */
     bool isLeakage() {
       return (    ( getBit(kMinNumberTotalHitsMuon) &&  getBit(kMinSumEnergyMuon) ) // not pedestal
     	       && (
		   ( ! getBit(kMinNumberMuonLikeLayers) && ( ! getBit(kMinNumberMuonLikeTowers) || getBit(kMaxNumberMuonLikeTowers) ) ) // not at all muon like shape
                   || ( ! ( getBit(kMaxNumberTotalHitsMuon) &&  getBit(kMaxSumEnergyMuon) ) ) // or too large signal for muon
     		  )
     	       );
     }

    void setMinNumberMuonLikeTowers(bool state) { return setBit(kMinNumberMuonLikeTowers, state); }
    void setMaxNumberMuonLikeTowers(bool state) { return setBit(kMaxNumberMuonLikeTowers, state); }
    void setMinNumberMuonLikeLayers(bool state) { return setBit(kMinNumberMuonLikeLayers, state); }
    void setMinNumberHitsMuon(bool state)       { return setBit(kMinNumberTotalHitsMuon, state); }
    void setMaxNumberHitsMuon(bool state)       { return setBit(kMaxNumberTotalHitsMuon, state); }
    void setMinSumEnergyMuon(bool state)        { return setBit(kMinSumEnergyMuon, state); }
    void setMaxSumEnergyMuon(bool state)        { return setBit(kMaxSumEnergyMuon, state); }

  };


}


#endif
