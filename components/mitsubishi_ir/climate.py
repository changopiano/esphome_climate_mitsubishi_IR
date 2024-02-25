import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate_ir
from esphome.const import CONF_ID

CODEOWNERS = ["@changopiano"]
AUTO_LOAD = ["climate_ir"]

mitsubishi_ns = cg.esphome_ns.namespace("mitsubishi_ir")
MitsubishiIRClimate = mitsubishi_ns.class_("MitsubishiIRClimate", climate_ir.ClimateIR)

CONFIG_SCHEMA = climate_ir.CLIMATE_IR_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(MitsubishiIRClimate),
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await climate_ir.register_climate_ir(var, config)
