// Copyright 2019 denxhun

#include "computhermrf.h"

const uint16_t ComputhermRF::_TICK_LENGTH = 220;
const uint16_t ComputhermRF::_SHORT_MIN = _TICK_LENGTH * 0.5;  // 110
const uint16_t ComputhermRF::_LONG_MIN = _TICK_LENGTH * 1.5;   // 330
const uint16_t ComputhermRF::_SYNC_MIN = _TICK_LENGTH * 2.5;   // 550
const uint16_t ComputhermRF::_SYNC_MAX = _TICK_LENGTH * 3.5;   // 770
const uint16_t ComputhermRF::_STOP_MIN = _TICK_LENGTH * 7;     // 1760
const uint16_t ComputhermRF::_STOP_MAX = _TICK_LENGTH * 10;    // 2200
const uint16_t ComputhermRF::_BUFF_SIZE = 70;
const uint16_t ComputhermRF::_MSG_LENGTH = 56;

bool ComputhermRF::_isReceiving = false;
uint8_t ComputhermRF::_inputPin;
uint8_t ComputhermRF::_outputPin;
volatile bool ComputhermRF::_avail;
volatile byte ComputhermRF::_buff[_BUFF_SIZE];
volatile byte ComputhermRF::_buffEnd;
byte ComputhermRF::_lastBuff[_BUFF_SIZE];
uint32_t ComputhermRF::_lastMessageArrived;

ComputhermRF::ComputhermRF() { ComputhermRF(255, 255); }

ComputhermRF::ComputhermRF(uint8_t inputPin, uint8_t outputPin) { setPins(inputPin, outputPin); }

void ComputhermRF::setPins(uint8_t inputPin, uint8_t outputPin) {
  stopReceiver();
  _inputPin = inputPin;
  _outputPin = outputPin;
  if (_outputPin < 255) {
    pinMode(_outputPin, OUTPUT);
  }
}

void ComputhermRF::startReceiver() {
  if (_inputPin < 255) {
    pinMode(_inputPin, INPUT);
    _avail = false;
    _buffEnd = 0;
    _lastMessageArrived = 0;
    attachInterrupt(digitalPinToInterrupt(_inputPin), _handler, CHANGE);
    _isReceiving = true;
  }
}
void ComputhermRF::stopReceiver() {
  detachInterrupt(digitalPinToInterrupt(_inputPin));
  _isReceiving = false;
}
bool ComputhermRF::isDataAvailable() { return _avail; }
// void ComputhermRF::getData(String &id, bool &on) {
//     computhermMessage result = getData();
//     id = result.address;
//     on = (result.command == "ON");
// }
computhermMessage ComputhermRF::getData() {
  computhermMessage result;

  result.addr = 0;
  // String a = "     ";
  uint8_t n = 0;
  for (int i = 0; i < 5; i++) {
    if (i)
      result.addr <<= 4;
    for (int j = 0; j < 4; j++) {
      for (int j = 0; j < 4; j++) {
        if (_buff[i * 4 + j] == 1) {
          n |= 1 << (3 - j);
        }
      }
      // a[i] = _toHex(n);
      result.addr |= n & 0xF;
      n = 0;
    }
  }
  // result.address = a;
  // String c = "";
  if (_buff[20] == 0 && _buff[21] == 0 && _buff[22] == 0 && _buff[23] == 0) {
    // c = "ON";
    result.on = true;
  } else {
    // c = "OFF";
    result.on = false;
  }
  // result.command = c;
  _avail = false;
  return result;
}

void ComputhermRF::sendMessage(unsigned long address, bool on) { _sendMessage(address, on, true); }

computhermMessage sending_message;
int sending_remaining_counter = 0;
void ComputhermRF::sending_loop() {
  if (sending_remaining_counter > 0)
    _sendMessage_iteration();
}
void ComputhermRF::_sendMessage(unsigned long address, bool on, bool normal_padding) {
  _wakeUpTransmitter();
  stopReceiver();
  sending_remaining_counter = 16;
  sending_message = {.addr = address, .on = on, .normal_padding = normal_padding};
  _sendMessage_iteration();
}

void ComputhermRF::_sendMessage_iteration() {
  // _sendMessage timing
  // bit = pulse_w = 3*tick / halfbyte = 4* bit = 12*tick / 5*halfbyte = 60*tick
  // [j]repeat - 2x => 120 tick + sync (12 tick) = 132 tick
  // [i] repeat 8 times - 1056 ticks
  // 1 cycle = 232 ms

  _sendSync();
  for (int j = 0; j < 2; j++) {
    // 16+8 bits
    // CHECK if address last halfbyte is 8?
    for (int k = 0; k < 5; k++) {
      // 0x56789 -> 5,6,7,8,9
      byte hb = (sending_message.addr >> (5 - 1 - k) * 4) & 0xF;
      // _sendHalfByte(address[k]);
      _sendHalfByte(hb);
    }

    // COMMAND: 4+4 bits
    //(CClib: TURN_ON_HEATING = 0xFF) --> "00" to send
    //(CClib:TURN_OFF_HEATING = 0x0F) --> "F0" to send
    //(CClib:PAIR = 0x00) --> "FF" to send
    if (sending_message.on) {
      //_sendHalfByte('0');  // ON
      _sendHalfByte(0x0);  // ON
    } else {
      // _sendHalfByte('F');  // OFF
      _sendHalfByte(0xF);  // OFF
    }
    if (sending_message.normal_padding) {
      // _sendHalfByte('0');  // default padding for ON/OFF - ON
      _sendHalfByte(0x0);  // default padding for ON/OFF - ON
    } else {
      // _sendHalfByte('F');  // padding for pairing - OFF
      _sendHalfByte(0xF);  // padding for pairing - OFF
    }
  }
  _sendStop();

  if (--sending_remaining_counter <= 0) {
    startReceiver();
  }
}
void ComputhermRF::pairAddress(unsigned long address) {
  // send FF for pairing
  _sendMessage(address, false, false);
}

/*
In order to make the receiver recognize the transmitter, we need to execute the pairing process.

Go to Home Assistant's Developer tools → Services and select the service esphome.<node_name>_<switch_name>.pair. Press
and hold the M/A button on the receiver until it starts flashing green. Now press Call service in the Services page. The
receiver should stop flashing, and the pairing is now complete. The receiver should react now if you try toggling the
associated Home Assistant UI switch.

If you wish to reset and use your original wireless thermostat, once again set the receiver into learning mode with the
M/A button, then hold the SET + DAY button on your wireless thermostat until the blinking stops. The receiver only
listens to the device currently paired.
*/

void ComputhermRF::_wakeUpTransmitter() {
  digitalWrite(_outputPin, HIGH);
  delayMicroseconds(100);
  digitalWrite(_outputPin, LOW);
}
void ComputhermRF::_sendPulse(uint8_t lowTime, uint8_t highTime) {
  digitalWrite(_outputPin, LOW);
  delayMicroseconds(lowTime * _TICK_LENGTH);
  if (highTime > 0) {
    digitalWrite(_outputPin, HIGH);
    delayMicroseconds(highTime * _TICK_LENGTH);
    digitalWrite(_outputPin, LOW);
  }
}
void ComputhermRF::_sendStop() {
  _sendPulse(6, 0);  // 000 000
}
void ComputhermRF::_sendSync() {
  _sendPulse(3, 3);  // 000 111
  _sendPulse(3, 3);  // 000 111 //??
}
void ComputhermRF::_sendBit(bool bit) {
  if (bit) {
    _sendPulse(1, 2);  // 011
  } else {
    _sendPulse(2, 1);  // 001
  }
}

void ComputhermRF::_sendHalfByte(byte num) {
  for (int i = 0; i < 4; i++) {
    _sendBit(num & 1 << (3 - i));
  }
}

bool ComputhermRF::_isRepeat() {
  bool result = false;
  for (int i = 0; i < _buffEnd; i++) {
    if (_buff[i] != _lastBuff[i]) {
      for (int j = 0; j < _buffEnd; j++) {
        _lastBuff[j] = _buff[j];
      }
      _lastMessageArrived = millis();
      return false;
    }
  }
  result = (millis() - _lastMessageArrived < 2000);
  _lastMessageArrived = millis();
  return result;
}

void ICACHE_RAM_ATTR ComputhermRF::_handler() {
  static uint32_t lastMs = 0, currMs, diffMs;
  currMs = micros();
  diffMs = currMs - lastMs;
  lastMs = currMs;
  if (_buffEnd == _BUFF_SIZE) {
    _buffEnd = 0;
  }
  if (!_avail) {
    if (digitalRead(_inputPin) == LOW) {  // Falling edge
      if (diffMs >= _SHORT_MIN && diffMs <= _SYNC_MAX) {
        if (diffMs >= _SYNC_MIN) {
          _buffEnd = 0;
        } else {
          if (diffMs <= _LONG_MIN) {
            _buff[_buffEnd++] = 0;
          } else {
            if (diffMs < _SYNC_MIN) {
              _buff[_buffEnd++] = 1;
            }
          }
        }
      }
    } else {  // Raising edge, only stop could be detected
      if (diffMs >= _STOP_MIN) {
        if (_buffEnd == _MSG_LENGTH && !_isRepeat()) {
          _avail = true;
        } else {
          _buffEnd = 0;
        }
      }
    }
  }
}
