#include "RunLocation.hh"


namespace CALICE {

  void RunLocation::print(  std::ostream& os )
  {
    os << "RunLocation information:" << std::endl
       << " Location: " << getLocation() << std::endl
       << " Generic RunType: " << getGenericRunType() << std::endl
       << " Year/Month: " << getMonthInfo() << std::endl
       << " Conditions folder: " << getConditionsFolder() << std::endl
       << " Converter folder: " << getConverterFolder( "<version>" ) << std::endl;
  }

}
