# Loxone Component

# Core Yaml
```yaml
external_components:
  - source:
      type: git
      url: https://github.com/hzkincony/esphome-loxone
      ref: v1.2.0

loxone:
  id: loxone1
  protocol: tcp # tcp or udp
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

text_sensor:
  - platform: template
    name: "Current 1"
    id: current_1
    lambda: |-
      return {"12.6"};
    update_interval: 10s
    on_value:
      - lambda: !lambda |-
          auto current1 = id(current_1).state;
          auto voltage1 = id(voltage_1).state;
          id(loxone1).send_string_data("RELAY-GET_energy-255,1," + current1 + "," + voltage1 + ",OK");
  - platform: template
    name: "Voltage 1"
    id: voltage_1
    lambda: |-
      return {"220"};
    update_interval: 10s
    on_value:
      - lambda: !lambda |-
          auto current = id(current_1).state;
          auto voltage = id(voltage_1).state;
          id(loxone1).send_string_data("RELAY-GET_energy-255,1," + current + "," + voltage + ",OK");
  - platform: template
    name: "Current 2"
    id: current_2
    lambda: |-
      return {"22.6"};
    update_interval: 15s
    on_value:
      - lambda: !lambda |-
          auto current = id(current_2).state;
          auto voltage = id(voltage_2).state;
          id(loxone1).send_string_data("RELAY-GET_energy-255,2," + current + "," + voltage + ",OK");
  - platform: template
    name: "Voltage 2"
    id: voltage_2
    lambda: |-
      return {"230"};
    update_interval: 15s
    on_value:
      - lambda: !lambda |-
          auto current = id(current_2).state;
          auto voltage = id(voltage_2).state;
          id(loxone1).send_string_data("RELAY-GET_energy-255,2," + current + "," + voltage + ",OK");

interval:
  - interval: 1min
    then:
      - lambda: !lambda |-
          auto current = id(current_1).state;
          auto voltage = id(voltage_1).state;
          id(loxone1).send_string_data("RELAY-GET_energy-255,1," + current + "," + voltage + ",OK");
          delay(10);
          current = id(current_2).state;
          voltage = id(voltage_2).state;
          id(loxone1).send_string_data("RELAY-GET_energy-255,2," + current + "," + voltage + ",OK");

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