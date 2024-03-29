#include "esphome.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"

#ifdef USE_COMPUTHERMQRF_BUTTON_PAIR
#ifdef USE_COMPUTHERMQRF_SWITCH

#include "computhermqrf_helper.h"
#include "computhermqthermostat_pairingbutton.h"

namespace esphome {
namespace computhermqrf {

static const char *TAG = "computhermqrf.button";

void ComputhermQThermostat_PairingButton::press_action() {
  if (parent_callback) {
    parent_callback();
  }
}

void ComputhermQThermostat_PairingButton::dump_config() { LOG_BUTTON(TAG, "Pairing button", this); }

}  // namespace computhermqrf
}  // namespace esphome

#endif
#endif
