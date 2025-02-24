#include "ledc.h"

namespace Haptics {
namespace LEDC {
Logging::Logger logger("LEDC");

unsigned long nextTick = 0; // big ticks that represent moments where all non-zero pins are set to high
unsigned long nextTock = 0; // small "subticks" that have RESOLUTION number of steps that set pins to low
uint8_t tockCount = 0; // marks how many subticks have been achieved in a tock period
unsigned long missedTocks = 0;
unsigned long oldMissedTocks = 0;

//frequency calculationss
const double_t UpdateFrequency = LEDC_FREQUENCY;
const unsigned long tickPeriod = (1./UpdateFrequency)*1000000; // microseconds
const unsigned long tockPeriod = tickPeriod / (1 << LEDC_RESOLUTION); // microseconds


void tick() {
    unsigned long now = micros();

    /// NOTE: IF WE OVERRUN IT WILL ENCROACH END VALUES. nonzero == 100% duty at extreme
    /// NOTE: CURRENTLY SLIDES ONE CYCLE (16ms) every 10 seconds

    if (now > nextTick) { //check for tick first 

        // unsigned long diff = missedTocks - oldMissedTocks;
        // if (diff != 0) {
        //    oldMissedTocks = missedTocks;
        //    if (diff > 10) { // see if we should make a big deal about this
        //        //logger.debug("late: %d", diff);
        //   }
           
        // }

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
            } else {
                digitalWrite(Haptics::conf.motor_map_ledc[motor], LOW);
            }
        }
        return;
    } 

    // Catch up if we missed a tock
    bool first = true;
    while (now > nextTock) {
        tockCount++; // increment tock count
        
        uint16_t num_motors = Haptics::conf.motor_map_ledc_num;
        for(uint16_t motor = 0; motor < num_motors; motor++) {
            uint8_t dutyCycle = Haptics::globals.ledcMotorVals[motor];
            if (dutyCycle != 0 && dutyCycle <= tockCount) { // stop spike when motor duty stops
                digitalWrite(Haptics::conf.motor_map_ledc[motor], LOW);
            }
        }
        nextTock += tockPeriod;
        // Update now in case a lot of time has passed
        now = micros();

        if (first) { // skip first
            first = false;
        } else {
            missedTocks++;
        }
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