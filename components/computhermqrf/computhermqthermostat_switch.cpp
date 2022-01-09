#include "esphome.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/switch/switch.h"

#include "computhermqthermostat_switch.h"

namespace esphome {
namespace computhermqrf {

static const char* TAG = "computhermqrf.switch";

void ComputhermQThermostat_Switch::setState(bool state) {
    ComputhermQThermostat_BinarySensorBase::setState(state);
    this->publish_state(state);
}

void ComputhermQThermostat_Switch::write_state(bool state) {
    // This will be called every time the user requests a state change.
    this->setState(state);
    this->setPendingMessage(state);
}

void ComputhermQThermostat_Switch::dump_config() {
  ESP_LOGCONFIG(TAG, "ComputhermQThermostat_Switch:");
  ESP_LOGCONFIG(TAG, "  code:'%s' name: '%s'", this->getCode(), this->getName());
  LOG_SWITCH("  ", "ComputhermQThermostat_Switch", this);
}

bool ComputhermQThermostat_Switch::popPendingMessage(/*OUT*/ bool &message) {
    if (this->has_pending_msg_) {
        message = this->pending_msg_;
        this->has_pending_msg_ = false;
        return true;
    } else {
        return false;
    }
}

void ComputhermQThermostat_Switch::setPendingMessage(bool new_message) {
    this->pending_msg_ = new_message;
    this->has_pending_msg_ = true;
}

}
}