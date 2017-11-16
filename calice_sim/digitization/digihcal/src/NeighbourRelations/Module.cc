#include "NeighbourRelations/Module.hh"

#include <algorithm>

#include <iostream>

using namespace CALICE::AHCAL::Digitization::NeighbourRelations;

  Module::Module(const std::vector<Cell>& allCells)  : 
  
    _allCells(allCells)

  {

    //std::cout << "Module() called" << std::endl;
    //
    //std::cout << _allCells.size() << " cells." << std::endl;

    this->evaluateNeighbourRelations();

  }

  void Module::evaluateNeighbourRelations() {

    //std::cout << "Module: evaluateNeighbourRelations() " << std::endl;

    for(unsigned int acellnr = 0; acellnr != this->_allCells.size();
        ++acellnr ) {

      for(unsigned int anothercellnr = 0; anothercellnr != this->_allCells.size();
          ++anothercellnr ) {

        if(_allCells[acellnr].isNeighbourOf(_allCells[anothercellnr])) {

          //std::cout << _allCells[acellnr] << " is neighbour of "
          //          << _allCells[anothercellnr] << std::endl;
            
          _allCells[acellnr].addNeighbour(&(_allCells[anothercellnr]));

        }

      }

    }

  }

  std::vector<Cell> Module::getNeighbours(const Cell& cell) {

    // std::cout << "Module: getNeighbours()" << std::endl;

    // no copy_if in the stl

    std::vector<Cell> neighbours;

    for(unsigned int cellnr = 0 ; cellnr != this->_allCells.size();
        ++cellnr) {

      if(cell == _allCells[cellnr]) {

        neighbours = _allCells[cellnr].getNeighbours();

      }
    
    }

    return neighbours;

  }

  std::vector<std::pair<unsigned int, unsigned int> > Module::getNeighbours(unsigned int I, unsigned int J) {

    // std::cout << "Module: getNeighbours()" << std::endl;

    // no copy_if in the stl

    std::vector<Cell> neighbours;

    for(unsigned int cellnr = 0 ; cellnr != this->_allCells.size();
        ++cellnr) {

      if(
         I == _allCells[cellnr].I() &&
         J == _allCells[cellnr].J()
         ) {

        neighbours = _allCells[cellnr].getNeighbours();

      }
    
    }

    std::vector<std::pair<unsigned int, unsigned int> > IJ_neighbours;

    for(unsigned int cellnr=0; cellnr != neighbours.size(); ++cellnr) {

      IJ_neighbours.push_back(std::make_pair(neighbours[cellnr].I(),
                                             neighbours[cellnr].J())
                              );

    }

    return IJ_neighbours;

  }
