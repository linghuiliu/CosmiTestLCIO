#ifndef _TEMPGEARVARIABLES_HPP
#define _TEMPGEARVARIABLES_HPP

class tempGEARvariables {


 /** SymmetryOrder is the order of the rotational symmetry <br>
     *   8 for an octagonal barrel calorimeter<br>
     *   2 for an endcap calorimeter<br>
     *   1 for a standalone prototype<br>
     *   0 for an idealized cylindrical calorimeter.
     */

public:
  int nlayers;
  int nWafer0;
  int nWafer1;
  int nCellperWafer0;
  int nCellperWafer1;
  bool useEncoder32;

  tempGEARvariables(int SymmetryOrder){
    if (SymmetryOrder == 1)
      {
	nWafer0 = 3;
	nWafer1 = 3;
	nCellperWafer0 = 6;
	nCellperWafer1 = 6;
	nlayers = 30;
	useEncoder32 = true;
      }
    else if (SymmetryOrder == 2)
      {
	nWafer0 = 4;
	nWafer1 = 2;
	nCellperWafer0 = 86;
	nCellperWafer1 = 160;
	nlayers = 30;
	useEncoder32 = true;
      }
    else if (SymmetryOrder == 8)
      {
	nWafer0 = 8;
	nWafer1 = 5;
	nCellperWafer0 = 86;
	nCellperWafer1 = 160;
	nlayers = 30;
	useEncoder32 = true;
      }
    else{
      std::cerr << " Other unknown symmetryOrder, will use by default the prototype Geometry !! " << std::endl;
      nWafer0 = 3;
      nWafer1 = 3;
      nCellperWafer0 = 6;
      nCellperWafer1 = 6;
      nlayers = 30;
      useEncoder32 = true;
    }
  };

};
#endif
