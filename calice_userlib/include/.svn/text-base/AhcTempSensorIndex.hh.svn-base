#ifndef AhcTempSensorIndex_h
#define AhcTempSensorIndex_h

namespace CALICE {

/**
 * \brief AhcTempSensorIndex represents and Ahcal Temperature Sensor Index
 *        So what? it encodes (modules,sensor) into an integer.
 */

class AhcTempSensorIndex
{
public:

    AhcTempSensorIndex(int key);
    AhcTempSensorIndex(int module,int sensor);

    void setKey(int key);
    void setModule(int m);
    void setSensor(int s);

    int getKey();
    int getModule();
    int getSensor();

    static const int MODULE_MASK = 0xff00;
    static const int MODULE_SHIFT = 8;

    static const int SENSOR_MASK = 0xff;
    static const int SENSOR_SHIFT = 0;

private:
    int _key;
};

}

#endif
