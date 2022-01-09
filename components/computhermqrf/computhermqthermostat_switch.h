#pragma once

#include "computhermqthermostat_binarysensorbase.h"

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace computhermqrf {

class ComputhermQThermostat_Switch : public switch_::Switch, public PollingComponent, public ComputhermQThermostat_BinarySensorBase {
  public:
    ComputhermQThermostat_Switch();

    void dump_config() override;
    void update() override;

    void setState(bool state) override;
    void write_state(bool state) override;
    uint8_t getPendingMessage();
    void setPendingMessage(uint8_t new_message);

  private:
    static const int rf_repeat_count = 4;
    static const uint8_t MSG_NONE = 0;
    static const uint8_t MSG_HEAT_ON = 1;
    static const uint8_t MSG_HEAT_OFF = 2;
    static const uint8_t MSG_PAIR = 3;
    static const uint16_t config_resend_interval_ = 60000;

    unsigned long last_msg_time_ = 0;
    uint8_t pending_msg_ = 0; // 0 = MSG_NONE

    void send_msg(uint8_t msg);
    unsigned long calculate_diff(long now, long last_update);
};

}
}