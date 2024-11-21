#pragma once

#include <string>
#include <queue>

#include "esphome.h"
#include "esphome/core/component.h"
#include "AsyncUDP.h"
#include "AsyncTCP.h"

#define TAG "loxone"

namespace esphome {
  namespace loxone {
    class OnStringDataTrigger;

    class LoxoneComponent : public PollingComponent {
    public:
      LoxoneComponent() : PollingComponent(5000) {};
      void setup() override;
      void update() override;
      void send_string_data(std::string data);
      void set_protocol(std::string protocol) {
        this->protocol_ = protocol;
      };
      void set_loxone_ip(std::string loxone_ip) {
        this->loxone_ip_ = loxone_ip;
      };
      void set_loxone_port(uint16_t port) {
        this->loxone_port_ = port;
      };
      void set_listen_port(uint16_t port) {
        this->listen_port_ = port;
      };
      void set_delimiter(std::string delimiter) {
        this->delimiter_ = delimiter;
      };
      void set_send_buffer_length(uint8_t buffer_length) {
        this->send_buffer_length_ = buffer_length;
      };

      void add_string_trigger(OnStringDataTrigger *trigger) {
        this->string_triggers_.push_back(trigger);
      };
    protected:
      std::vector<OnStringDataTrigger *> string_triggers_{};
      std::string protocol_;
      std::string loxone_ip_;
      uint16_t loxone_port_;
      uint16_t listen_port_;
      uint8_t send_buffer_length_;
      std::string delimiter_;
      AsyncUDP udp_client_;
      AsyncUDP udp_server_;
      AsyncClient tcp_client_;
      AsyncServer* tcp_server_;
      std::string receive_string_buffer_;
      std::queue<std::string> send_string_buffer_{};
      bool server_ready_ = false;
      bool client_ready_ = false;

      //void ensure_listen_udp();
      //void ensure_listen_tcp();
      void fire_triggers();
      //void ensure_connect_tcp();
      void ensure_connect_udp();
    };

    class OnStringDataTrigger : public Trigger<std::string>, public Component {
      friend class LoxoneComponent;

    public:
      explicit OnStringDataTrigger(LoxoneComponent *parent)
        : parent_(parent){};

      void setup() override { this->parent_->add_string_trigger(this); }

    protected:
      LoxoneComponent *parent_;
    };

  }
}
