#include "BmlSlowRunDataBlock.hh"
#include "to_binary_bitops.hh"

namespace CALICE{
   

   void BmlSlowRunDataBlock::setTypes() {
    //define the doubles
    _theDblTypes.clear();
    _theDblTypes["SEXTAPOLECURRENTS"] = std::make_pair<int, std::vector<double>* > (kIntBmlSroRunPosSextCur, &_sextapoleCurrents);
    _theDblTypes["BENDCURRENTS"] = std::make_pair<int, std::vector<double>* > (kIntBmlSroRunPosBendCur, &_bendCurrents);
    _theDblTypes["COLLIMATORPOSITIONS"]  = std::make_pair<int, std::vector<double>* > (kIntBmlSroRunPosColPos, &_colPositions);
    _theDblTypes["QUADRUPOLECURRENTS"] = std::make_pair<int, std::vector<double>* > (kIntBmlSroRunPosQuadCur, &_quadrupoleCurrents);
    _theDblTypes["TRIMCURRENTS"] = std::make_pair<int, std::vector<double>* > (kIntBmlSroRunPosTrimCur, &_trimCurrents);

    //define the ints 
    _theIntTypes.clear();
    _theIntTypes["H6AEXPERIMENTCOUNT"] = std::make_pair<int, std::vector<int>* > (kIntBmlSroRunPosH6aExpCount, &_h6aExpCounts);
    _theIntTypes["H6BEXPERIMENTCOUNT"] = std::make_pair<int, std::vector<int>* > (kIntBmlSroRunPosH6bExpCount, &_h6bExpCounts);
    _theIntTypes["H6CEXPERIMENTCOUNT"]  = std::make_pair<int, std::vector<int>* > (kIntBmlSroRunPosH6cExpCount, &_h6cExpCounts);
    _theIntTypes["RPEXPERIMENTCOUNT"] = std::make_pair<int, std::vector<int>* > (kIntBmlSroRunPosRpExpCount, &_rpExpCounts);
    _theIntTypes["SCINTILLATORCOUNT"] = std::make_pair<int, std::vector<int>* > (kIntBmlSroRunPosScintCount, &_scintCounts);


    //define the floats
    _theFloatTypes.clear();

  }
 

  void BmlSlowRunDataBlock::setDblArrays(const DaqTypeDataDblMap_t& daqTypeDataDblMap) 
  {
        //the position variable
    int ipos(kIntBmlSroRunDblValues);
    //Loop over the vector with the defined types
    for ( BmlSroTypesDblMap_t::iterator types_iter = _theDblTypes.begin(); types_iter != _theDblTypes.end(); types_iter++) {
      //std::cout << "Type: " << (*types_iter).first << std::endl;
      //Extract this type from the map (if possible)
      DaqTypeDataDblMap_t::const_iterator slowmap_iter = daqTypeDataDblMap.find((*types_iter).first); 
      if( slowmap_iter != daqTypeDataDblMap.end() )
	{
          //Set the position and the size of this datatype
	  setNumPos( (*types_iter).second.first, ipos , static_cast<int> ((*slowmap_iter).second.size())); 
	  for(std::vector<double>::const_iterator thevals_iter =
		(*slowmap_iter).second.begin(); thevals_iter != (*slowmap_iter).second.end(); thevals_iter++ ){
	    //std::cout << "Value: " << (*thevals_iter) << std::endl;
            obj()->setDoubleVal(ipos, (*thevals_iter));
            ipos++;
	  }  
	  
	} else std::cout << "Type " << (*types_iter).first <<" not known" << std::endl;
    }
  }


  void BmlSlowRunDataBlock::setIntArrays(const DaqTypeDataUIntMap_t& daqTypeDataUIntMap) 
  {
    //the position variable
    int ipos(kIntBmlSroRunIntValues+1);
    //Loop over the vector with the defined types
    for ( BmlSroTypesIntMap_t::iterator types_iter = _theIntTypes.begin(); types_iter != _theIntTypes.end(); types_iter++) {
      //Extract this type from the map (if possible)
      //std::cout << "Type: " << (*types_iter).first << std::endl;
      DaqTypeDataUIntMap_t::const_iterator slowmap_iter = daqTypeDataUIntMap.find((*types_iter).first); 
      if( slowmap_iter != daqTypeDataUIntMap.end() )
	{
          //Set the position and the size of this datatype
	  setNumPos( (*types_iter).second.first, ipos , static_cast<int> ((*slowmap_iter).second.size())); 
	  for(std::vector<unsigned int>::const_iterator thevals_iter =
		(*slowmap_iter).second.begin(); thevals_iter != (*slowmap_iter).second.end(); thevals_iter++ ){
            obj()->setIntVal(ipos, static_cast<int>((*thevals_iter)));
            ipos++;
	  }  
	} else std::cout << "Type " << (*types_iter).first <<" not known" << std::endl;
      
    }
    
  }
  

  void BmlSlowRunDataBlock::setFloatArrays(const DaqTypeDataFloatMap_t& daqTypeDataFloatMap) 
  {
    //the position variable
    int ipos(kIntBmlSroRunFloatValues);
    //Loop over the vector with the defined types
    for ( BmlSroTypesFloatMap_t::iterator types_iter = _theFloatTypes.begin(); types_iter != _theFloatTypes.end(); types_iter++) {
      //std::cout << "Type: " << (*types_iter).first << std::endl;
      //Extract this type from the map (if possible)
      DaqTypeDataFloatMap_t::const_iterator slowmap_iter = daqTypeDataFloatMap.find((*types_iter).first); 
      if( slowmap_iter != daqTypeDataFloatMap.end() )
	{
          //Set the position and the size of this datatype
	  setNumPos( (*types_iter).second.first, ipos , static_cast<int> ((*slowmap_iter).second.size())); 
	  for(std::vector<float>::const_iterator thevals_iter =
		(*slowmap_iter).second.begin(); thevals_iter != (*slowmap_iter).second.end(); thevals_iter++ ){
	    //std::cout << "Value: " << (*thevals_iter) << std::endl;
            obj()->setFloatVal(ipos, (*thevals_iter));
            ipos++;
	  }  
	  
	  
        } else std::cout << "Type " << (*types_iter).first <<" not known" << std::endl;
    }
  }




  void BmlSlowRunDataBlock::prepareOutputVecs(){
   
    //Loop over the types vec for Doubles
    for (BmlSroTypesDblMap_t::iterator types_iter = _theDblTypes.begin(); types_iter != _theDblTypes.end(); types_iter++) {
#ifdef CONV_DEBUG
      std::cout << "BmlSlowRunDataBlock::prepareOutputVecs Doubles: " << (*types_iter).first << std::endl;
      std::cout << "Position in map: " << types_iter->second.first << std::endl;
      std::cout << "Position in LCIO::Double Collec: " << getPos(types_iter->second.first) << std::endl;
      std::cout << "Number of values: " << getNum(types_iter->second.first) << std::endl;
#endif 
      //Retrieve position and number of values and store these in the
      //output vectors
      //Check first whether this datatype has been written at all
      int pos(getPos( types_iter->second.first));
      int numvals(getNum(types_iter->second.first));
      if(pos > -1){
	for(int ivals = pos; ivals < pos+numvals;ivals++)
	  types_iter->second.second->push_back(getDoubleVal(ivals));
      } else std::cout << "Nothing found for Type: " << (*types_iter).first<< std::endl;
	
    }
    

    //Loop over the types vec Ints
    for (BmlSroTypesIntMap_t::iterator types_iter =
    _theIntTypes.begin(); types_iter != _theIntTypes.end(); types_iter++) {
#ifdef CONV_DEBUG
      std::cout << "BmlSlowRunDataBlock::prepareOutputVecs Ints: " << (*types_iter).first << std::endl;
      std::cout << "Position in map: " << types_iter->second.first << std::endl;
      std::cout << "Position in LCIO::Int Collec: " << getPos(types_iter->second.first) << std::endl;
      std::cout << "Number of values: " << getNum(types_iter->second.first) << std::endl;
#endif 
      //Retrieve position and number of values and store these in the
      //output vectors
      //Check first whether this datatype has been written at all
      int pos(getPos( types_iter->second.first));
      int numvals(getNum(types_iter->second.first));
      if(pos > -1){
	for(int ivals = pos; ivals < pos+numvals;ivals++) types_iter->second.second->push_back(getIntVal(ivals));
      }	else std::cout << "Nothing found for Type: " << (*types_iter).first<< std::endl;
    }

    //Loop over the types vec Floats
    for (BmlSroTypesFloatMap_t::iterator types_iter = _theFloatTypes.begin(); types_iter != _theFloatTypes.end(); types_iter++) {
#ifdef CONV_DEBUG
      std::cout << "BmlSlowRunDataBlock::prepareOutputVecs Floats: " << (*types_iter).first << std::endl;
      std::cout << "Position in map: " << types_iter->second.first << std::endl;
      std::cout << "Position in LCIO::Double Collec: " << getPos(types_iter->second.first) << std::endl;
      std::cout << "Number of values: " << getNum(types_iter->second.first) << std::endl;
#endif 
      //Retrieve position and number of values and store it in the
      //output vectors
      //Check first whether this datatype has been written at all
      int pos(getPos( types_iter->second.first));
      int numvals(getNum(types_iter->second.first));
      if(pos > -1){
	for(int ivals = pos; ivals < pos+numvals; ivals++)
	  types_iter->second.second->push_back(getFloatVal(ivals));
      }	else std::cout << "Nothing found for Type: " << (*types_iter).first<< std::endl;
    }


    
  }


 /** Convenient print method 
   */
  void BmlSlowRunDataBlock::print(std::ostream& os) {
    //prepare the results
    prepareOutputVecs();    
    os << "BmlSlow Run Data" << std::endl;
    os << "Time Stamp:       " << getTimeStamp().getDateString() <<" h"  << std::endl;
    os << "AbsorberPosition: " << getAbsorberPosition() << " mm" << std::endl;
    os << "T4Position:       " << getT4Position() << " mm" << std::endl;
    os << "TargetPosition:   " << getTargetPosition() << " mm" << std::endl;

    os << "*** Sextapole Magnets Current: ***" << std::endl; 
    unsigned int idev(0);
    std::vector<double> sextapoleCurrents(getSextapoleCurrents());
    for(unsigned int isextcurs = 0; isextcurs < sextapoleCurrents.size()-1; isextcurs+=2) {
      os << "Sextapole Magnets Current[" << idev+1 << "] - Measurement:  " << sextapoleCurrents[isextcurs]  << " A" << std::endl;
      os << "Sextapole Magnets Current[" << idev+1 << "] - Reference:    " << sextapoleCurrents[isextcurs+1]  << " A" << std::endl;
      idev++;
    }


    os << "*** Bending Magnets Current: ***" << std::endl; 
    idev=0;
    std::vector<double> bendCurrents(getBendCurrents());
    for(unsigned int ibendcurs = 0; ibendcurs < bendCurrents.size()-1; ibendcurs+=2) {
      os << "Bending Magnets Current["  << idev+1 << "] - Measurement:  " << bendCurrents[ibendcurs]  << " A" << std::endl;
      os << "Bending Magnets Current["  << idev+1 << "] - Reference:    " << bendCurrents[ibendcurs+1]  << " A" << std::endl;
      idev++;
    }

    os << "*** Quadupole Magnets Current: ***" << std::endl; 
    idev=0;
    std::vector<double> quadCurrents(getQuadrupoleCurrents());
    for(unsigned int iquadcurs = 0; iquadcurs < quadCurrents.size()-1; iquadcurs+=2) {
      os << "Quadrupole Magnets Current["  << idev+1 << "] - Measurement:  " << quadCurrents[iquadcurs]  << " A" << std::endl;
      os << "Quadrupole Magnets Current["  << idev+1 << "] - Reference:    " << quadCurrents[iquadcurs+1]  << " A" << std::endl;
      idev++;
    }

    os << "*** Trim Magnets Current: ***" << std::endl; 
    idev=0;
    std::vector<double> trimCurrents(getTrimCurrents());
    for(unsigned int itrimcurs = 0; itrimcurs < trimCurrents.size()-1; itrimcurs+=2) {
      os << "Trim Magnets Current["  << idev+1 << "] - Measurement:  " << trimCurrents[itrimcurs]  << " A" << std::endl;
      os << "Trim Magnets Current["  << idev+1 << "] - Reference:    " << trimCurrents[itrimcurs+1] << " A"  << std::endl;
      idev++;
    }

    os << "*** Collimator Settings: ***" << std::endl; 
    idev=0;
    std::vector<double> colPositions(getCollimatorPositions());
    for(unsigned int icolpos = 0; icolpos < colPositions.size()-3; icolpos+=4) {
      os << "Collimator Position x["  << idev+1 << "] - Measurement:  " << colPositions[icolpos]  << " mm" << std::endl;
      os << "Collimator Position x["  << idev+1 << "] - Reference:    " << colPositions[icolpos+1] << " mm"  << std::endl;
      os << "Collimator Position y["  << idev+1 << "] - Measurement:  " << colPositions[icolpos+2] << " mm" << std::endl;
      os << "Collimator Position y["  << idev+1 << "] - Reference:    " << colPositions[icolpos+3] << " mm"  << std::endl;
      idev++;
    }


    os << "*** H6A Experiment Count: ***" << std::endl; 
    //Print the h6a experiment count
    int ih6aexpcounts(0);
    std::vector<int> h6aExpCounts(getH6aExperimentCounts());
    for(std::vector<int>::iterator vec_iter = h6aExpCounts.begin(); vec_iter !=
          h6aExpCounts.end(); vec_iter++) {
      if(ih6aexpcounts < 4 ) os << "H6A Experiment Count [" << ih6aexpcounts+1 << "]: " << (*vec_iter) << " Counts" << std::endl;
      if(ih6aexpcounts > 3)  os << "H6A Experiment Count UNKNOWN TYPE [" << ih6aexpcounts+1 << "]: " << (*vec_iter) << " Counts" << std::endl;
      ih6aexpcounts++;
    }


    os << "*** H6B Experiment Counts: ***" << std::endl; 
    //Print the h6b experiment counts
    int ih6bexpcounts(0);
    std::vector<int> h6bExpCounts(getH6bExperimentCounts());
    for(std::vector<int>::iterator vec_iter = h6bExpCounts.begin(); vec_iter !=
          h6bExpCounts.end(); vec_iter++) {
      if(ih6bexpcounts < 4 ) os << "H6B Experiment Count [" << ih6bexpcounts+1 << "]: " << (*vec_iter) << " Counts" << std::endl;
      if(ih6bexpcounts > 3)  os << "H6B Experiment Count UNKNOWN TYPE [" << ih6bexpcounts+1 << "]: " << (*vec_iter) << " Counts" << std::endl;
      ih6bexpcounts++;
    }


    os << "*** H6C Experiment Counts: ***" << std::endl; 
    //Print the h6c experiment counts
    int ih6cexpcounts(0);
    std::vector<int> h6cExpCounts(getH6cExperimentCounts());
    for(std::vector<int>::iterator vec_iter = h6cExpCounts.begin(); vec_iter !=
          h6cExpCounts.end(); vec_iter++) {
      if(ih6cexpcounts < 4 ) os << "H6B Experiment Count [" << ih6cexpcounts+1 << "]: " << (*vec_iter) << " Counts" << std::endl;
      if(ih6cexpcounts > 3)  os << "H6B Experiment Count UNKNOWN TYPE [" << ih6cexpcounts+1 << "]: " << (*vec_iter) << " Counts" << std::endl;
      ih6cexpcounts++;
    }

    os << "*** RP Experiment Counts: ***" << std::endl; 
    //Print the rp experiment counts
    int irpexpcounts(0);
    std::vector<int> rpExpCounts(getRpExperimentCounts());
    for(std::vector<int>::iterator vec_iter = rpExpCounts.begin(); vec_iter !=
          rpExpCounts.end(); vec_iter++) {
      if(irpexpcounts < 8 ) os << "RP Experiment Count [" << irpexpcounts+1 << "]: " << (*vec_iter) << " Counts" << std::endl;
      if(irpexpcounts > 7)  os << "RP Experiment Count UNKNOWN TYPE [" << irpexpcounts+1 << "]: " << (*vec_iter) << " Counts" << std::endl;
      irpexpcounts++;
    }

    os << "*** Scintillator Counts: ***" << std::endl; 
    //Print the scintillator counts
    int iscintcounts(0);
    std::vector<int> scintCounts(getScintillatorCounts());
    for(std::vector<int>::iterator vec_iter = scintCounts.begin(); vec_iter !=
          scintCounts.end(); vec_iter++) {
      if(iscintcounts < 9 ) os << "Scintillator Counter [" << iscintcounts+1 << "]: " << (*vec_iter) << std::endl;
      if(iscintcounts > 8)  os << "Scintillator Counter UNKNOWN TYPE [" << iscintcounts+1 << "]: " << (*vec_iter) << std::endl;
      iscintcounts++;
    }




}


  
  
}
