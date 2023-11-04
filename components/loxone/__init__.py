import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import uart
from esphome.const import (
    CONF_ID,
    CONF_TRIGGER_ID,
)

DEPENDENCIES = ['network']

loxone_ns = cg.esphome_ns.namespace('loxone')
LoxoneComponent = loxone_ns.class_('LoxoneComponent', cg.PollingComponent)
OnDataTrigger = loxone_ns.class_(
    "OnDataTrigger", automation.Trigger.template(cg.std_vector.template(cg.uint8)), cg.Component
)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(LoxoneComponent),
    cv.Required("loxone_ip"): cv.ipv4,
    cv.Required("loxone_port"): cv.int_range(0, 65535),
    cv.Optional("on_data"): automation.validate_automation(
        {
            cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(OnDataTrigger),
        }
    ),
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    cg.add_library("ESP32 Async UDP", None)
    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_loxone_ip(str(config["loxone_ip"])))
    cg.add(var.set_loxone_port(config["loxone_port"]))
    yield cg.register_component(var, config)

    for conf in config.get("on_data", []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        yield cg.register_component(trigger, conf)
        yield automation.build_automation(trigger, [(cg.std_vector.template(cg.uint8), "data")], conf)
