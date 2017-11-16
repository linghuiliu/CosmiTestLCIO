#include "Mapper.hh"

#include "CellIterator.hh"

namespace CALICE {

  Mapper::Mapper()  : _version(0), _decoderSet(0) {
    _decoderSet = new DecoderSet();
  }

  Mapper::Mapper(const std::string& cellIDencoding, const std::string& moduleEncoding, const std::string& DAQencoding ) :
    _version(0), _decoderSet(0) {
    _decoderSet = new DecoderSet(cellIDencoding, moduleEncoding, DAQencoding);
  }

  CellIterator Mapper::begin() const {
    return CellIterator(this);
  }

  CellIterator Mapper::end() const {
    return CellIterator(this,getMaxIndex());
  }

}
