#pragma once

#include <string>
#include <queue>

#include "esphome.h"
#include "esphome/core/component.h"
#include "AsyncUDP.h"

#define TAG "loxone"

namespace esphome {
  namespace loxone {
    class OnDataTrigger;

    class LoxoneComponent : public PollingComponent {
    public:
      LoxoneComponent() : PollingComponent(5000) {};
      void setup() override;
      void update() override;
      void send_string_data(std::string data);
      void set_loxone_ip(std::string loxone_ip) {
        this->loxone_ip_ = loxone_ip;
      };
      void set_loxone_port(uint16_t loxone_port) {
        this->loxone_port_ = loxone_port;
      };
      void add_trigger(OnDataTrigger *trigger) {
        this->triggers_.push_back(trigger);
      };
    protected:
      std::vector<OnDataTrigger *> triggers_{};
      std::string loxone_ip_;
      uint16_t loxone_port_;
      AsyncUDP udp_client_;
      AsyncUDP udp_server_;
      std::queue<std::string> pending_send_string_data_{};
    };

    class OnDataTrigger : public Trigger<std::vector<uint8_t>>, public Component {
      friend class LoxoneComponent;

    public:
      explicit OnDataTrigger(LoxoneComponent *parent)
        : parent_(parent){};

      void setup() override { this->parent_->add_trigger(this); }

    protected:
      LoxoneComponent *parent_;
    };

  }
}
