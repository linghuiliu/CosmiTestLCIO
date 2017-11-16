//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// File: CalHitModifier.hpp
// Module: CalDigi
//
// Purpose: Abstract class, defines the interface for the modifiers.
//          Each modifier will transform ideal hits into real hits,
//          according to some parametrization.  Each particular modifier
//          will define the proper parametrization.
//
// 20041112 - Guilherme Lima - Created
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
#ifndef _CALHITMODIFIER_HPP_
#define _CALHITMODIFIER_HPP_

#include "TempCalHit.hpp"
#include <vector>
#include <map>
#include <string>
#include <iostream>

namespace digisim {

class CalHitModifier {

public:
  virtual CalHitModifier* newInstance(const std::string& modName) = 0;

  //virtual CalHitModifier* newInstance(const Digitizer digitizer, ModifierParameters modpars) = 0;

  // destructor
  virtual ~CalHitModifier() {};

  // set debug mode
  virtual void setDebug(int deb){
    _debug = deb;
  }

  //*** Interface to be defined in modifiers ***

  // initialize parameters
  virtual void init( std::vector<float>& floats ) = 0;

  // this is where real work is performed
  // hitmap is both input and output
  virtual void processHits(TempCalHitMap& hitmap) = 0;

  // default printout
  virtual void print() const = 0;

protected: // methods

  // constructor
  CalHitModifier(const std::string& modName) : _debug(0), _name(modName) { };

  // default constructor is called by default constructor of derived modifiers,
  // which in turn only gets called when the global modifiers are instantiated
  CalHitModifier() : _debug(0), _name("") { }

  //store pointer to class into map of available modifiers
  static void registerModifier(const std::string& name,
			       CalHitModifier* pMod)
  {
    _modifiersAvailable[ name ] = pMod;
  }



protected: // data
  int _debug;
  const std::string _name;
  //Modifier Parameters associated to this modifier
  //ModifierParameters _modpars;

public: // this is made public, so that it can be instantiated from Globals.cpp
  static std::map<std::string,CalHitModifier*> _modifiersAvailable;
};

}
#endif  //  _CALHITMODIFIER_HPP_
