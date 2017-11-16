//Simple CellID decoder
//Created by A.-M. Magnan, 2006-06-26


#include "CellIDDecoder.hpp"
#include <assert.h>

CellIDDecoder::decoder::decoder()
{
  // assign the values to the arrays:
  shift_32[0] = 3; // S = 3 bits
  shift_32[1] = 0; // M = 3 bits
  shift_32[2] = 24; // K = 6 bits
  shift_32[3] = 6; // I = 9 bits
  shift_32[4] = 30; // 2
  shift_32[5] = 31; // 1
  shift_32[6] = 15; // J = 9 bits
  shift_32[7] = 0; // K+S+M = start at 0

  mask_32[0] = static_cast<unsigned int> (0x00000038);// S 
  mask_32[1] = static_cast<unsigned int> (0x00000007);// M
  mask_32[2] = static_cast<unsigned int> (0x3F000000); // K
  mask_32[3] = static_cast<unsigned int> (0x00007FC0); // I
  mask_32[4] = static_cast<unsigned int> (0x40000000); // 2
  mask_32[5] = static_cast<unsigned int> (0x80000000); // 1
  mask_32[6] = static_cast<unsigned int> (0x00FF8000); // J
  mask_32[7] = static_cast<unsigned int> (0x3F00003F); // S+M+K

  shift_64[0] = 0; //Stave = 3 bits
  shift_64[1] = 3; //Module = 3 bits
  shift_64[2] = 6; //Layer = 6 bits
  shift_64[3] = 12; //Cell X index = 16 bits
  shift_64[4] = 28; //Guard-ring zone = 3 bits
  shift_64[5] = 31; //Sign = 1 bit
  shift_64[6] = 0; //Cell Z index = 16 bits
  shift_64[7] = 16; //Provision = 15 bits
  shift_64[8] = 31; //Sign = 1 bit
  shift_64[9] = 0; //K+S+M start at 0

  mask_64[0] = static_cast<unsigned int> (0x00000007);// S
  mask_64[1] = static_cast<unsigned int> (0x00000038);// M
  mask_64[2] = static_cast<unsigned int> (0x00000FC0);// K
  mask_64[3] = static_cast<unsigned int> (0x0FFFF000);// I
  mask_64[4] = static_cast<unsigned int> (0x70000000);// Z
  mask_64[5] = static_cast<unsigned int> (0x80000000);// 1
  mask_64[6] = static_cast<unsigned int> (0x0000FFFF);// J
  mask_64[7] = static_cast<unsigned int> (0x7FFF0000);// P
  mask_64[8] = static_cast<unsigned int> (0x80000000);// 2
  mask_64[9] = static_cast<unsigned int> (0x00000FFF);// K+S+M

  shift_MAPS[0] = 0; //Stave = 3 bits
  shift_MAPS[1] = 3; //Module = 3 bits
  shift_MAPS[2] = 6; //Layer = 6 bits
  shift_MAPS[3] = 12; //Cell X index = 19 bits
  shift_MAPS[4] = 0; //Guard-ring zone = 3 bits
  shift_MAPS[5] = 31; //Sign = 1 bit
  shift_MAPS[6] = 3; //Cell Z index = 19 bits
  shift_MAPS[7] = 22; //Provision = 9 bits
  shift_MAPS[8] = 31; //Sign = 1 bit
  shift_MAPS[9] = 0; //K+S+M start at 0

  mask_MAPS[0] = static_cast<unsigned int> (0x00000007); //S
  mask_MAPS[1] = static_cast<unsigned int> (0x00000038); // M
  mask_MAPS[2] = static_cast<unsigned int> (0x00000FC0); // K
  mask_MAPS[3] = static_cast<unsigned int> (0x7FFFF000); // I
  mask_MAPS[4] = static_cast<unsigned int> (0x00000007); // Z
  mask_MAPS[5] = static_cast<unsigned int> (0x80000000); // 1
  mask_MAPS[6] = static_cast<unsigned int> (0x003FFFF8); // J
  mask_MAPS[7] = static_cast<unsigned int> (0x7FC00000); // P
  mask_MAPS[8] = static_cast<unsigned int> (0x80000000); // 2
  mask_MAPS[9] = static_cast<unsigned int> (0x00000FFF); // KSM
}
  // instantiation at static initialisation: 
//const CellIDDecoder::decoder mydecoder;

const CellIDDecoder::decoder & CellIDDecoder::GetDecoder(){
  static decoder ldecoder;
  return ldecoder;
}

CellIDDecoder::CellIDDecoder(const int cellID0, const int cellID1, int SymmetryOrder)
{
  _mycellID.id0 = static_cast<unsigned int>(cellID0);
  _mycellID.id1 = static_cast<unsigned int>(cellID1);
  if (SymmetryOrder == 16) {
    p_mask=GetDecoder().mask_MAPS;
    p_shift=GetDecoder().shift_MAPS;
    _is32 = false;
  }
  else if (SymmetryOrder == 1) {
    p_mask=GetDecoder().mask_32;
    p_shift=GetDecoder().shift_32;
    _is32 = true;
  }
 else {
    p_mask=GetDecoder().mask_64;
    p_shift=GetDecoder().shift_64;
    _is32 = false;
 }
  _symmetryOrder = SymmetryOrder;
}

CellIDDecoder::CellIDDecoder(long long cellID, int SymmetryOrder, bool useEncoder32)
{
  _mycellID.id0 = static_cast<unsigned int> (cellID) ;
  if (!useEncoder32) _mycellID.id1 = static_cast<unsigned int> (cellID >> 32);
  else _mycellID.id1 = static_cast<unsigned int> (0);
  _is32 = useEncoder32;
  if (_is32)
    {
      p_mask=GetDecoder().mask_32;
      p_shift=GetDecoder().shift_32;
    }
  else
    {
      if (SymmetryOrder == 16) {
	p_mask=GetDecoder().mask_MAPS;
	p_shift=GetDecoder().shift_MAPS;
      }
      else {
	p_mask=GetDecoder().mask_64;
	p_shift=GetDecoder().shift_64;
      }
    }
  _symmetryOrder = SymmetryOrder;
}
CellIDDecoder::CellIDDecoder(int cellID0, int SymmetryOrder)
{
  _mycellID.id0 = static_cast<unsigned int>(cellID0);
  _mycellID.id1 = 0;//static_cast<unsigned int>(cellID0);

  _symmetryOrder = SymmetryOrder;
  p_mask=GetDecoder().mask_32;
  p_shift=GetDecoder().shift_32;
  _is32 = true;
}

CellIDDecoder::CellIDDecoder(int SymmetryOrder, bool useEncoder32)
{
  _mycellID.id0 = 0;
  _mycellID.id1 = 0;

  _symmetryOrder = SymmetryOrder;
  _is32 = useEncoder32;
  if (_is32)
    {
      p_mask=GetDecoder().mask_32;
      p_shift=GetDecoder().shift_32;
    }
  else
    {
      if (SymmetryOrder == 16) {
	p_mask=GetDecoder().mask_MAPS;
	p_shift=GetDecoder().shift_MAPS;
      }
      else {
	p_mask=GetDecoder().mask_64;
	p_shift=GetDecoder().shift_64;
      }
    }

}
 

int CellIDDecoder::getL1()const {
  return ((_mycellID.id0 &p_mask[5]) >> p_shift[5]);
}

int CellIDDecoder::getL2()const {
  if (_is32) return ((_mycellID.id0 &p_mask[4]) >> p_shift[4]);
  return ((_mycellID.id1 &p_mask[8]) >> p_shift[8]);
}

int CellIDDecoder::getProv()const {
  if (_is32) return -1;
  return ((_mycellID.id1 &p_mask[7]) >> p_shift[7]);
}

int CellIDDecoder::getK()const {
  return ((_mycellID.id0 &p_mask[2]) >> p_shift[2]) +1;
}

int CellIDDecoder::getJ()const {
  if (_is32) return ((_mycellID.id0 &p_mask[6]) >> p_shift[6]);
  return ((_mycellID.id1 &p_mask[6]) >> p_shift[6]);
}

int CellIDDecoder::getI()const {
  return ((_mycellID.id0 &p_mask[3]) >> p_shift[3]);
}

int CellIDDecoder::getImax()const {
  return (p_mask[3] >> p_shift[3]);
}

int CellIDDecoder::getJmax()const {
  return (p_mask[6] >> p_shift[6]);
}

int CellIDDecoder::getM()const {
  return ((_mycellID.id0 &p_mask[1]) >> p_shift[1]);
}

int CellIDDecoder::getS()const {
  return ((_mycellID.id0 &p_mask[0]) >> p_shift[0]) +1;
}

int CellIDDecoder::getKSM()const {
  if (_is32) return ((_mycellID.id0 &p_mask[7]) >> p_shift[7]);
  return ((_mycellID.id0 &p_mask[9]) >> p_shift[9]);
}
  
int CellIDDecoder::getGRZone()const {
  //is meaningless for 32bits decoder.
  if (_is32) return -1;
  if (_symmetryOrder == 16) return ((_mycellID.id1 &p_mask[4]) >> p_shift[4]);
  return ((_mycellID.id0 &p_mask[4]) >> p_shift[4]);
}

// int CellIDDecoder::getMaxClosestNeighbours(){
//   if ( (this->getJ() == 0 || this->getJ() == 12000) && (this->getI() == 0 || this->getI() == 12000) ) return 3;
//   else if ((this->getI() == 0 || this->getJ() == 0) || (this->getI() == 12000 || this->getJ() == 12000)) return 5;
//   return 8;
// }

int CellIDDecoder::isSameWafer(const CellIDDecoder & neighbour){
  if(this->getS()==neighbour.getS() && this->getM()==neighbour.getM() && this->getK()==neighbour.getK()){
    return 1;
  }
  return 0;
}

int CellIDDecoder::getJ(int NCELLS) {
  return static_cast<unsigned int> (this->getJ()*1.0/NCELLS) ;
}

int CellIDDecoder::getI(int NCELLS) {
  return static_cast<unsigned int>(this->getI()*1.0/NCELLS) ;
}

std::pair<int, int> CellIDDecoder::Encode() {

  std::pair<int, int> temp;

  if (_is32)
    {
      temp.first  = (  ( (getL1() << p_shift[5]) & p_mask[5]) |
		       ( (getL2() << p_shift[4]) & p_mask[4]) |
		       ( ((getS()-1) << p_shift[0]) & p_mask[0]) |
		       ( ((getM()) << p_shift[1]) & p_mask[1]) |
		       ( (getI() << p_shift[3]) & p_mask[3]) |
		       ( (getJ() << p_shift[6]) & p_mask[6]) |
		       ( ((getK()-1) << p_shift[2]) & p_mask[2]) );

      temp.second = temp.first;
    }
  else
    {
      if (_symmetryOrder == 16)
	{//maps
	  temp.first  = ( ( ((getS()-1) << p_shift[0]) & p_mask[0]) |
			  ( (getM() << p_shift[1]) & p_mask[1]) |
			  ( ((getK()-1) << p_shift[2]) & p_mask[2]) |
			  ( (getI() << p_shift[3]) & p_mask[3])|
			  ( (getL1() << p_shift[5]) & p_mask[5])  );
                                                                                
	  temp.second = ( ( (getGRZone() << p_shift[4]) & p_mask[4]) |
			  ( (getJ() << p_shift[6]) & p_mask[6]) |
			  ( (getProv() << p_shift[7]) & p_mask[7]) |
			  ( (getL2() << p_shift[8]) & p_mask[8])  );
	}
      else
	{
	  temp.first  = ( ( ((getS()-1) << p_shift[0]) & p_mask[0]) |
			  ( (getM() << p_shift[1]) & p_mask[1]) |
			  ( ((getK()-1) << p_shift[2]) & p_mask[2]) |
			  ( (getI() << p_shift[3]) & p_mask[3])|
			  ( (getGRZone() << p_shift[4]) & p_mask[4]) |
			  ( (getL1() << p_shift[5]) & p_mask[5])  );
                                                                                
	  temp.second = ( ( (getJ() << p_shift[6]) & p_mask[6]) |
			  ( (getProv() << p_shift[7]) & p_mask[7]) |
			  ( (getL2() << p_shift[8]) & p_mask[8])  );
	}
    }
  return temp;
}
std::pair<int, int> CellIDDecoder::Encode(int K, int S, int M, int I, int J) {

  std::pair<int, int> temp;

  if (_is32)
    {
      temp.first  = (  ( (getL1() << p_shift[5]) & p_mask[5]) |
		       ( (getL2() << p_shift[4]) & p_mask[4]) |
		       ( ((S-1) << p_shift[0]) & p_mask[0]) |
		       ( ((M) << p_shift[1]) & p_mask[1]) |
		       ( (I << p_shift[3]) & p_mask[3]) |
		       ( (J << p_shift[6]) & p_mask[6]) |
		       ( ((K-1) << p_shift[2]) & p_mask[2]) );

      temp.second = 0;//temp.first;
    }
  else
    {
      if (_symmetryOrder == 16)
	{//maps
	  temp.first  = ( ( ((S-1) << p_shift[0]) & p_mask[0]) |
			  ( (M << p_shift[1]) & p_mask[1]) |
			  ( ((K-1) << p_shift[2]) & p_mask[2]) |
			  ( (I << p_shift[3]) & p_mask[3])|
			  ( (getL1() << p_shift[5]) & p_mask[5])  );
                                                                                
	  temp.second = ( ( (getGRZone() << p_shift[4]) & p_mask[4]) |
			  ( (J << p_shift[6]) & p_mask[6]) |
			  ( (getProv() << p_shift[7]) & p_mask[7]) |
			  ( (getL2() << p_shift[8]) & p_mask[8])  );
	}
      else
	{
	  temp.first  = ( ( ((S-1) << p_shift[0]) & p_mask[0]) |
			  ( (M << p_shift[1]) & p_mask[1]) |
			  ( ((K-1) << p_shift[2]) & p_mask[2]) |
			  ( (I << p_shift[3]) & p_mask[3])|
			  ( (getGRZone() << p_shift[4]) & p_mask[4]) |
			  ( (getL1() << p_shift[5]) & p_mask[5])  );
                                                                                
	  temp.second = ( ( (J << p_shift[6]) & p_mask[6]) |
			  ( (getProv() << p_shift[7]) & p_mask[7]) |
			  ( (getL2() << p_shift[8]) & p_mask[8])  );
	}
    }

  _mycellID.id0 = static_cast<unsigned int>(temp.first);
  if (_is32) _mycellID.id1 = 0;
  else _mycellID.id1 = static_cast<unsigned int>(temp.second);

  return temp;
}

std::pair<int, int> CellIDDecoder::Encode(int NCELLS) {

  std::pair<int, int> temp;

  if (_is32)
    {
      temp.first  = (  ( (getL1() << p_shift[5]) & p_mask[5]) |
		       ( (getL2() << p_shift[4]) & p_mask[4]) |
		       ( ((getS()-1) << p_shift[0]) & p_mask[0]) |
		       ( ((getM()) << p_shift[1]) & p_mask[1]) |
		       ( (getI(NCELLS) << p_shift[3]) & p_mask[3]) |
		       ( (getJ(NCELLS) << p_shift[6]) & p_mask[6]) |
		       ( ((getK()-1) << p_shift[2]) & p_mask[2]) );

      temp.second = 0;// temp.first;
    }
  else
    {
      if (_symmetryOrder == 16)
	{//maps
	  temp.first  = ( ( ((getS()-1) << p_shift[0]) & p_mask[0]) |
			  ( (getM() << p_shift[1]) & p_mask[1]) |
			  ( ((getK()-1) << p_shift[2]) & p_mask[2]) |
			  ( (getI(NCELLS) << p_shift[3]) & p_mask[3])|
			  ( (getL1() << p_shift[5]) & p_mask[5])  );
                                                                                
	  temp.second = ( ( (getGRZone() << p_shift[4]) & p_mask[4]) |
			  ( (getJ(NCELLS) << p_shift[6]) & p_mask[6]) |
			  ( (getProv() << p_shift[7]) & p_mask[7]) |
			  ( (getL2() << p_shift[8]) & p_mask[8])  );
	}
      else
	{
	  temp.first  = ( ( ((getS()-1) << p_shift[0]) & p_mask[0]) |
			  ( (getM() << p_shift[1]) & p_mask[1]) |
			  ( ((getK()-1) << p_shift[2]) & p_mask[2]) |
			  ( (getI(NCELLS) << p_shift[3]) & p_mask[3])|
			  ( (getGRZone() << p_shift[4]) & p_mask[4]) |
			  ( (getL1() << p_shift[5]) & p_mask[5])  );
                                                                                
	  temp.second = ( ( (getJ(NCELLS) << p_shift[6]) & p_mask[6]) |
			  ( (getProv() << p_shift[7]) & p_mask[7]) |
			  ( (getL2() << p_shift[8]) & p_mask[8])  );
	}
    }
  return temp;
}

long long CellIDDecoder::EncodeNeigh(int deltai,int deltaj,int NCELLS) {

  std::pair<int, int> temp;

  if (_is32)
    {
      temp.first  = (  ( (getL1() << p_shift[5]) & p_mask[5]) |
		       ( (getL2() << p_shift[4]) & p_mask[4]) |
		       ( ((getS()-1) << p_shift[0]) & p_mask[0]) |
		       ( ((getM()) << p_shift[1]) & p_mask[1]) |
		       ( ((getI(NCELLS)+deltai) << p_shift[3]) & p_mask[3]) |
		       ( ((getJ(NCELLS)+deltaj) << p_shift[6]) & p_mask[6]) |
		       ( ((getK()-1) << p_shift[2]) & p_mask[2]) );

      temp.second = 0;//temp.first;
    }
  else
    {
      if (_symmetryOrder == 16)
	{//maps
	  temp.first  = ( ( ((getS()-1) << p_shift[0]) & p_mask[0]) |
			  ( (getM() << p_shift[1]) & p_mask[1]) |
			  ( ((getK()-1) << p_shift[2]) & p_mask[2]) |
			  ( ((getI(NCELLS)+deltai) << p_shift[3]) & p_mask[3])|
			  ( (getL1() << p_shift[5]) & p_mask[5])  );
                                                                                
	  temp.second = ( ( (getGRZone() << p_shift[4]) & p_mask[4]) |
			  ( ((getJ(NCELLS)+deltaj) << p_shift[6]) & p_mask[6]) |
			  ( (getProv() << p_shift[7]) & p_mask[7]) |
			  ( (getL2() << p_shift[8]) & p_mask[8])  );
	}
      else
	{
	  temp.first  = ( ( ((getS()-1) << p_shift[0]) & p_mask[0]) |
			  ( (getM() << p_shift[1]) & p_mask[1]) |
			  ( ((getK()-1) << p_shift[2]) & p_mask[2]) |
			  ( ((getI(NCELLS)+deltai) << p_shift[3]) & p_mask[3])|
			  ( (getGRZone() << p_shift[4]) & p_mask[4]) |
			  ( (getL1() << p_shift[5]) & p_mask[5])  );
                                                                                
	  temp.second = ( ( ((getJ(NCELLS)+deltaj) << p_shift[6]) & p_mask[6]) |
			  ( (getProv() << p_shift[7]) & p_mask[7]) |
			  ( (getL2() << p_shift[8]) & p_mask[8])  );
	}
    }

  return ((static_cast<long long>(temp.second) << 32) + static_cast<long long>(temp.first));

}

long long CellIDDecoder::EncodeNeigh(int deltai,int deltaj) {

  std::pair<int, int> temp;
  temp.first = 0;
  temp.second = 0;

  if (_is32)
    {
      temp.first  = (  ( (this->getL1() << p_shift[5]) & p_mask[5]) |
		       ( (this->getL2() << p_shift[4]) & p_mask[4]) |
		       ( ((this->getS()-1) << p_shift[0]) & p_mask[0]) |
		       ( ((this->getM()) << p_shift[1]) & p_mask[1]) |
		       ( ((this->getI()+deltai) << p_shift[3]) & p_mask[3]) |
		       ( ((this->getJ()+deltaj) << p_shift[6]) & p_mask[6]) |
		       ( ((this->getK()-1) << p_shift[2]) & p_mask[2]) );

      temp.second = 0;//temp.first;
    }
  else
    {
      if (_symmetryOrder == 16)
	{//maps
	  temp.first  = ( ( ((this->getS()-1) << p_shift[0]) & p_mask[0]) |
			  ( (this->getM() << p_shift[1]) & p_mask[1]) |
			  ( ((this->getK()-1) << p_shift[2]) & p_mask[2]) |
			  ( ((this->getI()+deltai) << p_shift[3]) & p_mask[3])|
			  ( (this->getL1() << p_shift[5]) & p_mask[5])  );
                                                                                
	  temp.second = ( ( (this->getGRZone() << p_shift[4]) & p_mask[4]) |
			  ( ((this->getJ()+deltaj) << p_shift[6]) & p_mask[6]) |
			  ( (this->getProv() << p_shift[7]) & p_mask[7]) |
			  ( (this->getL2() << p_shift[8]) & p_mask[8])  );


	}
      else
	{
	  temp.first  = ( ( ((this->getS()-1) << p_shift[0]) & p_mask[0]) |
			  ( (this->getM() << p_shift[1]) & p_mask[1]) |
			  ( ((this->getK()-1) << p_shift[2]) & p_mask[2]) |
			  ( ((this->getI()+deltai) << p_shift[3]) & p_mask[3])|
			  ( (this->getGRZone() << p_shift[4]) & p_mask[4]) |
			  ( (this->getL1() << p_shift[5]) & p_mask[5])  );
                                                                                
	  temp.second = ( ( ((this->getJ()+deltaj) << p_shift[6]) & p_mask[6]) |
			  ( (this->getProv() << p_shift[7]) & p_mask[7]) |
			  ( (this->getL2() << p_shift[8]) & p_mask[8])  );
	}
    }

  if (deltai == 0) assert(temp.first == static_cast<int>(_mycellID.id0));
  if (deltaj == 0) assert(temp.second == static_cast<int>(_mycellID.id1));

  return ((static_cast<long long>(temp.second) << 32) + static_cast<long long>(temp.first));
}
