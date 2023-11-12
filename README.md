# Loxone Component

# Core Yaml
```yaml
external_components:
  - source:
      type: git
      url: https://github.com/hzkincony/esphome-loxone
      ref: v1.2.1

i2c:
   - id: bus_a
     sda: 4
     scl: 15
     scan: true
     frequency: 400kHz

sensor:
  - platform: ina226
    address: 0x40
    i2c_id: bus_a
    shunt_resistance: 0.1 ohm
    current:
      name: "INA226 Current-1"
      id: current_1
    power:
      name: "INA226 Power-1"
    bus_voltage:
      name: "INA226 Bus Voltage-1"
      id: voltage_1
    shunt_voltage:
      name: "INA226 Shunt Voltage-1"
    max_current: 5A
    update_interval: 5s
  - platform: ina226
    address: 0x41
    i2c_id: bus_a
    shunt_resistance: 0.01 ohm
    current:
      name: "INA226 Current-2"
      id: current_2
    power:
      name: "INA226 Power-2"
    bus_voltage:
      name: "INA226 Bus Voltage-2"
      id: voltage_2
    shunt_voltage:
      name: "INA226 Shunt Voltage-2"
    max_current: 5A
    update_interval: 5s

switch:
  - platform: gpio
    pin: 22
    name: "Loxone Switch 1"
    id: loxone_switch_1
    on_turn_on:
      - lambda: !lambda |-
          id(loxone1).send_string_data("RELAY-SET-255,1,1,OK");
    on_turn_off:
      - lambda: !lambda |-
          id(loxone1).send_string_data("RELAY-SET-255,1,0,OK");

binary_sensor:
  - platform: gpio
    pin: 23
    publish_initial_state: true
    name: "Loxone Binary Sensor 1"
    on_press:
      - lambda: !lambda |-
          id(loxone1).send_string_data("RELAY-GET_INPUT-255,1,1,OK");
    on_release:
      - lambda: !lambda |-
          id(loxone1).send_string_data("RELAY-GET_INPUT-255,1,0,OK");

loxone:
  id: loxone1
  protocol: udp
  loxone_ip: "192.168.50.124"
  loxone_port: 9999
  listen_port: 8888 # esp32 will listen on this port
  delimiter: "\n"
  send_buffer_length: 64
  on_string_data:
    - logger.log:
        format: "on_string_data, data=%s"
        args: [ data.c_str() ]
    - lambda: !lambda |-
        if (data == "RELAY-SET-255,1,1") {
          id(loxone_switch_1).turn_on();
        } else if (data == "RELAY-SET-255,1,0") {
          id(loxone_switch_1).turn_off();
        }

interval:
  - interval: 1min
    then:
      - lambda: !lambda |-
          auto current = value_accuracy_to_string(id(current_1).state, 2);
          auto voltage = value_accuracy_to_string(id(voltage_1).state, 2);
          id(loxone1).send_string_data("RELAY-GET_energy-255,1," + current + "," + voltage + ",OK");
          delay(10);
          current = value_accuracy_to_string(id(current_2).state, 2);
          voltage = value_accuracy_to_string(id(voltage_2).state, 2);
          id(loxone1).send_string_data("RELAY-GET_energy-255,2," + current + "," + voltage + ",OK");


```