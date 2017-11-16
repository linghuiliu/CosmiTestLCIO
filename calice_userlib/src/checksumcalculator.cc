#include "checksumcalculator.hh"
#include <vector>

char* checksumcalculator(LCObject* obj){

  MD5 md5;

  EVENT::LCGenericObject *generic_object=dynamic_cast<LCGenericObject *>(obj);
  if (!generic_object) {
    throw std::runtime_error( "ERROR:Checksum can only be calculated for LCGenericObjects" );
  }



  //calculate checksum for integer values in this object
  if(generic_object->getNInt() > 0) {
    vector<int> intvals;
    intvals.resize(generic_object->getNInt());
    //cout << "Number of Integers in Generic Object: " << generic_object->getNInt() << endl;

    for (int iint = 0; iint < generic_object->getNInt(); iint++) {
      intvals[iint] = generic_object->getIntVal(iint);
    }
    md5.update(reinterpret_cast<unsigned char *>( &(intvals[0] ) ), sizeof(int)/sizeof(char)
		* generic_object->getNInt()); 

  }


  //calculate checksum for float values in this object
  if(generic_object->getNFloat() > 0) {
    vector<float> floatvals;
    floatvals.resize(generic_object->getNFloat());
    //cout << "Number of Floats in Generic Object: " << generic_object->getNFloat() << endl;

    for (int ifloat = 0; ifloat < generic_object->getNFloat(); ifloat++) {
      floatvals[ifloat] = generic_object->getFloatVal(ifloat);
    }
    md5.update(reinterpret_cast<unsigned char *>(&floatvals[0]), sizeof(float)/sizeof(char)
		* generic_object->getNFloat()); 

  }


  //calculate checksum for double values in this object
  if(generic_object->getNDouble() > 0) {
    vector<double> doublevals;
    doublevals.resize(generic_object->getNFloat());
    //cout << "Number of Doubles in Generic Object: " << generic_object->getNDouble() << endl;
    
    for (int idouble = 0; idouble < generic_object->getNDouble(); idouble++) {
      doublevals[idouble] = generic_object->getDoubleVal(idouble);
    }
    md5.update(reinterpret_cast<unsigned char *>( &doublevals[0] ), sizeof(double)/sizeof(char)
	       * generic_object->getNFloat()); 
    
  }


  md5.finalize();
  return md5.hex_digest();

}
