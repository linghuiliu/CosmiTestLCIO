#ifndef COARSE_MODULE_HH
#define COARSE_MODULE_HH

#include "NeighbourRelations/Module.hh"
#include "NeighbourRelations/Cell.hh"

#include <vector>

namespace CALICE {

  namespace AHCAL {

    namespace Digitization {

      namespace NeighbourRelations {

        /** A coarse module. It's implementations has a list of all
            cells. Has to be changed if cell numbering scheme changes!
         */
        class CoarseModule : public Module {

        public:

          CoarseModule();

        private:

          static std::vector<Cell> createAllCells();

        };

      }

    }
  }
}
#endif
