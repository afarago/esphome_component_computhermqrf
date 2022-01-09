#include "computhermqthermostat_baseunit.h"

namespace esphome {
namespace computhermqrf {

const char *ComputhermQThermostat_BaseUnit::getName() {
    return this->config_device_description_;
}
const char *ComputhermQThermostat_BaseUnit::getCode() {
    return this->config_device_sid_;
}
void ComputhermQThermostat_BaseUnit::setName(const char* name) {
    this->config_device_description_ = name;
}
void ComputhermQThermostat_BaseUnit::setCode(const char* code) {
    this->config_device_sid_ = code;
}

}
}