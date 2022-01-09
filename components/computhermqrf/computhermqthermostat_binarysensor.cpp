#include "esphome.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"

#ifdef USE_COMPUTHERMQRF_BINARY_SENSOR

#include "esphome/components/binary_sensor/binary_sensor.h"
#include "computhermqthermostat_binarysensor.h"

namespace esphome {
namespace computhermqrf {

static const char* TAG = "computhermqrf.binarysensor";

void ComputhermQThermostat_BinarySensor::setState(bool state) {
    ComputhermQThermostat_BinarySensorBase::setState(state);
    this->publish_state(state);
}

void ComputhermQThermostat_BinarySensor::dump_config() {
  ESP_LOGCONFIG(TAG, "ComputhermQThermostat_BinarySensor:");
  ESP_LOGCONFIG(TAG, "  code:'%s' name: '%s'", this->getCode(), this->getName());
  LOG_BINARY_SENSOR("  ", "ComputhermQThermostat_BinarySensor", this);
}

}
}

#endif 