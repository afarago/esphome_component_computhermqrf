import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.config_validation import hex_int_range 
from esphome.components import switch
from esphome.const import CONF_DEVICE_CLASS, DEVICE_CLASS_RUNNING, CONF_ID, CONF_CODE, CONF_NAME, CONF_ICON
from voluptuous.util import Lower
from . import computhermqrf_ns, ComputhermQRF, CONF_ComputhermQRF_ID, hex_uint20_t

DEPENDENCIES = ["computhermqrf"]

ComputhermQRF_Switch = computhermqrf_ns.class_("ComputhermQThermostat_Switch", switch.Switch)

CONF_PAIRING_MODE = "pairing_mode" 
ICON_THERMOMETER_LINES = "mdi:thermometer-lines"

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
        cv.Optional(CONF_PAIRING_MODE): cv.boolean,
        cv.Optional(
            CONF_ICON, default=ICON_THERMOMETER_LINES
        ): cv.icon
    }),
    validate_config
)

async def to_code(config):
    hub = await cg.get_variable(config[CONF_ComputhermQRF_ID])

    var = cg.new_Pvariable(config[CONF_ID])
    await switch.register_switch(var, config)
    if (CONF_PAIRING_MODE in config and config[CONF_PAIRING_MODE]):
        cg.add(var.setPairingMode())
    cg.add(var.setName(config[CONF_NAME]))
    code_string = format(config[CONF_CODE], 'X')
    cg.add(var.setCode(code_string))
    cg.add(hub.addSwitch(var))

    # var = await binary_sensor.new_binary_sensor(config)
    # var = cg.new_Pvariable(config[CONF_ID], config[CONF_NAME])
    # cg.add(cg.App.register_binary_sensor(var))
    # await setup_binary_sensor_core_(var, config)
    # func = getattr(hub, DIRECTIONS[config[CONF_DIRECTION]])
    # cg.add(func(var))
