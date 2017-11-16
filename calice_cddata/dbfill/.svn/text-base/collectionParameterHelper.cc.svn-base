#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <sstream>
#include "lcio.h"
#include "IMPL/LCCollectionVec.h"

#include "collectionParameterHelper.hh"

typedef std::vector<std::vector<std::string> > ParameterContainer;

void setParameter(lcio::LCCollectionVec* col, 
                  ParameterContainer params) {

  for(ParameterContainer::iterator it = params.begin();
      it != params.end();
      ++it) {

    std::string type = (*it).at(0);

    //    (*it).erase((*it).begin());

    //    while(line >> element)

    std::cout << "type: ";
    if(type == "int:") {
      std::cout << "int\n";

      std::string key = (*it).at(1);
      std::cout << key << '\n';
      
      if((*it).size() - 2 > 1 ) {

        std::cout << "too many ints\n";

      }

      int value = atoi((*it).at(2).c_str());

      col->parameters().setValue(key, value);

      //      std::cout << value << '\n';

    } else if (type == "float:") {
      std::cout << "float\n";

      std::string key = (*it).at(1);
      std::cout << key << '\n';
      
      if((*it).size() - 2 > 1 ) {

        std::cout << "too many floats\n";

      }

      float value = atof((*it).at(2).c_str());

      col->parameters().setValue(key, value);

      //      std::cout << value << '\n';

    } else if (type == "string:") {
      std::cout << "string\n";

      std::string key = (*it).at(1);
      std::cout << key << '\n';
      
      if((*it).size() - 2 > 1 ) {

        std::cout << "too many strings\n";

      }

      std::string value = (*it).at(2);

      col->parameters().setValue(key, value);

    } else if (type == "IntVec:") {
      std::cout << "IntVec\n";

      std::string key = (*it).at(1);
      std::cout << key << '\n';

      std::vector<int> intvec;

      for(unsigned int n=2; n != (*it).size(); ++n) {

        intvec.push_back(atoi((*it).at(n).c_str()));

      }

      col->parameters().setValues(key, intvec);
      
      //      std::copy(intvec.begin(),intvec.end(),std::ostream_iterator<int>(std::cout," "));
      //      std::cout << '\n';

    } else if (type == "FloatVec:") {
      std::cout << "FloatVec\n";

      std::string key = (*it).at(1);
      std::cout << key << '\n';

      std::vector<float> floatvec;

      for(unsigned int n=2; n != (*it).size(); ++n) {

        floatvec.push_back(atof((*it).at(n).c_str()));

      }

      col->parameters().setValues(key, floatvec);


    } else if (type == "StringVec:") {
      std::cout << "StringVec\n";

      std::string key = (*it).at(1);
      std::cout << key << '\n';

      std::vector<std::string> stringvec;

      for(unsigned int n=2; n != (*it).size(); ++n) {

        stringvec.push_back((*it).at(n));

      }

      col->parameters().setValues(key, stringvec);


    } else {
      std::cerr << "Unknown type!\n";
    }

  }

}



ParameterContainer parseParameterfile(std::ifstream& f) {

  ParameterContainer lines;

  {

    std::string line;

    while(getline(f,line)) {

      std::vector<std::string> words;

      std::stringstream ss(line);

      std::string word;

      while(ss >> word) {

        words.push_back(word);

      }

      lines.push_back(words);

    }

  }

  return lines;

}

void setCollectionParameters(lcio::LCCollectionVec* col, 
                             std::string parameterConfigFile) {

  std::ifstream f(parameterConfigFile.c_str());

  if(f.good() == true && col != 0) {

    ParameterContainer params(parseParameterfile(f));

    setParameter(col, params);

  }

}
