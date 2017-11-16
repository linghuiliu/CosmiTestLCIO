#include "AhcSlowReadoutModMapper.hh"

#include "IMPL/LCCollectionVec.h"
#include "EVENT/LCGenericObject.h"

#include "AhcSlowReadoutModMapping.hh"
#include <sys/time.h>

#include "CaliceException.hh"

namespace CALICE {

  AhcSlowReadoutModMapper::AhcSlowReadoutModMapper(){
    init();
  }

  void AhcSlowReadoutModMapper::init() {
    _isAvailable.clear();
    _CMBlabel.clear();
    _HVlabel.clear();
    _LVlabel.clear();
  }

  void AhcSlowReadoutModMapper::ensureSize(unsigned int i) {
    if (i <_isAvailable.size()) return;
    else {
      _isAvailable.resize(i+1,false);
      _CMBlabel.resize(i+1,-2);
      _HVlabel.resize(i+1,-2);
      _LVlabel.resize(i+1,-2);
    }
  }

  bool AhcSlowReadoutModMapper::isAvailable(int module) {
    if (module >=0 && (unsigned int)module <  _isAvailable.size()) return _isAvailable[module];
    else return false;
  }

  void AhcSlowReadoutModMapper::setValues(AhcSlowReadoutModBlock* output, const std::vector<AhcSlowReadoutModBlock*>& input, unsigned int module) const {

    AhcSlowMeasuresDblMap_t dblValues;

    // avoid the meaningless warning in AhcSlowReadoutModBlock while filling
    dblValues["CMBTEMPERATURES"];
    dblValues["CMBVOLTAGES"];
    dblValues["CMBVALUES"];
    dblValues["HBABVOLTAGES"];
    dblValues["HBABTEMPERATURES"];
    //

    if (_CMBlabel[module] >= 0 && _CMBlabel[module] < (int) (input.size()) && input[_CMBlabel[module]]) {
      unsigned int i = _CMBlabel[module];
      time_t oldTime = input[i]->getTimeStamp().unixTime();
      output->setTimeStamp(gmtime(&oldTime));
      output->setLedSetting(input[i]->getLedSetting());
      output->setCmbWidth(input[i]->getCmbWidth());
      output->setCmbHeight(input[i]->getCmbHeight());
      dblValues["CMBTEMPERATURES"] = input[i]->getCmbTemperatures();
      dblValues["CMBVOLTAGES"] = input[i]->getCmbVoltages();
      dblValues["CMBVALUES"] = input[i]->getCmbValues();
    }

    if (_HVlabel[module] >= 0 && _HVlabel[module] < (int)(input.size()) && input[_HVlabel[module]]) {
      unsigned int i = _HVlabel[module];
      time_t oldTime = input[i]->getTimeStamp().unixTime();
      output->setTimeStamp(gmtime(&oldTime));

      for (unsigned int iV=0; iV < input[i]->getHbabVoltages().size() && iV < 4;++iV ) dblValues["HBABVOLTAGES"].push_back(input[i]->getHbabVoltages()[iV]); // fill up to first 4 values
    }

    if (_LVlabel[module] >= 0 && _LVlabel[module] < (int)(input.size()) && input[_LVlabel[module]]) {
      if (dblValues["HBABVOLTAGES"].size() < 4)
        for (unsigned int iV=dblValues["HBABVOLTAGES"].size(); iV < 4;++iV ) dblValues["HBABVOLTAGES"].push_back(-1); // if there have been not enough HV voltages, fill up to 4
      unsigned int i = _LVlabel[module];
      time_t oldTime = input[i]->getTimeStamp().unixTime();
      output->setTimeStamp(gmtime(&oldTime));
      if (input[i]->getHbabVoltages().size()>4)
        for (unsigned int iV=4;iV<input[i]->getHbabVoltages().size();++iV) dblValues["HBABVOLTAGES"].push_back(input[i]->getHbabVoltages()[iV]);
      dblValues["HBABTEMPERATURES"] = input[i]->getHbabTemperatures();
    }


    if (dblValues.size() > 0) output->setDblArrays(dblValues);

  }


  lcio::LCCollection* AhcSlowReadoutModMapper::mapCollection(const lcio::LCCollection* const col ) const{

    if (col->getParameters().getStringVal("TypeName") != "AhcSlowReadoutModBlock") throw WrongDataFormatException("AhcSlowReadoutModMapper::mapCollection expects collection of AhcSlowReadoutModBlock");

    std::vector<AhcSlowReadoutModBlock*> sortedBlocks;
    sortedBlocks.clear();
    sortedBlocks.resize(_isAvailable.size(),0);

    for (int i=0;i<col->getNumberOfElements();++i) {
      lcio::LCGenericObject* obj = dynamic_cast<lcio::LCGenericObject*> (col->getElementAt(i));
      if (obj) {

        AhcSlowReadoutModBlock* element = new AhcSlowReadoutModBlock( obj);

        unsigned int moduleNo = element->getModuleNumber();

        if (! (moduleNo < sortedBlocks.size()) ) sortedBlocks.resize(moduleNo+1,0);

        sortedBlocks[moduleNo] = element;

      }
      else throw WrongDataFormatException("AhcSlowReadoutModMapper::mapCollection expects collection of AhcSlowReadoutModBlock resp. LCGenericObject");
    }

    lcio::LCCollectionVec *output = new LCCollectionVec(lcio::LCIO::LCGENERICOBJECT);

    for (unsigned int i=0; i<_isAvailable.size();++i)
      if (_isAvailable[i] && ( sortedBlocks[_CMBlabel[i]] || sortedBlocks[_HVlabel[i]] || sortedBlocks[_LVlabel[i]])) { //mapping has to exist for the module and at least one of the records where data of it ended up
        AhcSlowReadoutModBlock *correctedElement = new AhcSlowReadoutModBlock();

        correctedElement->setModuleNumber(i);

        setValues(correctedElement,sortedBlocks,i);

        output->addElement(correctedElement);
        //      correctedElement->print(std::cout);
      }

    for (unsigned int i=0; i < sortedBlocks.size(); ++i)
      if (sortedBlocks[i]) delete sortedBlocks[i];


    return output;
  }

  void AhcSlowReadoutModMapper::updateMapping(const lcio::LCCollection* const col){
    init();
    if (col->getParameters().getStringVal("TypeName") != "AhcSlowReadoutModMapping") throw WrongDataFormatException("AhcSlowReadoutModMapper::updateMapping expects collection of AhcSlowReadoutModMapping");

    for (int i=0;i<col->getNumberOfElements();++i) {

      lcio::LCGenericObject* obj = dynamic_cast<lcio::LCGenericObject*> (col->getElementAt(i));
      if (obj) {

        AhcSlowReadoutModMapping* mappingElement = new AhcSlowReadoutModMapping( obj);

        unsigned int moduleNo = mappingElement->getModule();
        ensureSize(moduleNo);
        _isAvailable[moduleNo] = true;
        _CMBlabel[moduleNo]    = mappingElement->getCMBlabel();
        _HVlabel[moduleNo]     = mappingElement->getHVlabel();
        _LVlabel[moduleNo]     = mappingElement->getLVlabel();
      }
      else throw WrongDataFormatException("AhcSlowReadoutModMapper::updateMapping expects collection of AhcSlowReadoutModMapping resp. LCGenericObject");
    }
  }


} //namespace CALICE
