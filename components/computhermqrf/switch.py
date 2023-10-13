import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.config_validation import hex_int_range
from esphome.components import switch
from esphome.const import CONF_DEVICE_CLASS, DEVICE_CLASS_RUNNING, CONF_ID, CONF_CODE, CONF_NAME, CONF_ICON
from voluptuous.util import Lower
from . import (
    computhermqrf_ns, ComputhermQRF, CONF_ComputhermQRF_ID, hex_uint20_t,
    CONF_ABBREVIATION
)

DEPENDENCIES = ["computhermqrf"]

ComputhermQRF_Switch = computhermqrf_ns.class_(
    "ComputhermQThermostat_Switch", switch.Switch, cg.Component)

ICON_THERMOMETER_LINES = "mdi:thermometer-lines"
CONF_TURN_ON_WATCHDOG_INTERVAL = "turn_on_watchdog_interval"
CONF_RESEND_INTERVAL = "resend_interval"


def validate_config(config):
    # todo: sould not allow switch if transmitter_pin is not registered
    return config


CONFIG_SCHEMA = cv.All(
    switch.SWITCH_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(ComputhermQRF_Switch),
            cv.GenerateID(CONF_ComputhermQRF_ID): cv.use_id(ComputhermQRF),
            cv.Required(CONF_CODE): cv.templatable(hex_uint20_t),
            # cv.Required(CONF_CODE): cv.string,
            cv.Optional(CONF_ABBREVIATION): cv.string,
            cv.Optional(CONF_TURN_ON_WATCHDOG_INTERVAL, default="30min"): cv.positive_time_period_milliseconds,
            cv.Optional(CONF_RESEND_INTERVAL, default="30sec"): cv.positive_time_period_milliseconds,
            cv.Optional(
                CONF_ICON, default=ICON_THERMOMETER_LINES
            ): cv.icon
        }),
    validate_config
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_ComputhermQRF_ID])

    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await switch.register_switch(var, config)

    cg.add(var.setName(config[CONF_NAME]))
    # code_string = format(config[CONF_CODE], 'X')
    # cg.add(var.setCode(code_string))
    cg.add(var.setCode(config[CONF_CODE]))
    if CONF_ABBREVIATION in config:
        cg.add(var.setAbbreviation(config[CONF_ABBREVIATION]))

    if (CONF_TURN_ON_WATCHDOG_INTERVAL in config):
        cg.add(var.setTurnOnWatchdogInterval(
            config[CONF_TURN_ON_WATCHDOG_INTERVAL]))
    if (CONF_RESEND_INTERVAL in config):
        cg.add(var.setResendInterval(config[CONF_RESEND_INTERVAL]))

    cg.add(hub.addSwitch(var))
    cg.add_define("USE_COMPUTHERMQRF_SWITCH")
