#pragma once

#include "computhermqthermostat_binarysensorbase.h"

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

namespace esphome {
namespace computhermqrf {

#ifdef USE_COMPUTHERMQRF_BINARY_SENSOR

class ComputhermQThermostat_BinarySensor : public binary_sensor::BinarySensor, public Component, public ComputhermQThermostat_BinarySensorBase {
  public:
    virtual void setState(bool state) override;
    void dump_config() override;
};

#endif 

}
}