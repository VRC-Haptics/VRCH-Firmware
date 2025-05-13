#include "ledc.h"

namespace Haptics {
namespace LEDC {
Logging::Logger logger("LEDC");

//frequency calculationss
const double_t UpdateFrequency = LEDC_FREQUENCY;
const unsigned long tickPeriod = (1./UpdateFrequency)*1000000; // microseconds
const unsigned long tockPeriod = tickPeriod / (1 << LEDC_RESOLUTION); // microseconds

volatile uint8_t phase = 0;

void IRAM_ATTR pwm_isr()
{
    uint32_t setMask = 0;   // pins to drive HIGH this sub‑cycle
    uint32_t clrMask = 0;   // pins to drive LOW  this sub‑cycle

    for (int motor = 0; motor < Haptics::conf.motor_map_ledc_num; ++motor) {
        uint32_t bit = 1UL << Haptics::conf.motor_map_ledc[motor];
        if (Haptics::globals.ledcMotorVals[motor] > phase) setMask |= bit;
        else clrMask |= bit;
    }

    GPIO.out_w1ts = setMask;     // atomic SET
    GPIO.out_w1tc = clrMask;     // atomic CLEAR
    /* advance PWM phase */
    if (++phase == 1 << LEDC_RESOLUTION) phase = 0;
}

int start(Config *conf) {

    // Update pins and declare pinmodes
    for (const auto& pin : conf->motor_map_ledc) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, HIGH); //Cycling this seems to get it to work, idk why, pinmode should just work
        digitalWrite(pin, LOW);
    }

    // 1 MHz hardware timer -> 39 µs alarms
    hw_timer_t *timer = timerBegin(3, 80, true);// 80 MHz / 80 = 1 MHz
    timerAttachInterrupt(timer, &pwm_isr, true);
    timerAlarmWrite(timer, tockPeriod, true);   // fire every 39 µs
    timerAlarmEnable(timer);
    return 0;
}

} // namespace LEDC
} // namespace Haptics