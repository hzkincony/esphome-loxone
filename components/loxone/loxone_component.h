#pragma once

#include <string>
#include <queue>

#include "esphome.h"
#include "esphome/core/component.h"
#include "AsyncUDP.h"

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
      void add_string_trigger(OnStringDataTrigger *trigger) {
        this->string_triggers_.push_back(trigger);
      };
    protected:
      std::vector<OnStringDataTrigger *> string_triggers_{};
      std::string loxone_ip_;
      uint16_t loxone_port_;
      uint16_t listen_port_;
      std::string delimiter_;
      AsyncUDP udp_client_;
      AsyncUDP udp_server_;
      std::string buffer_;
      std::queue<std::string> pending_send_string_data_{};
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
