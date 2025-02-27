# Loxone Component
more information, you can check with KinCony's webpage: https://www.kincony.com

# Core Yaml
```yaml
external_components:
  - source:
      type: git
      url: https://github.com/chudini93/esphome-loxone-readonly
      ref: v1.0.1

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
  loxone_ip: "192.168.50.124" # loxone server ip
  loxone_port: 9999 # loxone server port

  # Send buffer length
  # When the network is not ready, it can buffer some commands that are pending to be sent.
  send_buffer_length: 64
```
