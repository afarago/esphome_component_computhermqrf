#include "esphome.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"

#ifdef USE_COMPUTHERMQRF_SWITCH

#include "esphome/components/switch/switch.h"
#include "computhermqrf_helper.h"
#include "computhermqthermostat_switch.h"

namespace esphome {
namespace computhermqrf {

static const char* TAG = "computhermqrf.switch";

void ComputhermQThermostat_Switch::write_state(bool state) {
    // This will be called every time the user requests a state change.
    this->setPendingMessage(this->getMessageBasedOnState());
    this->publish_state(state);

    if (state) {
      this->last_turn_on_time_ = millis();
    }
}

void ComputhermQThermostat_Switch::dump_config() {
  ESP_LOGCONFIG(TAG, "ComputhermQThermostat_Switch:");
  ESP_LOGCONFIG(TAG, "  Code:'%s' Name: '%s'", this->getCode(), this->getName());
  ESP_LOGCONFIG(TAG, "  Resend interval: %d ms", this->config_resend_interval_);
  ESP_LOGCONFIG(TAG, "  Turn on watchdog interval: %d ms", this->config_turn_on_watchdog_interval_);
  LOG_SWITCH("  ", "ComputhermQThermostat_Switch", this);
}

ComputhermRFMessage ComputhermQThermostat_Switch::popPendingMessage() {
    ComputhermRFMessage msg = this->pending_msg_;    
    this->pending_msg_ = ComputhermRFMessage::none;
    return msg;
}

void ComputhermQThermostat_Switch::setPendingMessage(ComputhermRFMessage new_message) {
    this->pending_msg_ = new_message;
}

ComputhermRFMessage ComputhermQThermostat_Switch::getMessageBasedOnState() {
    return this->state ?  
            ComputhermRFMessage::heat_on : 
            ComputhermRFMessage::heat_off;
}

}
}

#endif
