#ifndef NEIGHBOUR_CELL_HH
#define NEIGHBOUR_CELL_HH

#include <set>
#include <utility>
#include <vector>
#include <iostream>

namespace CALICE {

  namespace AHCAL {

    namespace Digitization {

      namespace NeighbourRelations {

        /** The border is a vector of pairs of I and J
         */
        typedef std::set<std::pair<unsigned int, unsigned int> > Border;

        /** The cell knows its I, J, cellsize, neighbours and its border.
         */
        class Cell {

        public:

          Cell(unsigned int i, 
               unsigned int j, 
               unsigned int size) : _I(i), _J(j), _cellsize(size) {};

          bool operator==(const Cell&) const;
          bool operator!=(const Cell&) const;

          Cell& operator=(const Cell&);

          unsigned int I() const {return this->_I;}
          unsigned int J() const {return this->_J;}

          unsigned int size() const {return this->_cellsize;}

          void addNeighbour(Cell* newNeighbour);
  
          /** Get the I/J of the border of the cell on a Mokka like grid.
           */
          Border getBorder() const;

          std::vector<Cell> getNeighbours();

          /** 
           */
          bool isNeighbourOf(const Cell&) const;

        private:

          unsigned int _I;
          unsigned int _J;
          unsigned int _cellsize;

          std::vector<Cell*> _neighbours;

        };

      }

    }
  }
}
std::ostream& operator<<(std::ostream& output, 
                         const CALICE::AHCAL::Digitization::NeighbourRelations::Cell& c);

#endif
