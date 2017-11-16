#ifndef ClusterShapesTyped_hh
#define ClusterShapesTyped_hh 1

#include "lcio.h"
#include "EVENT/LCCollection.h"
#include "ClusterShapes.h"

namespace CALICE {

  class ClusterShapesTyped {

  public:
    ClusterShapesTyped( ) : _shapes(0), _shapesPointer(&_shapes) {
    }

    virtual ~ClusterShapesTyped() {
      if (_shapes) delete _shapes;
    }

    template <class T>
    void fill(const lcio::LCCollection *col);

    ClusterShapes *getClusterShapesPointer() {return _shapes;}
    ClusterShapes **getClusterShapesPointerPointer() {return _shapesPointer;}

  private:

    void generateShapes() {
      if (_shapes) delete _shapes;
      if (_aHit.size() > 0) _shapes = new ClusterShapes(_aHit.size(), &(_aHit[0]), &(_xHit[0]), &(_yHit[0]), &(_zHit[0]) );
      *_shapesPointer = _shapes;
    }

    ClusterShapes *_shapes;
    ClusterShapes **_shapesPointer;

    std::vector<float> _aHit;
    std::vector<float> _xHit;
    std::vector<float> _yHit;
    std::vector<float> _zHit;

  };

} // end namepsace CALICE


#include "EVENT/CalorimeterHit.h"

namespace CALICE {

  template <class T>
  void ClusterShapesTyped::fill(const lcio::LCCollection *col) {

    for (int i = 0; i < col->getNumberOfElements(); ++i) {
      T* hit = dynamic_cast<T*>( col->getElementAt(i) );

      if (hit) {
        _aHit.push_back(hit->getEnergy() );
        _xHit.push_back( hit->getPosition()[0] );
        _yHit.push_back( hit->getPosition()[1] );
        _zHit.push_back( hit->getPosition()[2] );
      }
    }

    generateShapes();
  }


} //end namespace CALICE
#endif
