#include "esphome.h"

#include "computhermrf.h"
#include "computhermqthermostat_baseunit.h"
#include "computhermqthermostat_binarysensor.h"
#include "computhermqthermostat_switch.h"
#include "computhermqrf.h"

#include <vector>
#include <map>
using namespace std;

namespace esphome {
namespace computhermqrf {

static const char *TAG = "computhermqrf";

#ifdef USE_COMPUTHERMQRF_BINARY_SENSOR
const char *ComputhermQRF::ComputhermQ_ONOFF(bool state) { return state ? "ON" : "OFF"; }

bool ComputhermQRF::ComputhermQ_isONOFF(const char *state) { return (strcmp(state, "ON") == 0) ? 1 : 0; }
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
  return threshold > 0 && (elapsed(millis(), since) > threshold);
}
#endif

#ifdef USE_COMPUTHERMQRF_BINARY_SENSOR
void ComputhermQRF::addSensor(ComputhermQThermostat_BinarySensor *sensor) { sensors.push_back(sensor); }
#endif

#ifdef USE_COMPUTHERMQRF_SWITCH
void ComputhermQRF::addSwitch(ComputhermQThermostat_Switch *aswitch) { switches.push_back(aswitch); }

#ifdef USE_COMPUTHERMQRF_BUTTON_PAIR
void ComputhermQRF::setPairingButton(ComputhermQThermostat_PairingButton *button) {
  this->pairing_button = button;

  // add callback reference
  this->pairing_button->setParentCallback([this]() { this->on_pairing(); });
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
  rfhandler_rf = new ComputhermRF(receiver_pin_ ? receiver_pin_->get_pin() : 255,
                                  transmitter_pin_ ? transmitter_pin_->get_pin() : 255);
  rfhandler_rf->startReceiver();

#ifdef USE_API
#ifdef USE_COMPUTHERMQRF_SWITCH
  // Register the pairing service
  register_service(&ComputhermQRF::on_pairing, "computhermqrf_pair");
#endif
#endif
}

void ComputhermQRF::loop() {
  // check if sending iteration is needed
  rfhandler_rf->sending_loop();

  if (rfhandler_rf->isDataAvailable()) {
    computhermMessage msg = rfhandler_rf->getData();

    ComputhermQThermostat_BinarySensor *sensor = find_closest_by_id(msg.addr);

    // trigger esphome event value
    ComputhermQRFData data;
    data.addr = msg.addr;
    data.on = msg.on;
    if (sensor->getCode() == msg.addr) {
      data.name = sensor->getName().c_str();
      data.abbrev = sensor->getAbbreviation();
    }
    if (this->show_extra_debug_)
      ESP_LOGI(TAG, "Received ComputhermQRF Code - code{0x%05lX} msg{%d}", data.addr, data.on);
    else
      ESP_LOGD(TAG, "Received ComputhermQRF Code - code{0x%05lX} msg{%d}", data.addr, data.on);

    this->data_callback_.call(data);

    // update sensor value
    if (sensor) {
// find closest - if not found, display hamming distance 56789 => 567>79
#ifdef USE_COMPUTHERMQRF_BINARY_SENSOR
      if (sensor->getCode() == msg.addr) {
        sensor->publish_state(msg.on);
        if (this->show_extra_debug_)
          ESP_LOGI(TAG, "Message received - Registered - name{%s}, msg{%s}", sensor->getName().c_str(),
                   ComputhermQ_ONOFF(sensor->state));
        else
          ESP_LOGD(TAG, "Message received - Registered - name{%s}, msg{%s}", sensor->getName().c_str(),
                   ComputhermQ_ONOFF(sensor->state));
      } else
#endif
      {
        unsigned long closest_addr = sensor->getCode();

        // Compute decorated address / error string 0x56789 -> 0x56789< 0xA<678B<
        char erraddr_buf[12];  // max 5+5=10  chars including error display chars, +1 for initial separator, +1 zero
        uint8_t idx = 0;
        erraddr_buf[idx++] = ':';  // initial separator
        for (int k = 0; k < 5; k++) {
          uint8_t hb_receivd = (msg.addr >> (5 - 1 - k) * 4) & 0xF;
          uint8_t hb_closest = (closest_addr >> (5 - 1 - k) * 4) & 0xF;

          erraddr_buf[idx++] = hb_receivd < 10 ? ('0' + hb_receivd) : ('a' + (hb_receivd - 10));
          if (hb_receivd != hb_closest)
            erraddr_buf[idx++] = '<';
        }
        erraddr_buf[idx] = '\0';

        // adding abbreviation
        std::string erraddr = std::string(sensor->getAbbreviation()) + std::string(erraddr_buf);

        if (this->show_extra_debug_)
          ESP_LOGI(TAG, "Message received - Unregistered - addr{%lx (%s)}, cmd{%d}", msg.addr, erraddr.c_str(), msg.on);
        else
          ESP_LOGD(TAG, "Message received - Unregistered - addr{%lx (%s)}, cmd{%d}", msg.addr, erraddr.c_str(), msg.on);
      }
    }
  }
}

void ComputhermQRF::update() {
#ifdef USE_COMPUTHERMQRF_SWITCH
  for (ComputhermQThermostat_Switch *aswitch : switches) {
    // priorized message handling
    ComputhermRFMessage msg = aswitch->popPendingMessage();

    // forced update and watchdog
    if (msg == ComputhermRFMessage::none) {
      if (aswitch->state && has_elapsed(aswitch->getLastTurnOnTime(), aswitch->getTurnOnWatchdogInterval())) {
        ESP_LOGD(TAG, "Turn_on_watchdog triggered, turning off switch.");
        aswitch->write_state(false);
        msg = aswitch->popPendingMessage();  // as ComputhermQThermostat_Switch::write_state pushes a message based on
                                             // the state
      } else if (has_elapsed(aswitch->getLastMsgTime(), aswitch->getResendInterval())) {
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
ComputhermQThermostat_BinarySensor *ComputhermQRF::find_closest_by_id(unsigned long device_id) {
  ComputhermQThermostat_BinarySensor *closest = NULL;
  int closest_distance = 99;
  for (ComputhermQThermostat_BinarySensor *sensor : sensors) {
    if (sensor->getCode() == device_id) {
      closest = sensor;
      closest_distance = 0;
      break;
    } else {
      // check hamming distance
      unsigned long temp = sensor->getCode() ^ device_id;  // store the XOR output in result variable
      int distance = 0;
      while (temp > 0) {
        distance += temp & 1;  // storing the count of different bit
        temp >>= 1;            // right shift by 1
      }
      if (distance < closest_distance) {
        closest_distance = distance;
        closest = sensor;
      }
    }
  }
  return closest;
}
#endif

#ifdef USE_COMPUTHERMQRF_SWITCH
void ComputhermQRF::send_msg(unsigned long code, ComputhermRFMessage msg) {
  if (msg == ComputhermRFMessage::none)
    return;

  ESP_LOGD(TAG, "RF Sending message: 0x%02x for %lx", msg, code);

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
}

void ComputhermQRF::on_pairing() {
  ESP_LOGI(TAG, "Initiated pairing on all switches...");

  for (ComputhermQThermostat_Switch *aswitch : switches) {
    aswitch->setPendingMessage(ComputhermRFMessage::pairing);
    yield();
  }
}
#endif

#ifdef USE_SENSOR
void ComputhermQRF::publish_sensor_state_(sensor::Sensor *sensor, float value, bool change_only) {
  if (!sensor || (change_only && sensor->has_state() && sensor->state == value)) {
    return;
  }
  sensor->publish_state(value);
}
#endif

#ifdef USE_TEXT_SENSOR
void ComputhermQRF::publish_sensor_state_(text_sensor::TextSensor *sensor, const std::string &value) {
  if (!sensor || (sensor->has_state() && sensor->state == value)) {
    return;
  }
  sensor->publish_state(value);
}
#endif

}  // namespace computhermqrf
}  // namespace esphome