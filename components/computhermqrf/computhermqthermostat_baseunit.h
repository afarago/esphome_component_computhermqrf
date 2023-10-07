#pragma once

#include <stdio.h>
namespace esphome {
namespace computhermqrf {

class ComputhermQThermostat_BaseUnit {
  public:
    const std::string getCodeAsString() { return this->config_device_sid_; }
    unsigned long getCode() { return this->config_device_id_; }
    const std::string& getName() { return this->config_device_description_; }
    const std::string& getAbbreviation() { return this->config_device_abbrev_; }
    void setName(const char* value) { this->config_device_description_ = std::string(value); }
    void setCode(const unsigned long code) { this->config_device_id_ = code; 
      //TODO: remove on long term
      snprintf( config_device_sid_, 6, "%05lX", code);
    }
    void setAbbreviation(const char* abbrev) { this->config_device_abbrev_ = std::string(abbrev); }

  protected:
    char config_device_sid_[6];
    unsigned long config_device_id_;
    std::string config_device_description_;
    std::string config_device_abbrev_;
};

}
}