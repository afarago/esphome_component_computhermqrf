#pragma once

namespace esphome {
namespace computhermqrf {

class ComputhermQThermostat_BinarySensorBase {
  public:
    ComputhermQThermostat_BinarySensorBase();
    virtual void setState(bool state);
    bool getState();
    const char *getName();
    const char *getCode();
    void setName(const char* name);
    void setCode(const char* code);

  protected:
    bool state_ = false;
    const char* config_device_sid_;
    unsigned long last_change_timestamp_ = 0;
    const char* config_device_description_;
};

}
}