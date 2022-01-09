# ComputhermQRF component for ESPHome

## About
This external component provides a way retrieve and control Computherm Q series thermostats. The RF receiver allows up to four individual devices. Existing devices can either be monitored as sensors or via manual pairing (in progress) you can add virtual zones that control the boiler separately.
[Computherm Q8](https://computherm.info/en/digital_thermostats/computherm_q8rf)

The communication with the RF receiver is accomplised via 868.35 MHz in a custom protocol originally decoded by @dexhun in his [Arduino library](https://github.com/denxhun/ComputhermRF).
Inspiration and hardware design came from flogi in his [blog](https://flogi-diyiot.blog.hu/2021/10/13/rf868mhz_wifi_gateway_esp8266_rfm217w_rfm119w_computherm_q8rf) 

## Configuration
Communication with the device is done through a receiver and an transmitter module. They are not connected, so theoretically sensor and switch module works independently. 


The communication with the hardware is done using UART. Therefore you need to have an [UART bus](https://esphome.io/components/uart.html#uart) in your configuration with the `rx_pin` connected to the output of your hardware sensor component. The baud rate usually has to be set to 9600bps.

```yaml
# Example configuration entry

external_components:
  - source: github://afarago/esphome_component_computhermqrf
    components: [ computhermqrf ]

computhermqrf:
  receiver_pin: D1
  transmitter_pin: D2

binary_sensor:
  - platform: computhermqrf
    name: "${systemName} zone 1"
    code: 0x12347
  - platform: computhermqrf
    name: "${systemName} zone 2"
    code: 0x1234B

switch:
  - platform: computhermqrf
    name: "${systemName} zone 3"
    code: 0x12343
```

## Configuration variables

### ComputhermQRF platform:
- **receiver_pin** (*Optional*): Specify the receiver pin.
- **transmitter_pin** (*Optional*): Specify the transmitter pin.

### Binary Sensor
- **code** (*Required*, string): Specify the 5 digit hex code associated with the zone.
- **name** (*Optional*, string): Specify the zone name.
- All other options from [Binary Sensor](https://esphome.io/components/binary_sensor/index.html#config-binary_sensor).

### Switch
- **code** (*Required*, string): Specify the 5 digit hex code associated with the zone.
- **name** (*Optional*, string): Specify the zone name.
- All other options from [Binary Sensor](https://esphome.io/components/switch/index.html#config-switch).

## Hardware
You need the following list of hardware
1. [ESP 8266 Wemos D1 mini](https://www.hestore.hu/prod_10037901.html)
2. [RF Receiver: HopeRF RFM217W-868S1](https://en.maritex.com.pl/product/show/52755.html)
3. [RF Transmitter: HopeRF RFM117W-868S1](https://www.chipcad.hu/hu/product/rf-products-hoperf-askook-module/rfm117w-868s1--HRF146)
4. (optional) DHT22 Digital Temperature and Humidity Temperature Sensor

Connections:

    D1_MINI    RFM117W    RFM217W    DHT22       10K_Ohm_pullup_resistor
    =========================================================
    3.3V       VCC        VCC        VCC(pin1)   PIN2
    GND        GND        GND        GND(pin4)   -
    D1         -          DATA       -            -
    D2         DATA       -          -            -
    D3         -          -          DATA(pin2)  PIN1

![module connections](./doc/d1computherm_wiring.jpg "module connections")

## Obtaining Computherm QRF Codes
Computherm QRF Codes are 5 half byte codes, represented as hex code.
The codes are randomly generated and assigned upon a pairing process. Without configuring any binary_sensors you still see all ongoing traffic n the debug log. You check which is your zone code before setting up a binary_sensor.

```
[D][computhermqrf:069]: Message received - Unregistered - thermostat: ABCD7, command: OFF
```

For virtual zones used for switches you can come up with any arbitrary code.
It seens that the first 4 bytes are arbitrary and the last half byte is
* zone 1 --> 0111
* zone 2 --> 1011
* zone 3 --> 0011
* zone 4 --> 1101 (have not checked)