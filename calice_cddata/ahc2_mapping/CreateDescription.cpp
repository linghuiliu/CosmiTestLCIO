#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <TMath.h>
#include <math.h>
#include <string>
#include <cmath>
#include <iomanip>

using namespace std;

int FastDecoder(int CellID, string encoding, string variable);

void CreateDescription()
{
  ofstream outfile("ModuleDescription_TopReadout_Inversed.txt");
  ifstream infile("ModuleDescription_TopReadout.txt");
 
  outfile << "#\t Module \t Chip \t Chn \t CellID1 \t I \t J \t K \t CellID0" << endl;

  if(infile.is_open())
    {
      string line;
      while(getline(infile, line))
	{
	  if(line[0] == '#') continue;

	  istringstream iss(line);
	  int Module, Chip, Chn, CellID1;
	  int I, J, K, CellID0;

	  iss >> Module >> Chip >> Chn >> CellID1 >> I >> J >> K >> CellID0;

	  //cout << Chip << " " << Chn << " " << CellID1 << " " << I << " " << J << " " << K << " " << CellID0 << endl;

	  string encoding = "M:3,S-1:3,I:9,J:9,K-1:6";
	  string Module_encoding = "module:6,chip:4,chan:6,SiPM:16";
	  FastDecoder(CellID0, encoding, "I");
	  FastDecoder(CellID0, encoding, "J");
	  FastDecoder(CellID0, encoding, "K");

	  FastDecoder(CellID1, Module_encoding, "module");
	  FastDecoder(CellID1, Module_encoding, "chip");
	  FastDecoder(CellID1, Module_encoding, "chan");

	  int newI = 0;
	  int newJ = 0;
	  int newCellID0 = 0;

	  //Do Inversion (tiles in front of the beam)
	  //Module 1 == HBU
	  //Module 2 == 2*2 HBU
	  //Module 4 == EBU horizontal (36 I / 4 J)
	  //Module 3 & 5 == EBU vertical (4 I / 36 J)

	  if(Module == 1 || Module == 2)
	    {
	      newI = 25 - I;
	      newJ = J;
	      newCellID0 = ((newI & 0x1ff) << 6) + ((newJ & 0x1ff) << 15) + ((K-1 & 0x3f) << 24);
	    }
	  if(Module == 3 || Module == 5)
	    {
	      newI = 5 - I;
	      newJ = J;
	      newCellID0 = ((newI & 0x1ff) << 6) + ((newJ & 0x1ff) << 15) + ((K-1 & 0x3f) << 24);
	    }
	  if(Module == 4)
	    {
	      newI = 37 - I;
	      newJ = J;
	      newCellID0 = ((newI & 0x1ff) << 6) + ((newJ & 0x1ff) << 15) + ((K-1 & 0x3f) << 24);
	    }

	  outfile <<  Module << "\t" << Chip << "\t" << Chn << "\t" << CellID1 << "\t" << newI << "\t" << newJ << "\t" << K << "\t" << CellID0 << endl;
	}
    }

  infile.close();
  outfile.close();
}

int FastDecoder(int CellID, string encoding, string variable)
{

  int position = 0;
  int mask = 0;
  int offset = 0;

  int currentPosition = 0;

  stringstream stream;
  if(encoding == "") return -1;
  
  stream << encoding;
  string token = "";

  while(!(stream.fail() | stream.eof()))
    {

      getline(stream, token, ',');
      size_t firstSep = token.find(':');
      int size = 0;

      size_t secondSep = token.find(':', firstSep+1);
      if(secondSep == string::npos)
	{
	  stringstream converterSize;
	  converterSize << token.substr(firstSep+1);

	  if((converterSize >> size).fail())
	    {
	      cout << "cannot convert size " << converterSize.str() << " to int" << endl;
	      return -1;
	    }
	  if(size>0) currentPosition += size;
	  else currentPosition -= size;
	}
      else
	{
	  stringstream converterSize;
	  converterSize << token.substr(secondSep+1);

	  if((converterSize >> size).fail())
	    {
	      cout << "cannot convert size " << converterSize.str() << " to int" << endl;
	      return -1;
	    }

	  stringstream converterPosition;
	  converterPosition << token.substr(firstSep+1, secondSep-firstSep-1);
	  if((converterPosition >> currentPosition).fail())
	    {
	      cout << "Cannot convert position " << converterPosition.str() << " to int" << endl;
	      return -1;
	    }
	  currentPosition += size;
	}

      if(token.substr(0, firstSep).find(variable) == 0 && token.find_first_of("+-:") == variable.size())
	{

	  string identifier = token.substr(0, firstSep);
	  if(identifier.size() > variable.size())
	    {
	      stringstream converter;
	      converter << identifier.substr(variable.size());
	      if((converter >> offset).fail())
		{
		  cout << "Cannot convert offset " << converter.str() << " to int" << endl;
		  return -1;
		}
	    }
	  
	  if(size < 0)
	    {
	      mask = pow(2, -size)-1;
	    }
	  else
	    {
	      mask = pow(2, size)-1;
	    }

	  position = currentPosition-size;

	
	  cout << variable << " at " << position << " size " << size << " mask : " << std::hex << mask << " with offset " << std::dec << offset << endl;

	  return ((CellID >> position) & mask) - offset;
	}
    }
}
