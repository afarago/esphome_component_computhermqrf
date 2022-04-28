#pragma once

#include <stdio.h>
namespace esphome {
namespace computhermqrf {

class ComputhermQThermostat_BaseUnit {
  public:
    const char *getCodeAsString() { return this->config_device_sid_; }
    unsigned long getCode() { return this->config_device_id_; }
    const char *getName() { return this->config_device_description_; }
    const char *getAbbreviation() { return this->config_device_abbrev_; }
    void setName(const char* name) { this->config_device_description_ = name; }
    void setCode(unsigned long code) { this->config_device_id_ = code; 
      //TODO: remove on long term
      snprintf( config_device_sid_, 6, "%05lX", code);
    }
    void setAbbreviation(const char* abbrev) { this->config_device_abbrev_ = abbrev; }

  protected:
    char config_device_sid_[6];
    unsigned long config_device_id_;
    const char* config_device_description_;
    const char* config_device_abbrev_;
};

}
}