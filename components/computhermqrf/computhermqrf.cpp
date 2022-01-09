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

#ifdef USE_COMPUTHERMQRF_BINARY_SENSOR
const char* ComputhermQRF::ComputhermQ_ONOFF(bool state) {
  return state ? "ON" : "OFF";
}

bool ComputhermQRF::ComputhermQ_isONOFF(const char *state) {
  return (strcmp(state,"ON")==0) ? 1 : 0;
}
#endif

#ifdef USE_COMPUTHERMQRF_SWITCH
unsigned long ComputhermQRF::elapsed(unsigned long now, unsigned long last_update) {  
  unsigned long diff = 0UL;

  if (last_update > now) {
    // millis() overflows every ~50 days
    diff = (ULONG_MAX - last_update) + now;
  } else {
    diff = now - last_update;
  }
  return diff;
}

bool ComputhermQRF::has_elapsed(unsigned long since, unsigned long threshold) {
    return 
        threshold>0 &&
        ( elapsed(millis(), since) > threshold );
}
#endif

#ifdef USE_COMPUTHERMQRF_BINARY_SENSOR
void ComputhermQRF::addSensor(ComputhermQThermostat_BinarySensor *sensor) {
    sensors.push_back(sensor);
}
#endif

#ifdef USE_COMPUTHERMQRF_SWITCH
void ComputhermQRF::addSwitch(ComputhermQThermostat_Switch *aswitch) {
    switches.push_back(aswitch);
}

#ifdef USE_COMPUTHERMQRF_BUTTON_PAIR
void ComputhermQRF::setPairingButton(ComputhermQThermostat_PairingButton *button) {
    this->pairing_button = button;

    // add callback reference
    this->pairing_button->setParentCallback([this](){
        this->debug_id++;
        this->on_pairing(); 
    });
}
#endif
#endif

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

    #ifdef USE_API
    #ifdef USE_COMPUTHERMQRF_SWITCH
    ESP_LOGD(TAG, "ComputhermQRF::setup regservice");
    // Register the pairing service
    register_service(&ComputhermQRF::on_pairing, "computhermqrf_pair");
    #endif
    #endif
}

void ComputhermQRF::loop() {
    #ifdef USE_COMPUTHERMQRF_BINARY_SENSOR
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
    #endif
}

void ComputhermQRF::update() {
    #ifdef USE_COMPUTHERMQRF_SWITCH
    for(ComputhermQThermostat_Switch *aswitch : switches) {
        
        // priorized message handling
        ComputhermRFMessage msg = aswitch->popPendingMessage();
        
        // forced update and watchdog
        if (msg == ComputhermRFMessage::none) {

            if (aswitch->getState() && has_elapsed(aswitch->getLastTurnOnTime(), aswitch->getTurnOnWatchdogInterval())) {
                ESP_LOGD(TAG, "Turn on watchdog triggered, turning off switch.");
                aswitch->write_state(false);
                msg = aswitch->popPendingMessage();
            } 
            else if (has_elapsed(aswitch->getLastMsgTime(), aswitch->getResendInterval())) { 
                msg = aswitch->getMessageBasedOnState();
            }
        }        

        if (msg != ComputhermRFMessage::none) { 
            this->send_msg(aswitch->getCode(), msg);
            aswitch->setLastMsgTime(millis());
        }
    }
    #endif
}

#ifdef USE_COMPUTHERMQRF_BINARY_SENSOR
ComputhermQThermostat_BinarySensor *ComputhermQRF::findbyid(const char* device_sid) {
    for(ComputhermQThermostat_BinarySensor *sensor : sensors) {
        if (strcmp(sensor->getCode(), device_sid) == 0) {
            return sensor;
        }
    }
    return NULL;
}
#endif

#ifdef USE_COMPUTHERMQRF_SWITCH
void ComputhermQRF::send_msg(const char* code, ComputhermRFMessage msg) {
    if (msg == ComputhermRFMessage::none) return;

    ESP_LOGD(TAG, "RF Sending message: 0x%02x for %s", msg, code);

    for (int i = 0; i < rf_repeat_count; i++) {
        switch (msg) {
        case ComputhermRFMessage::heat_on:
        case ComputhermRFMessage::heat_off:
            rfhandler_rf->sendMessage(code, msg == ComputhermRFMessage::heat_on);
            break;
        case ComputhermRFMessage::pairing:
            rfhandler_rf->pairAddress(code);
            break;
        default:
            break;
        }
        yield();        
    }
}

void ComputhermQRF::on_pairing() {
    ESP_LOGI(TAG, "Start pairing all switches...");
    
    for(ComputhermQThermostat_Switch *aswitch : switches) {
        aswitch->setPendingMessage(ComputhermRFMessage::pairing);
        yield();
    }
}
#endif

}
}