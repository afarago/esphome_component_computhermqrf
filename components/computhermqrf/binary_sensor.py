import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_DEVICE_CLASS, DEVICE_CLASS_RUNNING, CONF_ID, CONF_CODE, CONF_NAME, CONF_ICON, ICON_THERMOMETER
from . import computhermqrf_ns, ComputhermQRF, CONF_ComputhermQRF_ID, hex_uint20_t

DEPENDENCIES = ["computhermqrf"]

ComputhermQRF_BinarySensor = computhermqrf_ns.class_("ComputhermQThermostat_BinarySensor", binary_sensor.BinarySensor, cg.Component)

def validate_config(config):
    # todo: sould not allow binary_sensor if receiver_pin is not registered
    return config

CONFIG_SCHEMA = cv.All(
    binary_sensor.BINARY_SENSOR_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(ComputhermQRF_BinarySensor),
        cv.GenerateID(CONF_ComputhermQRF_ID): cv.use_id(ComputhermQRF),
        cv.Required(CONF_CODE): hex_uint20_t,
        # cv.Required(CONF_CODE): cv.string,
        cv.Optional(
            CONF_DEVICE_CLASS, default=DEVICE_CLASS_RUNNING
        ): binary_sensor.device_class,
        cv.Optional(
            CONF_ICON, default=ICON_THERMOMETER
        ): cv.icon
    }), 
    validate_config,
)

async def to_code(config):
    hub = await cg.get_variable(config[CONF_ComputhermQRF_ID])

    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await binary_sensor.register_binary_sensor(var, config)

    cg.add(var.setName(config[CONF_NAME]))
    code_string = format(config[CONF_CODE], 'X')
    cg.add(var.setCode(code_string))
    
    cg.add(hub.addSensor(var))
    cg.add_define("USE_COMPUTHERMQRF_BINARY_SENSOR")
