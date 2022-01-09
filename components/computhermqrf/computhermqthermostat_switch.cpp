#include "esphome.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/switch/switch.h"

#include "computhermqthermostat_switch.h"

namespace esphome {
namespace computhermqrf {

static const char* TAG = "computhermqrf.switch";

ComputhermQThermostat_Switch::ComputhermQThermostat_Switch():
    PollingComponent(), // default defined in .py 1000 -- update every 1000ms 
    ComputhermQThermostat_BinarySensorBase() { 
}

void ComputhermQThermostat_Switch::setState(bool state) {
    ComputhermQThermostat_BinarySensorBase::setState(state);
    this->publish_state(state);
}

void ComputhermQThermostat_Switch::write_state(bool state) {
    // This will be called every time the user requests a state change.
    this->setState(state);
    this->pending_msg_ = state ? MSG_HEAT_ON : MSG_HEAT_OFF;
}

unsigned long ComputhermQThermostat_Switch::calculate_diff(long now, long last_update) {  
  unsigned long diff = 0UL;

  if (last_update > now) {
    // millis() overflows every ~50 days
    diff = (ULONG_MAX - last_update) + now;
  } else {
    diff = now - last_update;
  }
  return diff;
}

void ComputhermQThermostat_Switch::dump_config() {
  ESP_LOGCONFIG(TAG, "ComputhermQThermostat_Switch:");
  ESP_LOGCONFIG(TAG, "  code:'%s' name: '%s'", this->getCode(), this->getName());
  LOG_SWITCH("  ", "ComputhermQThermostat_Switch", this);
}

void ComputhermQThermostat_Switch::update() {
    // This will be called every "update_interval" milliseconds.
    uint8_t msg = MSG_NONE;

    // Check if there is any message to be sent
    if (this->pending_msg_ != MSG_NONE) {
        // Send prioritized message
        msg = this->pending_msg_;
        this->pending_msg_ = MSG_NONE;
    } else if ( calculate_diff(millis(), this->last_msg_time_) > this->config_resend_interval_ ) {
        // Check if we have to resend current state by now
        msg = this->getState() ? MSG_HEAT_ON : MSG_HEAT_OFF;
    }

    // Do the actual sending
    if (msg != MSG_NONE) {
        this->send_msg(msg);

        // // if thermostat is ON, blink LED to show state X times
        // if ( calculate_diff(now, last_display_time_) > 10000 ) {
        //     if (this->state_) display_blink_led();
        //     last_display_time_ = now;
        // }
    }

    //TODO: later on move this to bridge to centralize the sending to one source - matters only if there are more than 1 switch
}

uint8_t ComputhermQThermostat_Switch::getPendingMessage() {
    return this->pending_msg_;
}

void ComputhermQThermostat_Switch::setPendingMessage(uint8_t new_message) {
    this->pending_msg_ = new_message;
}

void ComputhermQThermostat_Switch::send_msg(uint8_t msg) {
    if (msg == MSG_NONE) return;

    ESP_LOGD(TAG, "RF Sending message: 0x%02x for %s", msg, this->getCode());

    for (int i = 0; i < rf_repeat_count; i++) {
        switch (msg) {
        case MSG_HEAT_ON:
            // rfhandler_rf.sendMessage(this->getCode(), true);
            break;
        case MSG_HEAT_OFF:
            // rfhandler_rf.sendMessage(this->getCode(), false);
            break;
        case MSG_PAIR:
            // rfhandler_rf.pairAddress(this->getCode());
            break;
        }
    }

    this->last_msg_time_ = millis();
}

}
}