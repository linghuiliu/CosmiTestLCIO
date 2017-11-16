#ifndef __AhcSlowReadoutModMapper_hh__
#define __AhcSlowReadoutModMapper_hh__ 1

#include "lcio.h"
#include "EVENT/LCCollection.h"

#include <vector>

#include "AhcSlowReadoutModBlock.hh"

namespace CALICE {


  /**
   * @brief Class to correct the mapping of AhcSlowReadoutModBlock-classes
   *
   * The class AhcSlowReadoutModBlock containes the module number for which the records stands.
   * Due to problems in the slow control in the years 2006  and 2007 the values from the
   * different data sources have been written with wrong module number. Therefore, the
   * single AhcSlowReadoutModBlock contains a collection of values from different modules.
   * e.g. record with label "module = 17" contains CMB readings from module 23,
   * HV readings from module 19 and corrupted LV readings
   *
   * This class collects, as far as possible, the datas for the modules from the different records these
   * ended up. If the mapping for one part is missing the values for this source will not be filled.
   *
   * @see AhcSlowReadoutModBlock
   * @see AhcSlowReadoutModMapping
   *
   * @author Benjamin.Lutz@desy.de
   * @date Nov 2008
   * @version 1.0
   */
  class AhcSlowReadoutModMapper {
  public:
    AhcSlowReadoutModMapper();

    /**
     * @brief returns collection with corrected AhcSlowReadoutModBlock-elements for an unmapped
     *        collection of AhcSlowReadoutModBlock
     *
     * @throws WrongDataFormatException When the collection type is not LCGenericObjects or the collection parameter "TypeName" is not "AhcSlowReadoutModBlock"
     * @param[in] col collection of AhcSlowReadoutModBlock
     * @return corrected collection of AhcSlowReadoutModBlock
     * @see WrongDataFormatException
     * @see AhcSlowReadoutModBlock
     */
    lcio::LCCollection* mapCollection(const lcio::LCCollection* const col ) const;

    /**
     * @brief Function to update the mapping
     *
     * @throws CALICE::WrongDataFormatException When the collection type is not LCGenericObjects or the collection parameter "TypeName" is not "AhcSlowReadoutModMapping"
     * @param[in] col The collection of AhcSlowReadoutModMapping objects
     * @see WrongDataFormatException
     * @see AhcSlowReadoutModMapping
     */
    void updateMapping(const lcio::LCCollection* const col);

  private:
    void init();
    void ensureSize(unsigned int i);
    bool isAvailable(int module);

    void setValues(AhcSlowReadoutModBlock* output, const std::vector<AhcSlowReadoutModBlock*>& input, unsigned int module) const;

    std::vector<bool> _isAvailable;
    std::vector<int>  _CMBlabel;
    std::vector<int>  _HVlabel;
    std::vector<int>  _LVlabel;

  }; // class AhcSlowReadoutModMapper


} // namespace CALICE

#endif
