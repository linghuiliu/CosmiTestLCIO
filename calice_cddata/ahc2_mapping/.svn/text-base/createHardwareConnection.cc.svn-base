#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

//usage ./createHardwareConnection [outFile] [MinChip] [MaxChip]

int main(int argc, char **argv)
{

  if(argc < 4)
    {
      cout << "Provide output file name / MinChip / MaxChip !!!" << endl;
      return 0;
    }

  string filename = argv[1];
  int iChipMin = atoi(argv[2]);
  int iChipMax = atoi(argv[3]);

  ofstream fIn;
  fIn.open(filename.c_str());

  if(fIn.is_open())
    {

      fIn << "#ChipID \t ModuleNumber \t ChipNumber" << endl;

      for(int HBU_CHIPID = iChipMin; HBU_CHIPID <= iChipMax; HBU_CHIPID++)
	{
	  int Module = 0;
	  int ChipNumber = -1;

	  //Layer1
	  if( HBU_CHIPID >= 129 && HBU_CHIPID <= 132 ) { 
	    Module = 1;//For June2015
	    ChipNumber = (HBU_CHIPID - 129)%4;

	    fIn << HBU_CHIPID << "\t" << Module << "\t" << ChipNumber << endl;
	  } 
	  //Layer2
	  else if( HBU_CHIPID >= 137 && HBU_CHIPID <= 140 ) { 
	    Module = 2;//For June2015
	    ChipNumber = (HBU_CHIPID - 137)%4;

	    fIn << HBU_CHIPID << "\t" << Module << "\t" << ChipNumber << endl;
	  } 
	  //Layer3
	  else if( HBU_CHIPID >= 233 && HBU_CHIPID <= 236 ) { 
	    Module = 3;//For June2015
	    ChipNumber = (HBU_CHIPID - 233)%4;

	    fIn << HBU_CHIPID << "\t" << Module << "\t" << ChipNumber << endl;
	  } 
	  //Layer4
	  else if( HBU_CHIPID >= 165 && HBU_CHIPID <= 168 ) { 
	    Module = 4;//For June2015
	    ChipNumber = (HBU_CHIPID - 165)%4;

	    fIn << HBU_CHIPID << "\t" << Module << "\t" << ChipNumber << endl;
	  } 
	  //Layer5
	  else if( HBU_CHIPID >= 161 && HBU_CHIPID <= 164 ) { 
	    Module = 5;//For June2015
	    ChipNumber = (HBU_CHIPID - 161)%4;

	    fIn << HBU_CHIPID << "\t" << Module << "\t" << ChipNumber << endl;
	  } 
	  //Layer6
	  else if( HBU_CHIPID >= 157 && HBU_CHIPID <= 160 ) { 
	    Module = 6;//For June2015
	    ChipNumber = (HBU_CHIPID - 157)%4;

	    fIn << HBU_CHIPID << "\t" << Module << "\t" << ChipNumber << endl;
	  } 
	  //Layer7
	  else if( HBU_CHIPID >= 153 && HBU_CHIPID <= 156 ) { 
	    Module = 7;//For June2015
	    ChipNumber = (HBU_CHIPID - 153)%4;

	    fIn << HBU_CHIPID << "\t" << Module << "\t" << ChipNumber << endl;
	  } 
	  //Layer8
	  else if( HBU_CHIPID >= 149 && HBU_CHIPID <= 152 ) { 
	    Module = 8;//For June2015
	    ChipNumber = (HBU_CHIPID - 149)%4;

	    fIn << HBU_CHIPID << "\t" << Module << "\t" << ChipNumber << endl;
	  } 
	  //Layer9
	  else if( HBU_CHIPID >= 145 && HBU_CHIPID <= 148 ) { 
	    Module = 9;//For Apr2015
	    ChipNumber = (HBU_CHIPID - 145)%4;

	    fIn << HBU_CHIPID << "\t" << Module << "\t" << ChipNumber << endl;
	  } 
	  //Layer10
	  else if( HBU_CHIPID >= 141 && HBU_CHIPID <= 144 ) { 
	    Module = 10;//For Apr2015
	    ChipNumber = (HBU_CHIPID - 141)%4;

	    fIn << HBU_CHIPID << "\t" << Module << "\t" << ChipNumber << endl;
	  } 
	  //Layer11
	  else if( HBU_CHIPID >= 169 && HBU_CHIPID <= 184) { 
	    Module = 11;//For Apr2015
	    ChipNumber = (HBU_CHIPID - 169)%16;

	    fIn << HBU_CHIPID << "\t" << Module << "\t" << ChipNumber << endl;
	  } 
	  //Layer12
	  else if( HBU_CHIPID >= 185 && HBU_CHIPID <= 200 ) { 
	    Module = 12;//For Apr2015
	    ChipNumber = (HBU_CHIPID - 185)%16;

	    fIn << HBU_CHIPID << "\t" << Module << "\t" << ChipNumber << endl;
	  } 
	  //Layer13
	  else if( HBU_CHIPID >= 201 && HBU_CHIPID <= 216 ) { 
	    Module = 13;//For Apr2015
	    ChipNumber = (HBU_CHIPID - 201)%16;

	    fIn << HBU_CHIPID << "\t" << Module << "\t" << ChipNumber << endl;
	  } 
	  //Layer14
	  else if( HBU_CHIPID >= 217 && HBU_CHIPID <= 232 ) { 
	    Module = 14;//For June2015
	    ChipNumber = (HBU_CHIPID - 217)%16;

	    fIn << HBU_CHIPID << "\t" << Module << "\t" << ChipNumber << endl;
	  }
	} 
    }
  
  fIn.close();

}
