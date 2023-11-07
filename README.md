# Loxone Component

# Install
1. Download the latest version of the zip from https://github.com/hzkincony/esphome-loxone/releases
2. Extract loxone.zip to the custom_components folder in your esphome configuration dir.

# Core Yaml
```yaml
loxone:
  id: loxone1
  loxone_ip: "192.168.50.124"
  loxone_port: 9999
  listen_port: 8888
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
```