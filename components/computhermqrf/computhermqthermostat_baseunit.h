#pragma once

namespace esphome {
namespace computhermqrf {

class ComputhermQThermostat_BaseUnit {
  public:
    const char *getName();
    const char *getCode();
    void setName(const char* name);
    void setCode(const char* code);

  protected:
    const char* config_device_sid_;
    const char* config_device_description_;
};

}
}