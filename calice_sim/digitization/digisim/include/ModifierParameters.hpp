#include <vector>
#include <string>
using std::string;

namespace digisim {

//typedef basic_string< char > string;
class ModifierParameters {
  //-- class methods
public:
  // Constructor
  ModifierParameters(string name, string type, std::vector<double>& pars);

  // Enumerates all possible modifier types
  //enum Type;

  // Returns the type of this modifier
  string getType();

  // Returns the name of this modifier
  string getName() const;

  // Returns the parameters of this modifier
  std::vector<double> getParams() const;

private:
  // default constructor
  ModifierParameters() { }

  //-- Data Members
private:

  //Modifier type
  string _type;

  //Modifier name
  string _name;

  //Modifier parameters
  std::vector<double> _pars;

}; // end class definition

} // end digisim namespace
