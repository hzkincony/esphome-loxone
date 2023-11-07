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
          udp_server_.onPacket([this](AsyncUDPPacket packet) {
            ESP_LOGD(TAG, "receive data, length=%d, data=%s", packet.length(), packet.data());
            buffer_.append((char*)packet.data(), packet.length());
            ESP_LOGD(TAG, "current buffer data=%s", buffer_.c_str());

            // 检查缓冲区中是否含有 '\n'，即是否有完整的指令
            size_t pos;
            while ((pos = buffer_.find('\n')) != std::string::npos) {
              // 提取完整的指令
              std::string command = buffer_.substr(0, pos);
              buffer_.erase(0, pos + 1); // 从缓冲区中移除这个指令

              // 对每一个完整的指令调用 triggers_ 的 trigger 方法
              if (!command.empty()) {
                // 假设 triggers_ 是一个能够响应字符串指令的对象
                for (auto& trigger : string_triggers_) {
                  trigger->trigger(command);
                }
              }
            }
          });
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

        if (udp_client_ready) {
          while (!pending_send_string_data_.empty()) {
            std::string d = pending_send_string_data_.front();
            udp_client_.print(d.c_str());
            udp_client_.print(delimiter_.c_str());
            ESP_LOGD(TAG, "pop from queue, string data: %s", d.c_str());
            pending_send_string_data_.pop();
          }
        }
      }
    }

    void LoxoneComponent::send_string_data(std::string data) {
      if (!udp_client_ready) {
        ESP_LOGD(TAG, "udp client is not ready, push into queue, string data: %s", data.c_str());
        pending_send_string_data_.push(data.c_str());
        return;
      }

      udp_client_.print(data.c_str());
      udp_client_.print(delimiter_.c_str());
      ESP_LOGD(TAG, "send string data: %s", data.c_str());
    }
  }
}
