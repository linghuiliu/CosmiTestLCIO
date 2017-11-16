#include "NeighbourRelations/FineModule.hh"
#include "NeighbourRelations/Cell.hh"

using namespace CALICE::AHCAL::Digitization::NeighbourRelations;

  FineModule::FineModule() : Module(createAllCells()) {};

  std::vector<Cell> FineModule::createAllCells() {

    std::vector<Cell> allCells;

    // coarse cells

    // bottom
    allCells.push_back( Cell(19,1,12) );
    allCells.push_back( Cell(31,1,12) );
    allCells.push_back( Cell(43,1,12) );
    allCells.push_back( Cell(55,1,12) );
    allCells.push_back( Cell(67,1,12) );

    // left
    allCells.push_back( Cell(1,13,12) );
    allCells.push_back( Cell(1,25,12) );
    allCells.push_back( Cell(1,37,12) );
    allCells.push_back( Cell(1,49,12) );
    allCells.push_back( Cell(1,61,12) );

    // top
    allCells.push_back( Cell(13,79,12) );
    allCells.push_back( Cell(25,79,12) );
    allCells.push_back( Cell(37,79,12) );
    allCells.push_back( Cell(49,79,12) );
    allCells.push_back( Cell(61,79,12) );

    // right
    allCells.push_back( Cell(79,19,12) );
    allCells.push_back( Cell(79,31,12) );
    allCells.push_back( Cell(79,43,12) );
    allCells.push_back( Cell(79,55,12) );
    allCells.push_back( Cell(79,67,12) );

    for(unsigned int i = 13 ; i < 74 ; i += 6) {
      for(unsigned int j = 13 ; j < 74 ; j += 6) {

        if ( i > 30 && i < 60 && j > 30 && j < 60 ) {
          // fine grid
        } else {
          
          allCells.push_back( Cell(i,j,6) );
          
        }
        
      }

    }

    for(unsigned int i = 31; i < 61; i += 3) {
      for(unsigned int j = 31; j < 61; j += 3 ) {
      
        allCells.push_back( Cell(i,j,3) );
      
      }

    }

    return allCells;

  }


