#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"

#ifdef USE_COMPUTHERMQRF_BINARY_SENSOR

#include "esphome/components/binary_sensor/binary_sensor.h"

#include "computhermqthermostat_binarysensorbase.h"

namespace esphome {
namespace computhermqrf {

class ComputhermQThermostat_BinarySensor : public binary_sensor::BinarySensor, public Component, public ComputhermQThermostat_BinarySensorBase {
  public:
    virtual void setState(bool state) override;
    void dump_config() override;
};

}
}

#endif 
