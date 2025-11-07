#ifndef LEDC_ME_H // idk if just LEDC would interfere
#define LEDC_ME_H

#include <Arduino.h>

#include "globals.h"
#include "software_defines.h"
#include "board_defines.h"
#include "logging/Logger.h"
#include "config/config.h"

namespace Haptics {
namespace LEDC {

    void tick();
    inline int setChannel(const uint8_t channel, const uint16_t duty);
    int setAllTo(const uint16_t duty);
    int start(Haptics::Conf::Config *conf);
} // namespace LEDC
} // namespace Haptics

#endif // LEDC_ME_H