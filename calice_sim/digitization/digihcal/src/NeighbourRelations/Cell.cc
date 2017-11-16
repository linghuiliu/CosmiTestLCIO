#include "NeighbourRelations/Cell.hh"
#include <iostream>
#include <algorithm>

using namespace CALICE::AHCAL::Digitization::NeighbourRelations;

  Border Cell::getBorder() const {

    Border thisBorder;

    for(unsigned int i = this->I(); i < (this->I() + this->size() + 1); ++i) {

      //std::cout << i << " " << this->J() << std::endl;

      thisBorder.insert(std::make_pair(i,  this->J()));
      thisBorder.insert(std::make_pair(i , this->J()  + this->size()));


    }

    for(unsigned int j = this->J(); j < (this->J() + this->size() + 1); ++j) {

      thisBorder.insert(std::make_pair(this->I() ,  j ));
      thisBorder.insert(std::make_pair(this->I() + this->size() ,j));

      //std::cout << this->I() << " " << j << std::endl;

    }

    return thisBorder;

  }

  bool Cell::operator==(const Cell& rhs) const{

    if( (this->_I == rhs.I()) &&
        (this->_J == rhs.J()) &&
        (this->_cellsize == rhs.size())) {

      return true;
    
    } else {
    
      return false;
    
    }
  
  }

  bool Cell::operator!=(const Cell& rhs) const{

    return !(*this == rhs);
  
  }

  Cell& Cell::operator=(const Cell& rhs) {
  
    this->_I = rhs.I();
    this->_J = rhs.J();
    this->_cellsize = rhs.size();
  
    return *this;
  
  }

  bool Cell::isNeighbourOf(const Cell& otherCell) const {
    
    if((*this) == otherCell) {

      return false; 

    } else {

      const Border& thisBorder = this->getBorder();
    
      const Border& otherCellsBorder = otherCell.getBorder();

      Border borderIntersection;

      set_intersection(thisBorder.begin(),
                       thisBorder.end(),
                       otherCellsBorder.begin(),
                       otherCellsBorder.end(),
                       inserter(borderIntersection,
                                borderIntersection.begin()));

      if ( borderIntersection.size() > 1 ) {

        return true;

      } else {

        return false;

      }

    }

  }

  void Cell::addNeighbour(Cell* newNeighbour) {

    // if (this->_neighbours.size() < 4 ) {
      
      this->_neighbours.push_back(newNeighbour);
      
      //  return true;
      
      //} else {
      
      //  return false;

      //}
    
  }

  std::vector<Cell> Cell::getNeighbours() {

    std::vector<Cell> neighbours;

    for(unsigned int i = 0; i != this->_neighbours.size(); ++i) {

      neighbours.push_back(*(this->_neighbours[i]));

    }

    return neighbours;

  }

std::ostream& operator<<(std::ostream& output, const Cell& c) {
  
  output << "I: " << c.I() << " J: " << c.J() 
         << " size: " << c.size();

  return output;  // for multiple << operators.
}
