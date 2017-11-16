#include "AhcTempSensorIndex.hh"

namespace CALICE {
AhcTempSensorIndex::AhcTempSensorIndex(int key) { setKey(key); }


AhcTempSensorIndex::AhcTempSensorIndex(int module,int sensor)
{
    setKey(0); //important! otherwise key is randomly initiallized!
    setModule(module); setSensor(sensor);
}


void AhcTempSensorIndex::setKey(int key) { _key = key; }


void AhcTempSensorIndex::setModule(int m)
{ _key = (_key & ~MODULE_MASK) | ((m << MODULE_SHIFT) & MODULE_MASK); }


void AhcTempSensorIndex::setSensor(int s)
{ _key = (_key & ~SENSOR_MASK) | ((s << SENSOR_SHIFT) & SENSOR_MASK); }


int AhcTempSensorIndex::getSensor() { return (_key & SENSOR_MASK) >> SENSOR_SHIFT; }


int AhcTempSensorIndex::getModule() { return (_key & MODULE_MASK) >> MODULE_SHIFT; }


int AhcTempSensorIndex::getKey() { return _key; }


}
