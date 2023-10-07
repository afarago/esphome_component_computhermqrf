import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button
from esphome.const import CONF_ID, CONF_NAME, CONF_MODE
from voluptuous.util import Lower
from . import computhermqrf_ns, ComputhermQRF, CONF_ComputhermQRF_ID

DEPENDENCIES = ["computhermqrf"]

ComputhermQRF_Button = computhermqrf_ns.class_(
    "ComputhermQThermostat_PairingButton", button.Button, cg.Component)

CONF_MODE_PAIRING = "pair"

# TODO: allow pairing button only if Switches are enabled, otherwise fail

CONFIG_SCHEMA = \
    button.BUTTON_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(ComputhermQRF_Button),
            cv.GenerateID(CONF_ComputhermQRF_ID): cv.use_id(ComputhermQRF),
            cv.Required(CONF_MODE): cv.one_of(CONF_MODE_PAIRING)
        })


async def to_code(config):
    hub = await cg.get_variable(config[CONF_ComputhermQRF_ID])

    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await button.register_button(var, config)
    cg.add_define("USE_COMPUTHERMQRF_BUTTON_PAIR")

    cg.add(hub.setPairingButton(var))
