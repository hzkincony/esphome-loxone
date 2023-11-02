#pragma once

#include "esphome.h"
#include "esphome/core/component.h"

#define TAG "universal_uart"

namespace esphome {
  namespace loxone {
    class OnDataTrigger;

    class LoxoneComponent : public PollingComponent {
    public:
      void setup() override;
      void update() override;
      void set_loxone_ip(std::string loxone_ip) {
        this->loxone_ip_ = loxone_ip;
      }
      void set_loxone_port(int loxone_port) {
        this->loxone_port_ = loxone_port;
      }
      void add_trigger(OnDataTrigger *trigger) {
        this->triggers_.push_back(trigger);
      }
    protected:
      std::vector<OnDataTrigger *> triggers_{};
      std:string loxone_ip_;
      int loxone_port_;
    }

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
