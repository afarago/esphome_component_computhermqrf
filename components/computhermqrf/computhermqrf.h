#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/switch/switch.h"

#include "computhermrf.h"
#include "computhermqthermostat_binarysensorbase.h"

#include <vector>
using namespace std;

namespace esphome {
namespace computhermqrf {

class ComputhermQRF : public Component {
  public:
    void addZone(ComputhermQThermostat_BinarySensorBase* sensor);

    void set_receiver_pin(InternalGPIOPin *receiver_pin);
    void set_transmitter_pin(InternalGPIOPin *transmitter_pin);

  protected:
    InternalGPIOPin *receiver_pin_;
    InternalGPIOPin *transmitter_pin_;
    ComputhermRF *rfhandler_rf;

  private:
    std::vector<ComputhermQThermostat_BinarySensorBase*> sensors;

    void dump_config() override;
    void setup() override;
    void loop() override;
    ComputhermQThermostat_BinarySensorBase *findbyid(const char* device_sid);
    bool ComputhermQ_isONOFF(const char *state);
    const char* ComputhermQ_ONOFF(bool state);
};

}
}