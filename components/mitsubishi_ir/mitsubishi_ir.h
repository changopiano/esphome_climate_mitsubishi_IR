#pragma once

#include "esphome/components/climate_ir/climate_ir.h"

#include <cinttypes>

namespace esphome {
 namespace mitsubishi_ir {
  
  // Temperature
  const uint8_t MITSUBISHI_IR_TEMP_MIN = 16;  // Celsius
  const uint8_t MITSUBISHI_IR_TEMP_MAX = 31;  // Celsius
  
  class MitsubishiIRClimate : public climate_ir::ClimateIR {
   public:
    MitsubishiIRClimate() : climate_ir::ClimateIR(MITSUBISHI_IR_TEMP_MIN, MITSUBISHI_IR_TEMP_MAX) {}
  
   protected:
    /// Transmit via IR the state of this climate controller.
    void transmit_state() override;
  };
  
 }  // namespace mitsubishi_ir
}  // namespace esphome
