from esphome.cpp_generator import RawExpression
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.config_validation import hex_int_range, has_at_least_one_key
from esphome import automation
from esphome import pins
from esphome.components import binary_sensor, switch
from esphome.const import (
    CONF_ID,
    CONF_NUM_ATTEMPTS,
    CONF_PASSWORD,
    CONF_PORT,
    CONF_REBOOT_TIMEOUT,
    CONF_SAFE_MODE,
    CONF_TRIGGER_ID,
)
from esphome.core import CORE, coroutine_with_priority

CODEOWNERS = ["@afarago"]
AUTO_LOAD = ["binary_sensor", "switch"]

CONF_ComputhermQRF_ID = "ComputhermQRF_id"
CONF_DESCRIPTION = "PIN"
hex_uint20_t = hex_int_range(min=0, max=1048575)

computhermqrf_ns = cg.esphome_ns.namespace("computhermqrf")
ComputhermQRF = computhermqrf_ns.class_("ComputhermQRF", cg.Component) # TODO: PollingComponent

CONF_RECEIVER_PIN = "receiver_pin"
CONF_TRANSMITTER_PIN = "transmitter_pin"

CONFIG_SCHEMA = cv.All(
        cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(ComputhermQRF),
            cv.Optional(CONF_RECEIVER_PIN): pins.gpio_output_pin_schema, # TODO: check if inpit would be better
            cv.Optional(CONF_TRANSMITTER_PIN): pins.gpio_output_pin_schema,
        }
    )
    .extend(cv.COMPONENT_SCHEMA),
    cv.has_at_least_one_key(CONF_RECEIVER_PIN, CONF_TRANSMITTER_PIN)
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    if CONF_RECEIVER_PIN in config:
        pin = await cg.gpio_pin_expression(config[CONF_RECEIVER_PIN])
        cg.add(var.set_receiver_pin(pin))
    if CONF_TRANSMITTER_PIN in config:
        pin = await cg.gpio_pin_expression(config[CONF_TRANSMITTER_PIN])
        cg.add(var.set_transmitter_pin(pin))
