#include "loxone_component.h"

namespace esphome {
  namespace loxone {
    bool udp_server_ready = false;
    bool udp_client_ready = false;

    void LoxoneComponent::setup() {

    }

    void LoxoneComponent::update() {
      if (!network::is_connected()) {
        ESP_LOGD(TAG, "network not ready");
        return;
      }

      if (udp_server_ready == false) {
        if (udp_server_.listen(9999)) {
          udp_server_ready = true;
          ESP_LOGD(TAG, "listened");
          udp_server_.onPacket([](AsyncUDPPacket packet) {
            ESP_LOGD(TAG, "receive computer data, length=%d", packet.length());
            packet.printf("I am esp, data length=%d", packet.length());
            ESP_LOGD(TAG, "send data to computer");
          });
        }
      }

      if (udp_client_ready == false) {
        ESP_LOGD(TAG, "client connecting...");
        ip_addr_t addr;
        ipaddr_aton(loxone_ip_.c_str(), &addr);
        if (udp_client_.connect(&addr, loxone_port_)) {
          udp_client_ready = true;
          ESP_LOGD(TAG, "client connected");
        } else {
          ESP_LOGD(TAG, "client connect failed");
        }
      }
    }

    void LoxoneComponent::send_string_data(const std::string &data) {
      udp_client_.print(data.c_str());
    }
  }
}
