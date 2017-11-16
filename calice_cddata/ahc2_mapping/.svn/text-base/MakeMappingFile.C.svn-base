#include <fstream>
#include <iostream>

#include <map>

using namespace std;

map<int,int> m_chan2IJ;

//ifstream fIn("ModuleDescription_LeftReadout.txt");
//ifstream fIn("ModuleDescription_TopReadout.txt");
ifstream fIn("ModuleDescription_TopReadout_Inversed.txt");
//ifstream fIn("ModuleDescription_RightReadout.txt");

//ofstream fOut("MapChipChan2IJK_June20150605.txt");
//ofstream fOut("MapChipChan2IJK_July2015.txt");
//ofstream fOut("MapChipChan2IJK_August2015.txt");
//ofstream fOut("MapChipChan2IJK_May2016_2ndConfig.txt");
ofstream fOut("MapChipChan2IJK_July2016_Airstack.txt");

bool m_Debug = false;

void MakeMapChan2IJ(){

  if (fIn.is_open()){
    string line;
    while (!fIn.eof()){
      getline(fIn, line);
      if(line[0] == '#') continue;

      int type = -1, chip = -1, chan = -1, cellID1 = -1,
	I = -1, J = -1, K = -1, cellID0 = -1;
      
      fIn >> type >> chip >> chan >> cellID1 >> I >> J >> K >> cellID0;
      
      if (m_Debug)
	cout << type << " " << chip << " " << chan << " " << cellID1 
	     << " " << I << " " << J << " " << K << " " << cellID0 << endl;

      if (type==-1) continue;

      int indexChan = 100000*type + 100*chip + chan;
      int indexIJ   = 100*I + J;

      m_chan2IJ.insert(make_pair<int,int>(indexChan,indexIJ));
    }
  }
}

void PrintOneLayer(int LayerPos = 0, int type = 0, int nChip = 0, int iChipStart = 0, int RealChipIDStart = 0){

  //######### Type of the layer is to indicate whether 
  //######### it's a single HBU or 2x2 HBUs or EBUs with different mapping
  //Type 1: Single HBU layer
  //Type 2: 2x2 HBU layer
  //Type 3: EBU 0
  //Type 4: EBU 1
  //Type 5: EBU 2

  //######## The number of chips (nChip) corresponds to the actual number of chips we have in the layer: If the 2x2 HBU layer is not completely equiped for instance but only 3 HBUs, nChip should be 12. And depending on the position where you put these 3 HBUs you need to change the iChipStart. 

  //######## And of course the RealChipIDStart is mandatory to have a useable mapping

  for (int ichip = iChipStart; ichip < (iChipStart+nChip); ichip++){
    int RealChipID = RealChipIDStart + (ichip-iChipStart);
    
    for (int ichan = 0; ichan < 36; ichan++){
      int indexChan = 100000*type + 100*ichip + ichan;
      
      int indexIJ = (m_chan2IJ.find(indexChan))->second;
      
      int I = indexIJ/100;
      int J = indexIJ%100;
      
      cout << "RealChipID " << RealChipID << " ichip " << ichip
	   << " indexChan " << indexChan << " indexIJ " << indexIJ 
	   << " I " << I << " J " << J << endl;
      
      fOut << LayerPos << " " << RealChipID << " " << ichan << " " << I << " " << J << " " << LayerPos << endl;
    }

  }

}

void PrintFullConfiguration(){

  MakeMapChan2IJ();

  fOut << "#Layer Chip Chan I J K" << endl;
 
  //Config Apr 2015
  /*
    PrintOneLayer(1,2,16,0,169);//12
    PrintOneLayer(2,2,16,0,185);//13
    PrintOneLayer(3,2,16,0,201);//14
    PrintOneLayer(4,2,8,0,161);//ITEP
    PrintOneLayer(5,2,4,4,233);//Mainz
  */

  //Config June 2015 DESY
  //Config with MPPC
  /*
    PrintOneLayer(1,1,4,0,237);//MPPC
  
    PrintOneLayer(2,3,4,0,129);//EBU0
    PrintOneLayer(3,5,4,0,137);//EBU2

    PrintOneLayer(4,1,4,0,233);//Mainz
    PrintOneLayer(5,1,4,0,165);//ITEP 11
    PrintOneLayer(6,1,4,0,161);//ITEP 10
    PrintOneLayer(7,1,4,0,157);//9
    PrintOneLayer(8,1,4,0,153);//8
    PrintOneLayer(9,1,4,0,149);//7
    PrintOneLayer(10,1,4,0,145);//6
    PrintOneLayer(11,1,4,0,141);//5

    PrintOneLayer(12,2,16,0,169);//12
    PrintOneLayer(13,2,16,0,185);//13
    PrintOneLayer(14,2,16,0,201);//14
    PrintOneLayer(15,2,16,0,217);//15
  */

  //SPS July 2015
  /*
    PrintOneLayer(1,3,4,0,129);//EBU0
    PrintOneLayer(2,5,4,0,137);//EBU2

    PrintOneLayer(3,1,4,0,233);//Mainz
    PrintOneLayer(4,1,4,0,165);//ITEP 11
    PrintOneLayer(5,1,4,0,161);//ITEP 10
    PrintOneLayer(6,1,4,0,157);//9
    PrintOneLayer(7,1,4,0,153);//8
    PrintOneLayer(8,1,4,0,149);//7
    PrintOneLayer(9,1,4,0,145);//6
    PrintOneLayer(10,1,4,0,141);//5

    PrintOneLayer(11,2,16,0,169);//12
    PrintOneLayer(12,2,16,0,185);//13
    PrintOneLayer(13,2,16,0,201);//14
    PrintOneLayer(14,2,16,0,217);//15
  */

  //SPS August 2015
  /*
    PrintOneLayer(1,3,4,0,129);//EBU0
    PrintOneLayer(2,4,4,0,133);//EBU1
    PrintOneLayer(3,5,4,0,137);//EBU2

    PrintOneLayer(4,1,4,0,233);//Mainz
    PrintOneLayer(5,1,4,0,165);//ITEP 11
    PrintOneLayer(6,1,4,0,161);//ITEP 10
    PrintOneLayer(7,1,4,0,157);//9
    PrintOneLayer(8,1,4,0,153);//8
    PrintOneLayer(9,1,4,0,149);//7
    PrintOneLayer(10,1,4,0,145);//6
    PrintOneLayer(11,1,4,0,141);//5

    PrintOneLayer(12,2,16,0,169);//12
    PrintOneLayer(13,2,16,0,185);//13
    PrintOneLayer(14,2,16,0,201);//14
    PrintOneLayer(15,2,16,0,217);//15
  */

  //DESY May 2016
  /*
    PrintOneLayer(1,2,16,0,169);//12
    PrintOneLayer(2,2,16,0,185);//13
    PrintOneLayer(3,2,16,0,201);//14
    PrintOneLayer(4,2,16,0,217);//15
  */

  /*
    PrintOneLayer(1,1,4,0,221);//SenSL Main Slab HBU3_5
    PrintOneLayer(2,1,4,0,229);//SenSL Side Slab HBU3_12
    PrintOneLayer(3,1,4,0,237);//New Mainz SM
  */

  //DESY July 2016
  //SMD
  PrintOneLayer(1,1,4,0,237);
  PrintOneLayer(2,1,4,0,241);
  PrintOneLayer(3,1,4,0,245);
  PrintOneLayer(4,1,4,0,121);
  PrintOneLayer(5,1,4,0,117);
  PrintOneLayer(6,1,4,0,249);
  PrintOneLayer(7,1,4,0,233);

  //SenSLs
  PrintOneLayer(8,1,4,0,225);
  PrintOneLayer(9,1,4,0,217);
  PrintOneLayer(10,1,4,0,213);
  PrintOneLayer(11,1,4,0,205);
  PrintOneLayer(12,1,4,0,221);
  PrintOneLayer(13,1,4,0,201);
  PrintOneLayer(14,1,4,0,209);
  PrintOneLayer(15,1,4,0,229);

  fOut.close();

}

