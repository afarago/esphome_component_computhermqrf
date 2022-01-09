#include "esphome.h"

#include "computhermrf.h"
#include "computhermqthermostat_binarysensorbase.h"
#include "computhermqthermostat_binarysensor.h"
#include "computhermqthermostat_switch.h"
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

void ComputhermQRF::addSensor(ComputhermQThermostat_BinarySensor *sensor) {
    sensors.push_back(sensor);
}

void ComputhermQRF::addSwitch(ComputhermQThermostat_Switch *aswitch) {
    switches.push_back(aswitch);
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
    ESP_LOGD(TAG, "receiver_pin_: %d, transmitter_pin_: %d",
        receiver_pin_ ? receiver_pin_->get_pin() : 255, 
        transmitter_pin_ ? transmitter_pin_->get_pin() : 255);
    rfhandler_rf->startReceiver();
}

void ComputhermQRF::loop() {
    if (rfhandler_rf->isDataAvailable()) {
        computhermMessage msg = rfhandler_rf->getData();

        ComputhermQThermostat_BinarySensor* sensor = findbyid(msg.address.c_str());
        if (sensor) {
            sensor->setState(ComputhermQ_isONOFF(msg.command.c_str()));
            ESP_LOGD(TAG, "Message received - Registered - description: %s, state: %s", sensor->getName(), ComputhermQ_ONOFF(sensor->getState()));
        } else {
            ESP_LOGD(TAG, "Message received - Unregistered - thermostat: %s, command: %s", msg.address.c_str(), msg.command.c_str());
        }
    }
}

void ComputhermQRF::update() {
    bool force_send_all = ( calculate_diff(millis(), this->last_msg_time_) > this->config_resend_interval_ );

    for(ComputhermQThermostat_Switch *aswitch : switches) {
        bool msg = false;
        bool do_send = false;
        if (aswitch->popPendingMessage(msg)) {
            // clears pending message, indicate that sending is due
            do_send = true;
        } else {
            if (force_send_all) { 
                do_send = true;
                msg = aswitch->getState(); 
            }
        }
        
        if (do_send) { 
            this->send_msg(aswitch->getCode(), msg ? Message::heat_on : Message::heat_off);
            last_msg_time_ = millis();
        }
    }
}



ComputhermQThermostat_BinarySensor *ComputhermQRF::findbyid(const char* device_sid) {
    for(ComputhermQThermostat_BinarySensor *sensor : sensors) {
        if (strcmp(sensor->getCode(), device_sid) == 0) {
            return sensor;
        }
    }
    return NULL;
}

void ComputhermQRF::send_msg(const char* code, Message msg) {
    if (msg == Message::none) return;

    ESP_LOGD(TAG, "RF Sending message: 0x%02x for %s", msg, code);

    for (int i = 0; i < rf_repeat_count; i++) {
        switch (msg) {
        case Message::heat_on:
        case Message::heat_off:
            rfhandler_rf->sendMessage(code, msg == Message::heat_on);
            break;
        case Message::pair:
            rfhandler_rf->pairAddress(code);
            break;
        default:
            break;
        }
    }
}

unsigned long ComputhermQRF::calculate_diff(long now, long last_update) {  
  unsigned long diff = 0UL;

  if (last_update > now) {
    // millis() overflows every ~50 days
    diff = (ULONG_MAX - last_update) + now;
  } else {
    diff = now - last_update;
  }
  return diff;
}

}
}