#include "esphome.h"

#include "computhermrf.h"
#include "computhermqthermostat_binarysensorbase.h"
#include "computhermqrf.h"

#include <vector>
using namespace std;

namespace esphome {
namespace computhermqrf {

static const char* TAG = "computhermqrf";

const char* ComputhermQRF::ComputhermQ_ONOFF(bool state) {
  return state ? "ON" : "OFF";
}

bool ComputhermQRF::ComputhermQ_isONOFF(const char *state) {
  return (strcmp(state,"ON")==0) ? 1 : 0;
}

// static unsigned long calculate_diff(long now, long last_update) {  
//   unsigned long diff = 0UL;

//   if (last_update > now) {
//     // millis() overflows every ~50 days
//     diff = (ULONG_MAX - last_update) + now;
//   } else {
//     diff = now - last_update;
//   }
//   return diff;
// }

void ComputhermQRF::addZone(ComputhermQThermostat_BinarySensorBase* sensor) {
    sensors.push_back(sensor);
}

void ComputhermQRF::set_receiver_pin(InternalGPIOPin *receiver_pin) { 
    receiver_pin_ = receiver_pin;
}
void ComputhermQRF::set_transmitter_pin(InternalGPIOPin *transmitter_pin) { 
    transmitter_pin_ = transmitter_pin;
}

void ComputhermQRF::dump_config() {
  ESP_LOGCONFIG(TAG, "ComputhermQRF:");
  LOG_PIN("  Receiver Pin: ", this->receiver_pin_);
  LOG_PIN("  Transmitter Pin: ", this->transmitter_pin_);
}

void ComputhermQRF::setup() {
    // if not configured, use 255
    rfhandler_rf = new ComputhermRF(
        receiver_pin_ ? receiver_pin_->get_pin() : 255, 
        transmitter_pin_ ? transmitter_pin_->get_pin() : 255);
    rfhandler_rf->startReceiver();
}

void ComputhermQRF::loop() {
    if (rfhandler_rf->isDataAvailable()) {
        computhermMessage msg = rfhandler_rf->getData();

        ComputhermQThermostat_BinarySensorBase* sensor = findbyid(msg.address.c_str());
        if (sensor) {
            sensor->setState(ComputhermQ_isONOFF(msg.command.c_str()));
            ESP_LOGD(TAG, "Message received - Registered - description: %s, state: %s", sensor->getName(), ComputhermQ_ONOFF(sensor->getState()));
        } else {
            ESP_LOGD(TAG, "Message received - Unregistered - thermostat: %s, command: %s", msg.address.c_str(), msg.command.c_str());
        }
    }
}

ComputhermQThermostat_BinarySensorBase *ComputhermQRF::findbyid(const char* device_sid) {
    for(ComputhermQThermostat_BinarySensorBase *sensor : sensors) {
        if (strcmp(sensor->getCode(), device_sid) == 0) {
            return sensor;
        }
    }
    return NULL;
}

}
}