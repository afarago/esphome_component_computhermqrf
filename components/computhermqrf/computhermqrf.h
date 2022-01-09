#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/switch/switch.h"

#include "computhermrf.h"
#include "computhermqthermostat_binarysensorbase.h"
#include "computhermqthermostat_binarysensor.h"
#include "computhermqthermostat_switch.h"

#include <vector>
using namespace std;

namespace esphome {
namespace computhermqrf {

class ComputhermQRF : public PollingComponent {
  public:
    enum Message { none, heat_on, heat_off, pairing };

    void addSensor(ComputhermQThermostat_BinarySensor *sensor);
    void addSwitch(ComputhermQThermostat_Switch *aswitch);

    void set_receiver_pin(InternalGPIOPin *receiver_pin);
    void set_transmitter_pin(InternalGPIOPin *transmitter_pin);

  protected:
    InternalGPIOPin *receiver_pin_;
    InternalGPIOPin *transmitter_pin_;
    ComputhermRF *rfhandler_rf;

  private:
    static const uint16_t config_resend_interval_ = 60000;
    static const int rf_repeat_count = 4;
    unsigned long last_msg_time_ = 0;

    std::vector<ComputhermQThermostat_BinarySensor*> sensors;
    std::vector<ComputhermQThermostat_Switch*> switches;

    void dump_config() override;
    void update() override;
    void setup() override;
    void loop() override;

    ComputhermQThermostat_BinarySensor *findbyid(const char *device_sid);
    void send_msg(const char *code, Message msg);
    unsigned long calculate_diff(long now, long last_update);
    bool ComputhermQ_isONOFF(const char *state);
    const char* ComputhermQ_ONOFF(bool state);
};

}
}