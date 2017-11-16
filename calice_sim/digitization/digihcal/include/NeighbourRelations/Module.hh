#ifndef NEIGHBOUR_MODULE_HH
#define NEIGHBOUR_MODULE_HH

#include "NeighbourRelations/Cell.hh"

#include <vector>
#include <utility>

namespace CALICE {

  namespace AHCAL {

    namespace Digitization {

      namespace NeighbourRelations {

        /** A module knows about its cells and neighbours.
         */
        class Module {

        public:
          /** All cells has to be set at construction time.
           */
          explicit Module(const std::vector<Cell>& allCells);

          virtual ~Module() {}
          
          /** 
           */
          std::vector<Cell> getNeighbours(const Cell&);

          /**
           */
          std::vector<std::pair<unsigned int,unsigned int> > getNeighbours(unsigned int I, unsigned int J);

        protected:

          /** Function which calculates all neibghour relations.
           */
          void evaluateNeighbourRelations();

        private:

          std::vector<Cell> _allCells;

        };

      }

    }

  }

}
#endif
