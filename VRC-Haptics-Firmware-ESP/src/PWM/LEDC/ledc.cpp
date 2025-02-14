#include "ledc.h"

namespace Haptics {
namespace LEDC {
Logging::Logger logger("LEDC");

#define RESOLUTION (1 << 8)
unsigned long nextTick = 0; // big ticks that represent moments where all non-zero pins are set to high
unsigned long nextTock = 0; // small "subticks" that have RESOLUTION number of steps that set pins to low
uint8_t tockCount = 0; // marks how many subticks have been achieved in a tock period
const double_t UpdateFrequency = LEDC_FREQUENCY;
const unsigned long tickPeriod = (1./UpdateFrequency)*1000000; // microseconds
const unsigned long tockPeriod = tickPeriod / RESOLUTION; // microseconds

void tick() {
    unsigned long now = micros();

    /// NOTE: IF WE OVERRUN IT WILL ENCROACH END VALUES. nonzero == 100% duty at extreme
    /// NOTE: CURRENTLY SLIDES ONE CYCLE (16ms) every 10 seconds

    if (now > nextTick) { //check for tick first 

        uint16_t num_motors = Haptics::conf.motor_map_ledc_num;// only need this if we make it in

        // schedule next tick and tock
        nextTick = now + tickPeriod;
        nextTock = now + tockPeriod;

        // reset tock count
        tockCount = 0;

        // set all to high (except those at zero)
        for(uint16_t motor = 0; motor < num_motors; motor++) {
            uint8_t dutyCycle = Haptics::globals.ledcMotorVals[motor];
            if (dutyCycle) {
                digitalWrite(Haptics::conf.motor_map_ledc[motor], HIGH);
            }
        }
        return;
    } 
    if (now > nextTock) {
        uint16_t num_motors = Haptics::conf.motor_map_ledc_num;
        tockCount++; // increment beforehand

        // see which values should be set on this tock 
        // maybe sort the list somewhere so we just go through the few indices due on this tock?
        for(uint16_t motor = 0; motor < num_motors; motor++) {
            uint8_t dutyCycle = Haptics::globals.ledcMotorVals[motor];
            if (dutyCycle == tockCount) {
                digitalWrite(Haptics::conf.motor_map_ledc[motor], LOW);
            }
        }
        nextTock = now + tockPeriod;
        return;
    }
}

int start(Config *conf) {

    // Update pins and declare pinmodes
    for (const auto& pin : conf->motor_map_ledc) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, HIGH); //Cycling this seems to get it to work, idk why, pinmode should just work
        digitalWrite(pin, LOW);
    }
    return 0;
}

} // namespace LEDC
} // namespace Haptics