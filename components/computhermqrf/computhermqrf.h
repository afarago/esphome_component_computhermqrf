#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"

#include "computhermqrf_helper.h"
#include "computhermrf.h"
#include "computhermqthermostat_baseunit.h"
#include "computhermqthermostat_binarysensor.h"
#include "computhermqthermostat_switch.h"
#include "computhermqthermostat_pairingbutton.h"

#include <vector>
#include <map>
using namespace std;

namespace esphome {
namespace computhermqrf {

class ComputhermQRF : public PollingComponent 
#ifdef USE_API
                      , public api::CustomAPIDevice
#endif
{
  public:
    ComputhermQRF() { }
#ifdef USE_COMPUTHERMQRF_BINARY_SENSOR
    void addSensor(ComputhermQThermostat_BinarySensor *sensor);
#endif
#ifdef USE_COMPUTHERMQRF_SWITCH
    void addSwitch(ComputhermQThermostat_Switch *aswitch);
#endif
#ifdef USE_COMPUTHERMQRF_BUTTON_PAIR
    void setPairingButton(ComputhermQThermostat_PairingButton *button);
#endif

    void set_receiver_pin(InternalGPIOPin *pin) { this->receiver_pin_ = pin; }
    void set_transmitter_pin(InternalGPIOPin *pin) { this->transmitter_pin_ = pin; }
#ifdef USE_COMPUTHERMQRF_UNREGISTERED_ADDR_TEXT_SENSOR
    void set_last_unregistered_address_text_sensor(text_sensor::TextSensor *sensor) { this->last_unregistered_address_text_sensor_ = sensor; }
#endif

  protected:
    InternalGPIOPin *receiver_pin_;
    InternalGPIOPin *transmitter_pin_;
    ComputhermRF *rfhandler_rf;
#ifdef USE_COMPUTHERMQRF_UNREGISTERED_ADDR_TEXT_SENSOR
    std::deque<std::tuple<unsigned long, std::string>> unknown_unit_ids;
#endif

    void publish_sensor_state_(sensor::Sensor *sensor, float value, bool change_only = false);
    void publish_sensor_state_(text_sensor::TextSensor *sensor, const std::string &value);

  private:
    static const int rf_repeat_count = 4;
#ifdef USE_COMPUTHERMQRF_UNREGISTERED_ADDR_TEXT_SENSOR
    static const int max_unregistered_address_to_store = 10;
    text_sensor::TextSensor *last_unregistered_address_text_sensor_;
#endif

#ifdef USE_COMPUTHERMQRF_BINARY_SENSOR
    std::vector<ComputhermQThermostat_BinarySensor*> sensors;
#endif
#ifdef USE_COMPUTHERMQRF_SWITCH
    std::vector<ComputhermQThermostat_Switch*> switches;
#endif
#ifdef USE_COMPUTHERMQRF_BUTTON_PAIR
    ComputhermQThermostat_PairingButton *pairing_button;
#endif

    void dump_config() override;
    void update() override;
    void setup() override;
    void loop() override;
#ifdef USE_COMPUTHERMQRF_SWITCH
    void on_pairing();
#endif

#ifdef USE_COMPUTHERMQRF_BINARY_SENSOR
    ComputhermQThermostat_BinarySensor *find_closest_by_id(unsigned long device_id);
    bool ComputhermQ_isONOFF(const char *state);
    const char* ComputhermQ_ONOFF(bool state);
#endif
#ifdef USE_COMPUTHERMQRF_SWITCH
    void send_msg(unsigned long code, ComputhermRFMessage msg);
    unsigned long elapsed(unsigned long now, unsigned long last_update);
    bool has_elapsed(unsigned long since, unsigned long threshold);
#endif
};

}
}