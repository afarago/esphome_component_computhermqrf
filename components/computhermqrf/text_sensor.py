import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor, text_sensor
from esphome.const import (
    CONF_DEVICE_CLASS, DEVICE_CLASS_RUNNING, CONF_ID, CONF_CODE, CONF_NAME, CONF_ICON, ICON_THERMOMETER,
    ENTITY_CATEGORY_DIAGNOSTIC
)
from . import (
    computhermqrf_ns, ComputhermQRF, CONF_ComputhermQRF_ID, hex_uint20_t
)

DEPENDENCIES = ["computhermqrf"]
CONF_LAST_UNREGISTERED_ADDRESS = "last_unregistered_address"

TYPES = [CONF_LAST_UNREGISTERED_ADDRESS]

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ComputhermQRF_ID): cv.use_id(ComputhermQRF),
        cv.Optional(CONF_LAST_UNREGISTERED_ADDRESS): text_sensor.text_sensor_schema(
            #icon=ICON_ACCURACY
        ),
    }
)

async def setup_conf(config, key, hub):
    if key in config:
        conf = config[key]
        sens = await text_sensor.new_text_sensor(conf)
        # await cg.register_component(var, conf)
        cg.add(getattr(hub, f"set_{key}_text_sensor")(sens))

async def to_code(config):
    hub = await cg.get_variable(config[CONF_ComputhermQRF_ID])
    for key in TYPES:
        await setup_conf(config, key, hub)
    cg.add_define("USE_COMPUTHERMQRF_UNREGISTERED_ADDR_TEXT_SENSOR")        