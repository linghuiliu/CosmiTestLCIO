
#include <math.h>

#include <iostream>

/*
 * This class collects statistics for a single cell
 *
*/
class SingleCellStatistics {

public:
  SingleCellStatistics() {

    _n_hits = 0;
    _energy_sum = 0;
    _energy_squared_sum = 0;
    _hit_energy_max = 0.;

  }

  ~SingleCellStatistics() {}

  SingleCellStatistics* add_hit( double ESum ){

    _n_hits++;
    _energy_sum += ESum;
    _energy_squared_sum += ( ESum * ESum );

    if ( ESum > _hit_energy_max )
      _hit_energy_max = ESum;

    return this;

  }

  int get_n_hits(){

    return _n_hits;

  }

  double get_energy_sum(){

    return _energy_sum;

  }

  double get_energy_mean(){

    if ( _n_hits > 0 )
      return ( _energy_sum / (double)_n_hits );
    else
      return 0;

  }

  double get_energy_rms(){

    if ( _n_hits > 0 )
      return sqrt( ( _energy_squared_sum / _n_hits ) - pow( _energy_sum / (double)_n_hits, 2 ) );
    else
      return 0;

  }

  double get_hit_energy_max(){

    return _hit_energy_max;

  }


private:

  int _n_hits;

  double _energy_sum;

  double _energy_squared_sum;

  double _hit_energy_max;

};
