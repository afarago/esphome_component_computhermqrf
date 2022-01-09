#include "computhermqthermostat_binarysensorbase.h"

namespace esphome {
namespace computhermqrf {

ComputhermQThermostat_BinarySensorBase::ComputhermQThermostat_BinarySensorBase() {
    this->state_ = false;

    //   // Read from LittleFs
    //   String filename = "/"+String(this->config_device_sid_);
    //   File devfile = LittleFS.open(filename, "r");
    //   if (devfile){
    //     String lastchanges = devfile.readString(); //readStringUntil, file.readbytes(buffer, length),  file.write(buffer, length) 
    //     last_change_timestamp = atol(lastchanges.c_str());
    //     devfile.close();
    //   }
}

void ComputhermQThermostat_BinarySensorBase::setState(bool state) {
    this->state_ = state;
    // //!! this->last_change_timestamp = ntphandler_getEpochTime();
    // this->publish_state(state);

    //   // Write to LittleFs
    // String filename = "/"+String(this->config_device_sid_);
    // File devfile = LittleFS.open(filename, "w");
    // if (devfile){
    //     devfile.print(last_change_timestamp);
    //     devfile.close();
    // }

    //   // if thermostat is ON, blink LED to show state X times
    //   if (newstate) this->display_blink_led()
}
bool ComputhermQThermostat_BinarySensorBase::getState() {
    return this->state_;
}
const char *ComputhermQThermostat_BinarySensorBase::getName() {
    return this->config_device_description_;
}
const char *ComputhermQThermostat_BinarySensorBase::getCode() {
    return this->config_device_sid_;
}
void ComputhermQThermostat_BinarySensorBase::setName(const char* name) {
    this->config_device_description_ = name;
}
void ComputhermQThermostat_BinarySensorBase::setCode(const char* code) {
    this->config_device_sid_ = code;
}

}
}