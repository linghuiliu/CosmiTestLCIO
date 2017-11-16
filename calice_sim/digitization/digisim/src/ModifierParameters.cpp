/**
 * Encapsulates the configuration parameters for any DigiSim modifier.
 *
 * @author Guilherme Lima, C.DeCaro
 * @version $Id: ModifierParameters.cpp,v 1.3 2008-02-01 16:11:29 lima Exp $
 */
#include "ModifierParameters.hpp"
#include <vector>
#include <string>
#include <cstdlib>

using std::vector;
using std::string;


namespace digisim {

  //Constructor
  ModifierParameters::ModifierParameters(string name, string type, std::vector<double>& pars) {
    _name = name;
    _type = type;
    _pars = pars;
  }

  //enum Type {
  //GainDiscrimination,
  //SmearedLinear,
  //RandomNoise,
  //HotCell,
  //DeadCell,
  //SiPMSaturation
  //};

  /*Returns Type of this modifier as a string
    Note: Modifier types are not enumerated, and are always of type char*/
  string ModifierParameters::getType() {
    return _type;
  }

  //Returns the name of this modifier
  string ModifierParameters::getName() const
  {
    return _name;
  }

  //Returns the parameters of this modifier
  std::vector<double> ModifierParameters::getParams() const
  {
    return _pars;
  }

  static int _random = rand();
}
