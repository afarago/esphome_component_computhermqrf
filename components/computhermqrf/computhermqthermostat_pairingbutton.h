#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"

#ifdef USE_COMPUTHERMQRF_BUTTON_PAIR
#ifdef USE_COMPUTHERMQRF_SWITCH

#include "esphome/components/button/button.h"

namespace esphome
{
  namespace computhermqrf
  {

    class ComputhermQThermostat_PairingButton : public button::Button, public Component
    {
    public:
      void dump_config() override;
      void setParentCallback(std::function<void()> callback) { this->parent_callback = callback; }

    protected:
      void press_action() override;

    private:
      std::function<void(void)> parent_callback;
    };

  }
}

#endif
#endif
