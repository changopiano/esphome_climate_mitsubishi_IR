#include "mitsubishi_ir.h"
#include "esphome/core/log.h"

namespace esphome {
  namespace mitsubishi_ir {
    
    static const char *const TAG = "mitsubishi_ir.climate";
    
    const uint32_t MITSUBISHI_IR_OFF = 0x00;
    
    const uint8_t MITSUBISHI_IR_COOL = 0x18;
    const uint8_t MITSUBISHI_IR_DRY = 0x10;
    const uint8_t MITSUBISHI_IR_AUTO = 0x20;
    const uint8_t MITSUBISHI_IR_HEAT = 0x08;
    const uint8_t MITSUBISHI_IR_FAN_AUTO = 0x00;
    
    // Pulse parameters in usec
    const uint16_t MITSUBISHI_IR_BIT_MARK = 430;
    const uint16_t MITSUBISHI_IR_ONE_SPACE = 1250;
    const uint16_t MITSUBISHI_IR_ZERO_SPACE = 390;
    const uint16_t MITSUBISHI_IR_HEADER_MARK = 3500;
    const uint16_t MITSUBISHI_IR_HEADER_SPACE = 1700;
    const uint16_t MITSUBISHI_IR_MIN_GAP = 17500;
    
    void MitsubishiIRClimate::transmit_state() {
      uint32_t remote_state[18] = {0x23, 0xCB, 0x26, 0x01, 0x00, 0x20, 0x08, 0x00, 0x30,
                                   0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    
      switch (this->mode) {
        case climate::CLIMATE_MODE_COOL:
          remote_state[6] = MITSUBISHI_IR_COOL;
          break;
        case climate::CLIMATE_MODE_HEAT:
          remote_state[6] = MITSUBISHI_IR_HEAT;
          break;
        case climate::CLIMATE_MODE_HEAT_COOL:
          remote_state[6] = MITSUBISHI_IR_AUTO;
          break;
        case climate::CLIMATE_MODE_OFF:
        default:
          remote_state[5] = MITSUBISHI_IR_OFF;
          break;
      }
    
      remote_state[7] = (uint8_t) roundf(clamp<float>(this->target_temperature, MITSUBISHI_IR_TEMP_MIN, MITSUBISHI_IR_TEMP_MAX) -
                                         MITSUBISHI_IR_TEMP_MIN);
    
      ESP_LOGV(TAG, "Sending Mitsubishi target temp: %.1f state: %02" PRIX32 " mode: %02" PRIX32 " temp: %02" PRIX32,
               this->target_temperature, remote_state[5], remote_state[6], remote_state[7]);
    
      // Checksum
      for (int i = 0; i < 17; i++) {
        remote_state[17] += remote_state[i];
      }
    
      auto transmit = this->transmitter_->transmit();
      auto *data = transmit.get_data();
    
      data->set_carrier_frequency(38000);
      // repeat twice
      for (uint16_t r = 0; r < 2; r++) {
        // Header
        data->mark(MITSUBISHI_IR_HEADER_MARK);
        data->space(MITSUBISHI_IR_HEADER_SPACE);
        // Data
        for (uint8_t i : remote_state) {
          for (uint8_t j = 0; j < 8; j++) {
            data->mark(MITSUBISHI_IR_BIT_MARK);
            bool bit = i & (1 << j);
            data->space(bit ? MITSUBISHI_IR_ONE_SPACE : MITSUBISHI_IR_ZERO_SPACE);
          }
        }
        // Footer
        if (r == 0) {
          data->mark(MITSUBISHI_IR_BIT_MARK);
          data->space(MITSUBISHI_IR_MIN_GAP);  // Pause before repeating
        }
      }
      data->mark(MITSUBISHI_IR_BIT_MARK);
    
      transmit.perform();
    }
    
  }  // namespace mitsubishi_ir
}  // namespace esphome
