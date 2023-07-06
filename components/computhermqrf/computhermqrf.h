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

namespace esphome {
namespace computhermqrf {

using namespace std;

struct ComputhermQRFData {
  unsigned long addr;
  bool on;
  const char* name;
  const char* abbrev;
};

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
    void set_show_extra_debug(bool value) { this->show_extra_debug_ = value; }
#ifdef USE_COMPUTHERMQRF_UNREGISTERED_ADDR_TEXT_SENSOR
    void set_last_unregistered_address_text_sensor(text_sensor::TextSensor *sensor) { this->last_unregistered_address_text_sensor_ = sensor; }
#endif

  void add_on_code_received_callback(std::function<void(ComputhermQRFData)> callback) {
    this->data_callback_.add(std::move(callback));
  }

  protected:
    InternalGPIOPin *receiver_pin_;
    InternalGPIOPin *transmitter_pin_;
    ComputhermRF *rfhandler_rf;
    bool show_extra_debug_ = false;
    CallbackManager<void(ComputhermQRFData)> data_callback_;
#ifdef USE_COMPUTHERMQRF_UNREGISTERED_ADDR_TEXT_SENSOR
    std::deque<std::tuple<unsigned long, std::string>> unknown_unit_ids;
#endif

#ifdef USE_SENSOR
    void publish_sensor_state_(sensor::Sensor *sensor, float value, bool change_only = false);
#endif
#ifdef USE_TEXT_SENSOR
    void publish_sensor_state_(text_sensor::TextSensor *sensor, const std::string &value);
#endif

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

class ComputhermQRFReceivedCodeTrigger : public Trigger<ComputhermQRFData> {
 public:
  explicit ComputhermQRFReceivedCodeTrigger(ComputhermQRF *parent) {
    parent->add_on_code_received_callback([this](ComputhermQRFData data) { this->trigger(data); });
  }
};

}
}