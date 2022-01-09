#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/switch/switch.h"

#include "computhermqthermostat_binarysensorbase.h"

namespace esphome {
namespace computhermqrf {

#ifdef USE_COMPUTHERMQRF_SWITCH

class ComputhermQThermostat_Switch : public switch_::Switch, public Component, 
                                     public ComputhermQThermostat_BinarySensorBase {
  public:
    void setResendInterval(uint32_t interval) { this->config_resend_interval_ = interval; }
    uint32_t getResendInterval() { return this->config_resend_interval_; }
    void setTurnOnWatchdogInterval(uint32_t interval) { this->config_turn_on_watchdog_interval_ = interval; }
    uint32_t getTurnOnWatchdogInterval() { return this->config_turn_on_watchdog_interval_; }

    void dump_config() override;
    void setState(bool state) override;
    void write_state(bool state) override;

    ComputhermRFMessage popPendingMessage();
    void setPendingMessage(ComputhermRFMessage new_message);
    ComputhermRFMessage getMessageBasedOnState();

    unsigned long getLastTurnOnTime() { return this->last_turn_on_time_; }
    unsigned long getLastMsgTime() { return this->last_msg_time_; }
    void setLastMsgTime(unsigned long value) { this->last_msg_time_ = value; }

  private:
    uint32_t config_resend_interval_ = 60000L; // driven by esphome config - 60 sec
    uint32_t config_turn_on_watchdog_interval_ = 1800000L; //driven by esphome config - 30 min = 30*60*1000;

    ComputhermRFMessage pending_msg_ = ComputhermRFMessage::none;
    unsigned long last_turn_on_time_ = 0;
    unsigned long last_msg_time_ = 0;
};

#endif

}
}