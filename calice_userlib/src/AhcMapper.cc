#include "AhcMapper.hh"

#include "CaliceException.hh"
#include "ModuleConnection.hh"
#include "ModuleDescription.hh"
#include "HcalCellIndex.hh"

#include <iomanip>
#include <sstream>
#include <cmath>

namespace CALICE {


  void AhcMapper::fillModuleConnection(const lcio::LCCollection* col) {
    if (col->getParameters().getStringVal("TypeName") != "ModuleConnection") throw WrongDataFormatException("AhcMapper::fillModuleConnection: expects collection of ModuleConnection objects");

    mappingModified();

    std::vector<CALICE::ModuleConnection*> moduleConnections;

    int maxModules = 0;
    int maxFe      = 0;
    int maxK       = 0;

    clearCrate();
    clearSlot();

    for (int i =0; i < col->getNumberOfElements();++i) {
      LCGenericObject* obj = dynamic_cast<LCGenericObject*>(col->getElementAt(i));

      if (obj) {
        moduleConnections.push_back(new CALICE::ModuleConnection(obj));
        if (moduleConnections.back()->getModuleID() > maxModules) maxModules = moduleConnections.back()->getModuleID();
        if (moduleConnections.back()->getFrontEnd() > maxFe) maxFe = moduleConnections.back()->getFrontEnd();
        if ((int)CALICE::CellIndex(moduleConnections.back()->getIndexOfLowerLeftCell()).getLayerIndex() > maxK ) maxK = CALICE::CellIndex(moduleConnections.back()->getIndexOfLowerLeftCell()).getLayerIndex();
        registerCrate(moduleConnections.back()->getCrate());
        registerSlot(moduleConnections.back()->getSlot());
        //      registerModuleType(moduleConnections.back()->getModuleType());
      }
      else throw WrongDataFormatException("AhcMapper::fillModuleConnection: expects collection of LCGenericObject resp. ModuleConnection");
    }

    initFe(maxFe);
    initModule(maxModules);
    initK(maxK);

    for (unsigned int j = 0; j < moduleConnections.size(); ++j) {
      int module        = moduleConnections[j]->getModuleID();

      int crate         = moduleConnections[j]->getCrate();
      int slot          = moduleConnections[j]->getSlot();
      int fe            = moduleConnections[j]->getFrontEnd();

      //      int connectorType = moduleConnections[j]->getConnectorType();
      int indexLowLeft  = moduleConnections[j]->getIndexOfLowerLeftCell();
      int moduleType    = moduleConnections[j]->getModuleType();
      //      std::string connectorTypeName(moduleConnections[j]->getConnectorTypeName());

      /* There is the feature that the type index stored in ModuleConnection is not
       * equal to the type stored in the module description. Therefore, a hardcode +4
       * is needed.
       */
      setModuleType(module, getCombinedModuleType(moduleType + 4));

      setModuleCrateSlotFe(module,crate,slot,fe);
      setModuleK(module,CALICE::CellIndex(indexLowLeft).getLayerIndex());

      /*
        std::cout << "Module " << module << std::endl;
        std::cout << " type: " << moduleType << std::endl;
        std::cout << " DAQ connection (crate/slot/fe): " << crate <<"/" << slot << "/" << fe << std::endl;
        std::cout << " connector type: " << connectorType << " (" << connectorTypeName << ")" << std::endl;
        std::cout << " index lower left cell: " << indexLowLeft << " " << CALICE::HcalCellIndex(indexLowLeft) << std::endl;
        std::cout << std::endl;
      */
    }

  }


  /* There is design flaw in CALICE::ModuleDescription:
   * The translation between the internal cell index and electronic channel
   * is not contained in the class and has to be hardcoded.
   */
  void AhcMapper::getChipChannelForModuleDescriptionID(const unsigned int id, const unsigned int moduleType, int &chip, int &channel) const {
    chip = id / 18;
    if ((moduleType == 5) || (moduleType == 7)) chip += 6;

    channel = id % 18;
  }

  /* To get unique module offset identifier for the half-modules
   * some clever guy had to shift the cell index. Now, this gives
   * the problem, that the geometrical cellID in ModuleDescription
   * alone is wrong and needs an offset that is saved in ModuleLocation
   * or ModuleConnection.
   *
   * Unfortunately this cannot be transparently mapped to the data
   * structure in this class. This class assumes module types that
   * relate i,j to chip,chan. But the above mechanism allows to shift
   * i,j for each module indiviually.
   *
   * Nevertheless, up to now only module-type- and not module-dependant
   * shifts are used. Therefore, this gets corrected here.
   */
  int AhcMapper::correctCellIndex(const unsigned int moduleType, const int cellIndex) const {
    if (moduleType == 5) return cellIndex + 0x8000;
    if (moduleType == 6) return cellIndex + 0x40;
    if (moduleType == 7) return cellIndex + 0x8040;
    return cellIndex;
  }

  /* This Mapper expects one module to have only one type, therefore the
   * splitting into half-modules has to be reversed.
   */
  unsigned int AhcMapper::getCombinedModuleType(const unsigned int moduleType) const {
    return moduleType/2;
  }

  void AhcMapper::fillModuleDescription(const lcio::LCCollection* col) {

    if (col->getParameters().getStringVal("TypeName") != "ModuleDescription") throw WrongDataFormatException("AhcMapper::fillModuleDescription: expects collection of ModuleDescription objects");

    mappingModified();

    std::vector<CALICE::ModuleDescription*> moduleDescriptions;

    int maxChip             = 0;
    int maxChannel          = 0;
    unsigned int maxI       = 0;
    unsigned int maxJ       = 0;

    clearModuleType();

    for (int i =0; i < col->getNumberOfElements();++i) {
      LCGenericObject* obj = dynamic_cast<LCGenericObject*>(col->getElementAt(i));

      if (obj) {
        moduleDescriptions.push_back(new CALICE::ModuleDescription(obj));

        unsigned int moduleType = moduleDescriptions.back()->getModuleType();

        bool individualSize = moduleDescriptions.back()->hasCellDimensionsPerCell();

        //      std::cout << "moduleType: " << moduleType << " has " << moduleDescriptions.back()->getNCells() << " cells " << std::endl;

        unsigned int combinedModuleType = getCombinedModuleType(moduleType);

        registerModuleType(combinedModuleType);

        /* Run over all cells of module types. One design flaw in CALICE::ModuleDescriptions
         * is that the classes do not know if cells are valid or not and the
         * electronic channel id defines the index of the indivual cells.
         * Obviously, a geometrical cell index of 0 seems not reasonable. Therefore,
         * we work around by ignoring cells with this index.
         */
        for ( unsigned int iCell=0; iCell < moduleDescriptions.back()->getNCells(); ++iCell )
          if (moduleDescriptions.back()->getGeometricalCellIndex(iCell)) {

            int chip;
            int channel;

            getChipChannelForModuleDescriptionID(iCell, moduleType, chip, channel);

            if (chip    > maxChip)    maxChip    = chip;
            if (channel > maxChannel) maxChannel = channel;


            CALICE::CellIndex geoID(correctCellIndex(moduleType,moduleDescriptions.back()->getGeometricalCellIndex(iCell)));

            unsigned int I = geoID.getPadColumn();
            unsigned int J = geoID.getPadRow();

            unsigned int sizeI;
            unsigned int sizeJ;

            if (individualSize) {
              sizeI = (unsigned int) moduleDescriptions.back()->getIndividualCellWidth(iCell)/10;
              sizeJ = (unsigned int) moduleDescriptions.back()->getIndividualCellHeight(iCell)/10;
            }
            else {
              sizeI = (unsigned int) moduleDescriptions.back()->getCellWidth()/10;
              sizeJ = (unsigned int) moduleDescriptions.back()->getCellHeight()/10;
            }

            if (I+sizeI > maxI) maxI = I+sizeI;
            if (J+sizeJ > maxJ) maxJ = J+sizeJ;


            //    std::cout << "cell " << std::setw(3) << iCell << " is [chip/channel||I/J] " << std::setw(2) << chip << "/" << std::setw(2) << channel << "||" << std::setw(2) << I << "/" << std::setw(2) << J  << " geoIndex:" << moduleDescriptions.back()->getGeometricalCellIndex(iCell)  << std::endl;

          }

      }
      else throw WrongDataFormatException("AhcMapper::fillModuleDescription: expects collection of LCGenericObject resp. ModuleDescription");

    }

    initIJChipChannel(maxI,maxJ,maxChip,maxChannel);

    for (unsigned int j = 0; j < moduleDescriptions.size(); ++j) {

      unsigned int moduleType = moduleDescriptions[j]->getModuleType();

      bool individualSize = moduleDescriptions[j]->hasCellDimensionsPerCell();

      /* Run over all cells of module types. One design flaw in CALICE::ModuleDescriptions
       * is that the classes do not know if cells are valid or not and the
       * electronic channel id defines the index of the indivual cells.
       * Obviously, a geometrical cell index of 0 seems not reasonable. Therefore,
       * we work around by ignoring cells with this index.
       */
      for ( unsigned int iCell=0; iCell < moduleDescriptions[j]->getNCells(); ++iCell )
        if (moduleDescriptions[j]->getGeometricalCellIndex(iCell)) {

          int chip;
          int channel;
          getChipChannelForModuleDescriptionID(iCell, moduleType, chip, channel);

          CALICE::CellIndex geoID(correctCellIndex(moduleType,moduleDescriptions[j]->getGeometricalCellIndex(iCell)));
          unsigned int I = geoID.getPadColumn();
          unsigned int J = geoID.getPadRow();

          unsigned int sizeI;
          unsigned int sizeJ;

          if (individualSize) {
            sizeI = (unsigned int) moduleDescriptions[j]->getIndividualCellWidth(iCell)/10;
            sizeJ = (unsigned int) moduleDescriptions[j]->getIndividualCellHeight(iCell)/10;
          }
          else {
            sizeI = (unsigned int) moduleDescriptions[j]->getCellWidth()/10;
            sizeJ = (unsigned int) moduleDescriptions[j]->getCellHeight()/10;
          }

          setModuleTypeIJChipChannel(getCombinedModuleType(moduleType), I, J, sizeI, sizeJ, chip, channel);

        }
    }


  }


  unsigned int AhcMapper::countAvailable(const std::vector<bool> &vec) const {
    unsigned int counter = 0;
    for (unsigned int i = 0; i < vec.size(); ++i)
      if (vec[i]) counter++;
    return counter;

  }

  void AhcMapper::printStats(std::ostream &ostream) const {
    ostream << "max/used modules       = " << std::setw(5) << _nMod     -1  << " / " << std::setw(5) << countAvailable(_moduleAvailable)      << std::endl
            << "max/used module Types  = " << std::setw(5) << _nModType -1  << " / " << std::setw(5) << countAvailable(_moduleTypeAvailable)  << std::endl
            << "    used crates        = " << std::setw(5) << ""            << " / " << std::setw(5) << _nCrate                               << std::endl
            << "max/used slots         = " << std::setw(5) << _maxSlot      << " / " << std::setw(5) << _nSlot                                << std::endl
            << "max      FE            = " << std::setw(5) << _nFe        -1  << std::endl
            << "    used crate/slot/FE = " << std::setw(5) << ""            << " / " << std::setw(5) << countAvailable(_connectionAvailable) << std::endl
            << "max      chip          = " << std::setw(5) << _nChip    -1  << std::endl
            << "max      chan          = " << std::setw(5) << _nChan    -1  << std::endl
            << "max      I             = " << std::setw(5) << _nI         -1  << std::endl
            << "max      J             = " << std::setw(5) << _nJ         -1  << std::endl
            << "max/used K             = " << std::setw(5) << _nK       -1  << " / " << std::setw(5) << countAvailable(_kAvailable)           << std::endl
            << "******* values per module type ****" << std::endl;
    for (unsigned int moduleType=0; moduleType < _nModType;++moduleType)
      if (_moduleTypeAvailable[_moduleTypeName[moduleType]]) {
        ostream << "module type "  << _moduleTypeName[moduleType]                            << std::endl
                << "  used I/J         = " << countAvailable(_ijAvailable[moduleType])       << std::endl
                << "  used chip/chan   = " << countAvailable(_chipChanAvailable[moduleType]) << std::endl;
      }
    ostream << std::endl;

  }

  const std::string AhcMapper::printIfAvailable(bool available, unsigned int value) const {
    std::ostringstream stream;
    if (available) {
      stream << value;
      return stream.str();
    }
    else return std::string("not available");
  }

  void AhcMapper::print(std::ostream &ostream) const {

    for (unsigned int moduleType=0; moduleType < _nModType;++moduleType)
      if (_moduleTypeAvailable[_moduleTypeName[moduleType]]) {
        ostream << "module type "  << _moduleTypeName[moduleType] << std::endl
                << "   chip channel    I    J  sizeI sizeJ" << std::endl;
        for (unsigned int chip=0; chip < _nChip; ++chip)
          for (unsigned int channel=0; channel < _nChan; ++channel)
            if (_chipChanAvailable[moduleType].at(getIndexChan(chip,channel))) {
              ostream << std::setw(7) << chip << " " << std::setw(7) << channel
                      << " " << std::setw(4) << getIFromIJIndex(_ijVchan.at(moduleType).at(getIndexChan(chip,channel)))
                      << " " << std::setw(4) << getJFromIJIndex(_ijVchan.at(moduleType).at(getIndexChan(chip,channel)))
                      << " " << std::setw(4) << _sizeIVchan.at(moduleType).at(getIndexChan(chip,channel))
                      << " " << std::setw(4) << _sizeJVchan.at(moduleType).at(getIndexChan(chip,channel))
                      << std::endl;
            }
      }

    for ( unsigned int module = 0; module < _nMod; module++ )
      if (_moduleAvailable.at(module)){
        ostream << "Module  " << module << std::endl
                << "   type " << printIfAvailable(_moduleAvailable.at(module),_moduleTypeName[_typeVmodule.at(module)]) << std::endl
                << "  crate " << getCrate(module) << std::endl
                << "   slot " << getSlot(module) << std::endl
                << "     fe " << getFe(module) << std::endl
                << "      K " << getK(module) << std::endl
                << std::endl;
      }
  }

}
