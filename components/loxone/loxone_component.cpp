#include "loxone_component.h"

namespace esphome {
  namespace loxone {
    void LoxoneComponent::setup() {

    }

    void LoxoneComponent::fire_triggers() {
      // 检查缓冲区中是否含有 '\n'，即是否有完整的指令
      if (delimiter_ == "") {
        return;
      }

      size_t pos;
      while ((pos = receive_string_buffer_.find(delimiter_)) != std::string::npos) {
        // 提取完整的指令
        std::string command = receive_string_buffer_.substr(0, pos);
        receive_string_buffer_.erase(0, pos + 1); // 从缓冲区中移除这个指令

        // 对每一个完整的指令调用 triggers_ 的 trigger 方法
        if (!command.empty()) {
          // 假设 triggers_ 是一个能够响应字符串指令的对象
          for (auto& trigger : string_triggers_) {
            trigger->trigger(command);
          }
        }
      }
    }

    void LoxoneComponent::ensure_connect_udp() {
      if (udp_client_.connected()) {
        if (client_ready_ == false) {
          ESP_LOGD(TAG, "client connected");
        }

        client_ready_ = true;
      } else {
        client_ready_ = false;
        ESP_LOGD(TAG, "client not connected");
      }

      if (!client_ready_) {
        ip_addr_t addr;
        ipaddr_aton(loxone_ip_.c_str(), &addr);
        if (udp_client_.connect(&addr, loxone_port_)) {
          ESP_LOGD(TAG, "client connecting...");
        } else {
          ESP_LOGD(TAG, "client connect failed");
        }
      }
    }

    void LoxoneComponent::update() {
      if (!network::is_connected()) {
        ESP_LOGD(TAG, "network not ready");
        return;
      }

      ensure_connect_udp();

      if (client_ready_) {
        while (!send_string_buffer_.empty()) {
          std::string d = send_string_buffer_.front();
          udp_client_.print(d.c_str());
          udp_client_.print(delimiter_.c_str());
          ESP_LOGD(TAG, "pop from queue, string data: %s", d.c_str());

          send_string_buffer_.pop();
        }
      }
    }

    void LoxoneComponent::send_string_data(std::string data) {
      if (!client_ready_) {
        if (send_string_buffer_.size() >= send_buffer_length_) {
          ESP_LOGW(TAG, "send buffer is full, discarding some data");
          send_string_buffer_.pop();
        }

        send_string_buffer_.push(data);
        ESP_LOGD(TAG, "client is not ready, push into buffer, string data: %s", data.c_str());
        return;
      }

      udp_client_.print(data.c_str());
      udp_client_.print(delimiter_.c_str());

      ESP_LOGD(TAG, "send string data: %s", data.c_str());
    }
  }
}
