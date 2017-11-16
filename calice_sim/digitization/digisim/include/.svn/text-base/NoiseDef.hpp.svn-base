#ifndef _NoiseDef_HPP_
#define  _NoiseDef_HPP_

#include <cassert>

class NoiseDef {

public:
  NoiseDef(int nLayer, int nchannelX, int nchannelY,  std::vector<float> parameters) {
    std::cout << "Initialisation of noise. For now : one noise per layer." << std::endl;
    assert(nLayer == 30);
    _nlayers = nLayer;
    _nChX = nchannelX;
    _nChY = nchannelY;
    _par = parameters;
  };

  double getNoise(int Layer, int channelX, int channelY) {
    //int pcb = static_cast<int>((Layer+0.1)/2.);
    return static_cast<double>(_par[Layer]);
  };

  double getCoherentNoise(int Layer) {
    //int pcb = static_cast<int>((Layer+0.1)/2.);
    return static_cast<double>(_par[Layer+30]);
  };

private:
  int _nlayers;
  int _nChX;
  int _nChY;
  std::vector<float> _par;

};

#endif
