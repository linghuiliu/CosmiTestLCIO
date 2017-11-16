#include "ScintCaloSlowReadoutModBlock.hh"
#include "to_binary_bitops.hh"

namespace CALICE{
   

 

  void ScintCaloSlowReadoutModBlock::setDblArrays(const ScintCaloSlowMeasuresDblMap_t& scintCaloSlowMeasuresDblMap) 
  {


    //the position variable
    int ipos(_startOfDoubles);
    //Loop over the vector with the defined types
    for ( ScintCaloSroTypesDblMap_t::iterator types_iter = _theDblTypes.begin(); types_iter != _theDblTypes.end(); types_iter++) {
      //std::cout << "Type: " << (*types_iter).first << std::endl;
      //Extract this type from the map (if possible)
      ScintCaloSlowMeasuresDblMap_t::const_iterator slowmap_iter = scintCaloSlowMeasuresDblMap.find((*types_iter).first); 
      if( slowmap_iter != scintCaloSlowMeasuresDblMap.end() )
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
    //set the start of the double values to the current position of the position index to allow for multiple calls of this method     
    _startOfDoubles=ipos;
  }


  void ScintCaloSlowReadoutModBlock::setIntArrays(const ScintCaloSlowMeasuresIntMap_t& scintCaloSlowMeasuresIntMap) 
  {


    //the position variable
    int ipos(_startOfInts);
    //int ipos(kIntSroModIntValues+1);
    //Loop over the vector with the defined types
    for ( ScintCaloSroTypesIntMap_t::iterator types_iter = _theIntTypes.begin(); types_iter != _theIntTypes.end(); types_iter++) {
      //std::cout << "Type: " << (*types_iter).first << std::endl;
      //Extract this type from the map (if possible)
      ScintCaloSlowMeasuresIntMap_t::const_iterator slowmap_iter = scintCaloSlowMeasuresIntMap.find((*types_iter).first); 
      if( slowmap_iter != scintCaloSlowMeasuresIntMap.end() )
	{
          //Set the position and the size of this datatype
	  setNumPos( (*types_iter).second.first, ipos , static_cast<int> ((*slowmap_iter).second.size())); 
	  for(std::vector<int>::const_iterator thevals_iter =
		(*slowmap_iter).second.begin(); thevals_iter != (*slowmap_iter).second.end(); thevals_iter++ ){
	    //std::cout << "Value: " << (*thevals_iter) << std::endl;
            obj()->setIntVal(ipos, (*thevals_iter));
            ipos++;
	  }  
	  
	  
        } else std::cout << "Type " << (*types_iter).first <<" not known" << std::endl;
    }
    //set the start of the int values to the current position of the position index to allow for multiple calls of this method     
    _startOfInts=ipos;
  }


  void ScintCaloSlowReadoutModBlock::setFloatArrays(const ScintCaloSlowMeasuresFloatMap_t& scintCaloSlowMeasuresFloatMap) 
  {
    //the position variable
    int ipos(_startOfFloats);
    //Loop over the vector with the defined types
    for ( ScintCaloSroTypesFloatMap_t::iterator types_iter = _theFloatTypes.begin(); types_iter != _theFloatTypes.end(); types_iter++) {
      //std::cout << "Type: " << (*types_iter).first << std::endl;
      //Extract this type from the map (if possible)
      ScintCaloSlowMeasuresFloatMap_t::const_iterator slowmap_iter = scintCaloSlowMeasuresFloatMap.find((*types_iter).first); 
      if( slowmap_iter != scintCaloSlowMeasuresFloatMap.end() )
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
   //set the start of the float values to the current position of the position index to allow for multiple calls of this method     
    _startOfFloats=ipos;
  }
  



  void ScintCaloSlowReadoutModBlock::prepareOutputVecs(){
   

    //Loop over the types vec for Doubles
    for (ScintCaloSroTypesDblMap_t::iterator types_iter = _theDblTypes.begin(); types_iter != _theDblTypes.end(); types_iter++) {
#ifdef CONV_DEBUG
      std::cout << "ScintCaloSlowReadoutModBlock::prepareOutputVecs Doubles: " << (*types_iter).first << std::endl;
      std::cout << "Position in map: " << types_iter->second.first << std::endl;
      std::cout << "Position in LCIO::Double Collec: " << getPos(types_iter->second.first) << std::endl;
      std::cout << "Number of values: " << getNum(types_iter->second.first) << std::endl;
#endif 
      //Retrieve position and number of values and store these in the
      //output vectors
      //Check first whether this datatype has been written at all
      int pos(getPos( types_iter->second.first));
      int numvals(getNum(types_iter->second.first));
      if(pos>-1) {
      for(int ivals = pos; ivals < pos+numvals;ivals++)
	types_iter->second.second->push_back(getDoubleVal(ivals));
      } else std::cout << "Nothing found for Type: " << (*types_iter).first<< std::endl;	
    }
    

    //Loop over the types vec Ints
    for (ScintCaloSroTypesIntMap_t::iterator types_iter = _theIntTypes.begin(); types_iter != _theIntTypes.end(); types_iter++) {
#ifdef CONV_DEBUG
      std::cout << "ScintCaloSlowReadoutModBlock::prepareOutputVecs Ints: " << (*types_iter).first << std::endl;
      std::cout << "Position in map: " << types_iter->second.first << std::endl;
      std::cout << "Position in LCIO::Double Collec: " << getPos(types_iter->second.first) << std::endl;
      std::cout << "Number of values: " << getNum(types_iter->second.first) << std::endl;
#endif 
      //Retrieve position and number of values and store these in the
      //output vectors
      //Check first whether this datatype has been written at all
      int pos(getPos( types_iter->second.first));
      int numvals(getNum(types_iter->second.first));
      if(pos>-1) {
	for(int ivals = pos; ivals < pos+numvals;ivals++)
	  types_iter->second.second->push_back(getIntVal(ivals));
      } std::cout << "Nothing found for Type: " << (*types_iter).first<< std::endl;
    }

    //Loop over the types vec Floats
    for (ScintCaloSroTypesFloatMap_t::iterator types_iter = _theFloatTypes.begin(); types_iter != _theFloatTypes.end(); types_iter++) {
#ifdef CONV_DEBUG
      std::cout << "ScintCaloSlowReadoutModBlock::prepareOutputVecs Floats: " << (*types_iter).first << std::endl;
      std::cout << "Position in map: " << types_iter->second.first << std::endl;
      std::cout << "Position in LCIO::Double Collec: " << getPos(types_iter->second.first) << std::endl;
      std::cout << "Number of values: " << getNum(types_iter->second.first) << std::endl;
#endif 
      //Retrieve position and number of values and store these in the
      //output vectors
      int pos(getPos( types_iter->second.first));
      int numvals(getNum(types_iter->second.first));
      if(pos > -1){
      for(int ivals = pos; ivals < pos+numvals;ivals++)
	types_iter->second.second->push_back(getFloatVal(ivals));
      }	 std::cout << "Nothing found for Type: " << (*types_iter).first<< std::endl;
    }


    
  }


 /** Convenient print method 
   */
  void ScintCaloSlowReadoutModBlock::print(std::ostream& os) {}
}
