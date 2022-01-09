#pragma once

#include "computhermqthermostat_binarysensorbase.h"

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace computhermqrf {

class ComputhermQThermostat_Switch : public switch_::Switch, public Component, public ComputhermQThermostat_BinarySensorBase {
  public:
    enum Mode { normal, pairing };

    void setPairingMode() { mode_ = Mode::pairing; }
    bool getPairingMode() { return mode_ == Mode::pairing; }
    void clearPairingMode() { mode_ = Mode::normal; }

    void dump_config() override;
    void setState(bool state) override;
    void write_state(bool state) override;

    bool popPendingMessage(/*OUT*/ bool &message);
    void setPendingMessage(bool new_message);

  private:
    bool has_pending_msg_ = false;
    bool pending_msg_ = false;
    Mode mode_ = Mode::normal;
};

}
}