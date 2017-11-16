#include "DecoderSet.hh"

namespace CALICE {

  const std::string DecoderSet::_defaultCellIDEncoding("M:3,S-1:3,I:9,J:9,K-1:6");
  const std::string DecoderSet::_defaultModuleEncoding("module:0:6,chip:6:5,chan:11:5,SiPM:16:16");
  const std::string DecoderSet::_defaultDAQEncoding("channel:8,chip:4,fe:3,slot:5,crate:8");


  DecoderSet::DecoderSet() : _IFromCellID(0),_JFromCellID(0), _KFromCellID(0),
                             _moduleFromModuleID(0), _chipFromModuleID(0),_channelFromModuleID(0),
                             _channelFromDAQID(0), _chipFromDAQID(0), _feFromDAQID(0),
			     _slotFromDAQID(0), _crateFromDAQID(0) 
  {
    setCellIDEncoding(_defaultCellIDEncoding);
    setModuleEncoding(_defaultModuleEncoding);
    setDAQEncoding(_defaultDAQEncoding);
  }


  DecoderSet::DecoderSet(const std::string& cellIDencoding, 
			 const std::string& moduleEncoding, 
			 const std::string& DAQencoding ) :
    _IFromCellID(0),_JFromCellID(0), _KFromCellID(0),
    _moduleFromModuleID(0), _chipFromModuleID(0),_channelFromModuleID(0),
    _channelFromDAQID(0), _chipFromDAQID(0), _feFromDAQID(0), _slotFromDAQID(0), _crateFromDAQID(0) 
  {
    setCellIDEncoding(cellIDencoding);
    setModuleEncoding(moduleEncoding);
    setDAQEncoding(DAQencoding);
  }

  /*
    Generating the FastDecoder is expensive in terms of speed.
    By using a map for each encoding, we don't have to generate
    the FastDecoder again, if the encoding has already been used.
  */

  void DecoderSet::setCellIDEncoding(const std::string &encoding) {
    if (encoding == "") _cellIDencodingString = _defaultCellIDEncoding;
    else                _cellIDencodingString = encoding;

    mokkaDecoderMap_t::iterator it = _mokkaDecoderMap.find(encoding);

    if ( it == _mokkaDecoderMap.end() ) {
      _IFromCellID = FastDecoder::generateDecoder(_cellIDencodingString, "I");
      _JFromCellID = FastDecoder::generateDecoder(_cellIDencodingString, "J");
      _KFromCellID = FastDecoder::generateDecoder(_cellIDencodingString, "K");

      MokkaID_t decoders;
      decoders.forI = _IFromCellID;
      decoders.forJ = _JFromCellID;
      decoders.forK = _KFromCellID;

      _mokkaDecoderMap[encoding] = decoders;
    }
    else {
      _IFromCellID = it->second.forI;
      _JFromCellID = it->second.forJ;
      _KFromCellID = it->second.forK;
    }
  }

  void DecoderSet::setModuleEncoding(const std::string &encoding) {
    if (encoding == "") _moduleIDEncodingString = _defaultModuleEncoding;
    else  _moduleIDEncodingString = encoding;

    if (_moduleFromModuleID)  delete _moduleFromModuleID;
    if (_chipFromModuleID)    delete _chipFromModuleID;
    if (_channelFromModuleID) delete _channelFromModuleID;
    _moduleFromModuleID  = FastDecoder::generateDecoder(_moduleIDEncodingString,"module");
    _chipFromModuleID    = FastDecoder::generateDecoder(_moduleIDEncodingString,"chip");
    _channelFromModuleID = FastDecoder::generateDecoder(_moduleIDEncodingString,"chan");
  }


  void DecoderSet::setDAQEncoding(const std::string &encoding) {
    if (encoding == "") _daqIDencodingString = _defaultDAQEncoding;
    else _daqIDencodingString = encoding;

    if (_channelFromDAQID) delete _channelFromDAQID;
    if (_chipFromDAQID)    delete _chipFromDAQID;
    if (_feFromDAQID)      delete _feFromDAQID;
    if (_slotFromDAQID)    delete _slotFromDAQID;
    if (_crateFromDAQID)   delete _crateFromDAQID;
    _channelFromDAQID = FastDecoder::generateDecoder(_daqIDencodingString,"channel");
    _chipFromDAQID    = FastDecoder::generateDecoder(_daqIDencodingString,"chip");
    _feFromDAQID      = FastDecoder::generateDecoder(_daqIDencodingString,"fe");
    _slotFromDAQID    = FastDecoder::generateDecoder(_daqIDencodingString,"slot");
    _crateFromDAQID   = FastDecoder::generateDecoder(_daqIDencodingString,"crate");
  }

} // end namespace CALICE
