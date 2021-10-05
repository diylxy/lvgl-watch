#include "A_config.h"

IRac irac(WATCH_IR_LED);     // Create a A/C object using GPIO to sending messages with.
IRsend irsend(WATCH_IR_LED); // Set the GPIO to be used to sending the message.

void WatchIR::ACModeReset()
{
    irac.next.protocol = decode_type_t::DAIKIN;      // Set a protocol to use.
    irac.next.model = 1;                             // Some A/Cs have different models. Try just the first.
    irac.next.mode = stdAc::opmode_t::kCool;         // Run in cool mode initially.
    irac.next.celsius = true;                        // Use Celsius for temp units. False = Fahrenheit
    irac.next.degrees = 25;                          // 25 degrees.
    irac.next.fanspeed = stdAc::fanspeed_t::kMedium; // Start the fan at medium.
    irac.next.swingv = stdAc::swingv_t::kOff;        // Don't swing the fan up or down.
    irac.next.swingh = stdAc::swingh_t::kOff;        // Don't swing the fan left or right.
    irac.next.light = true;                         // Turn off any LED/Lights/Display that we can.
    irac.next.beep = true;                          // Turn off any beep from the A/C if we can.
    irac.next.econo = false;                         // Turn off any economy modes if we can.
    irac.next.filter = false;                        // Turn off any Ion/Mold/Health filters if we can.
    irac.next.turbo = false;                         // Don't use any turbo/powerful/etc modes.
    irac.next.quiet = false;                         // Don't use any quiet/silent/etc modes.
    irac.next.sleep = -1;                            // Don't set any sleep time or modes.
    irac.next.clean = false;                         // Turn off any Cleaning options if we can.
    irac.next.clock = -1;                            // Don't set any current time if we can avoid it.
    irac.next.power = false;                         // Initially start with the unit off.
}

WatchIR ir;
