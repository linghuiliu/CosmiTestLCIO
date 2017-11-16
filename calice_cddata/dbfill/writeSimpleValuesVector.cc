#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <stdexcept>

#include "SimpleValueVector.hh"
#include "HcalTileIndex.hh"

#include "lcio.h"
#include "IMPL/LCCollectionVec.h"
#include "UTIL/LCTime.h"
#include "IO/LCWriter.h"
#include "IMPL/LCEventImpl.h"
#include "lccd.h"
#include "lccd/DBInterface.hh"

#include "collectionParameterHelper.hh"

#include "writeLCGenericObject.hh"

using std::cout;
using std::endl;

class writeSimpleValuesVector : public writeLCGenericObject 
{
  
public:
  
  /*******************************************************************************/
  /*                                                                             */
  /*                                                                             */
  /*                                                                             */
  /*******************************************************************************/
  writeSimpleValuesVector(std::string description) : writeLCGenericObject(description) {}


  /*******************************************************************************/
  /*                                                                             */
  /*                                                                             */
  /*                                                                             */
  /*******************************************************************************/
  lcio::LCCollectionVec* readFlatFile( const char* fname )
  {
    std::ifstream fin( fname );

    if (! fin.is_open() )
      {
	std::stringstream msg;
	msg << "Cannot open file " << fname;
	throw std::runtime_error( msg.str() );
      }
    
    IMPL::LCCollectionVec* col = new IMPL::LCCollectionVec( LCIO::LCGENERICOBJECT );

    std::string line;
    int module, chip, channel;
    std::vector<float> vec_buf;
    std::vector<float> value;
    std::vector<float> error;
    std::vector<int> status;

    cout<<"\n ReadFlatFile "<<fname<<endl;

    while ( fin.good() )
      {
	getline( fin, line );
	if ( line == "" || line.at(0) == '#' ) continue;

	std::stringstream ln( line );
	float tempbuf;
	
	while(ln >> tempbuf)
	  {
	    vec_buf.push_back(tempbuf);
	  }
	
	module = (int)vec_buf.at(0);
	chip = (int)vec_buf.at(1);
	channel = (int)vec_buf.at(2);

	unsigned int size = (vec_buf.size() - 2)/3;
	value.resize(size);
	error.resize(size);
	status.resize(size);

	for(int i = 0; i < size; i++)
	  {
	    value.at(i) = vec_buf.at(3*i+3);
	    error.at(i) = vec_buf.at(3*i+4);
	    status.at(i) = vec_buf.at(3*i+5);
	  }
	
	vec_buf.clear();

	CALICE::HcalTileIndex hti( module, chip, channel );
	CALICE::SimpleValueVector* sv = new CALICE::SimpleValueVector( hti.getIndex(), size, value, error, status );
	col->addElement( sv );

	cout<<" index: "<<hti.getIndex() <<" module: "<<module<<" chip: "<<chip<<" channel: "<<channel;
	for(int i = 0; i < size; i++)
	  {
	    cout << " value: "<<value.at(i)<<" error: "
		      <<error.at(i)<<" status: "<<status.at(i);
	  }
	cout << endl;
	

	value.clear();
	error.clear();
	status.clear();
      }
    
    return col;
  }

  /*******************************************************************************/
  /*                                                                             */
  /*                                                                             */
  /*                                                                             */
  /*******************************************************************************/
  int printHelp( int ret )
  {
    cout << "Program writeSimpleValuesVector" << endl
	 << "--inFile <file>        input flat file" << endl
	 << "--outFile <file>       output LCIO file" << endl
	 << "--colName <file>       name of LCCollection in file" << endl
	 << "--dbinit <string>      database init string" << endl
	 << "--folder <string>      database folder" << endl
	 << "--description <string> description stored in DB" << endl
	 << "--from <string>        start of validity time" << endl
	 << "--until <string>       end of validity time" << endl
	 << "--timeIsTimeStamp <string> true or false   " << endl
	 << "--write                dry test run otherwise" << endl
	 << "--parameterFile <file> file with collection parameters" << endl
	 << "--help                 print this text" << endl;

    cout<<"\n Example usage: \n"<<endl;
    cout<<" a) For writing a file: "<<endl;
    cout<<"    ./writeSimpleValuesVector --inFile <your input file> --outFile <some output file name>"
	<<" --colName <collection name>"
	<<" --description \"some description\" --from 2010-09-01-00-00-00-000000000 --until 2010-11-21-06-15-45-000000000 --timeIsTimeStamp false --write"
	<<"\n"<<endl;
    cout<<" b) For writing a data base folder, using time: "<<endl;
    cout<<"    ./writeSimpleValuesVector --inFile <your input file> --dbinit ${CALICE_DB_INIT} --folder \"/test_al/some/folder\""
	<<"--description \"some description\"  --from 2010-09-04-11-14-32.121853000  --until 2010-09-05-15-44-33.954407000 --timeIsTimeStamp false --write"
	<<"\n"<<endl;
    
    cout<<" c) For writing a data base folder, using RUNS to set the  validity time: "<<endl;
    cout<<"    ./writeSimpleValuesVector --inFile <your input file> --dbinit ${CALICE_DB_INIT} --folder \"/test_al/some/folder\""
	<<"--description \"some description\"  --from 360336  --until 360869 --timeIsTimeStamp false --useRunNumberAsTime --write"
	<<"\n"<<endl;
    cout<<"Note: In the last case, don't forget to use the '--useRunNumberAsTime'."<<"\n"<<endl;

    return ret;
  }


  /*******************************************************************************/
  /*                                                                             */
  /*                                                                             */
  /*                                                                             */
  /*******************************************************************************/
  void screenDump( lcio::LCCollection* col )
  {
    for ( int i = 0; i != col->getNumberOfElements(); ++i )
      {
	CALICE::SimpleValueVector* val = static_cast<CALICE::SimpleValueVector*>( col->getElementAt( i ) );
	CALICE::HcalTileIndex hti( val->getCellID() );
	cout << "module: " <<hti.getModule() << " "
	     << "chip: "   <<hti.getChip() << " "
	     << "channel: "<<hti.getChannel() << " " 
	     << "size "<< val->getSize() << endl;
	
	  for(int ii = 0; ii < val->getSize(); ii++)
	    {
	      cout << "value: "  << val->getValue(ii) << " "
	      << "error: "  <<val->getError(ii) << " "
	      << "status: " <<val->getStatus(ii) << endl;
	    }
      }
  }
  
  
};


/*******************************************************************************/
/*                                                                             */
/*                                                                             */
/*                                                                             */
/*******************************************************************************/
int main(int argc, char** argv) 
{
  writeSimpleValuesVector wsv("wsv");

  wsv.mymain(argc, argv);
  
}
