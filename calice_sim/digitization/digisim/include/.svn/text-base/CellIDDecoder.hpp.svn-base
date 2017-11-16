#ifndef _CELLIDDECODER_HPP_
#define _CELLIDDECODER_HPP_

#include <utility>

class CellIDDecoder
{
public:
  class decoder {
    friend class CellIDDecoder;
    int mask_32 [8];
    int shift_32 [8];
    int mask_64 [10];
    int shift_64 [10];
    int mask_MAPS [10];
    int shift_MAPS [10];
  public:
    decoder();
  };

  
  static const decoder & GetDecoder(); 

private:
typedef struct {
        unsigned int id0;
        unsigned int id1;
}cellID;

  

public:

  //basic constructor
  CellIDDecoder(const int cellID0, const int cellID1, int SymmetryOrder);
  CellIDDecoder(long long cellID, int SymmetryOrder, bool useEncoder32);
  CellIDDecoder(int cellID0, int SymmetryOrder);
  CellIDDecoder(int SymmetryOrder, bool useEncoder32);

  CellIDDecoder(){};
  //destructor
  ~CellIDDecoder(){};

 /** SymmetryOrder is the order of the rotational symmetry <br>
     *   8 for an octagonal barrel calorimeter<br>
     *   2 for an endcap calorimeter<br>
     *   1 for a standalone prototype<br>
     *   0 for an idealized cylindrical calorimeter.
     */

  int getL1()const;
  int getL2()const;
  int getProv()const;
  int getK()const;
  int getM()const;
  int getS()const;
  int getI()const;
  int getJ()const;
  int getImax()const;
  int getJmax()const;
  int getKSM()const;
  int getGRZone()const;
  //  int getMaxClosestNeighbours();
  int isSameWafer(const CellIDDecoder & neighbour);
  int getI(int NCELLS);
  int getJ(int NCELLS);
  std::pair<int,int> Encode();
  std::pair<int,int> Encode(int NCELLS);
  std::pair<int,int> Encode(int K, int S, int M, int I, int J);
  long long EncodeNeigh(int deltai,int deltaj,int NCELLS);
  long long EncodeNeigh(int deltai,int deltaj);

private:
  cellID _mycellID;
  bool _is32;
  int _symmetryOrder;
  //const static decoder mydecoder;

  const int * p_mask;
  const int * p_shift;
  

};
#endif
