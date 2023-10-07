#pragma once

namespace esphome
{
    namespace computhermqrf
    {

        enum ComputhermRFMessage
        {
            heat_off = 0,
            heat_on = 1,
            pairing = 9,
            none = 0xF
        };

    }
}