#include "loxone_component.h"
#include "AsyncUDP.h"

namespace esphome {
  namespace loxone {
    AsyncUDP Udp;
    bool udp_server_ready = false;

    void UniversalUartComponent::setup() {

    }

    void UniversalUartComponent::update() {
      if (!network::is_connected()) {
        ESP_LOGD(TAG, "network not ready");
        return;
      }

      if (udp_server_ready == false) {
        if (Udp.listen(9999)) {
          udp_server_ready = true;
          ESP_LOGD(TAG, "listen");
          Udp.onPacket([](AsyncUDPPacket packet) {
            ESP_LOGD(TAG, "receive computer data, length=%d", packet.length());
            packet.printf("I am esp, data length=%d", packet.length());
            ESP_LOGD(TAG, "send data to computer");
          });
        }

      }
    }
  }

}
