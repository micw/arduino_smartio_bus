/**
 * Simple example:
 * - one button on D1
 * - one led on D4
 * - ON/OFF with tapping on button 1
 * - DIM with holding of button 1
 */

#define SMARTIO_DEBUG 1
#include <smartio.h>

Bus bus;
Button b(&bus,D1,1);
DimmableLed led1(D4);

void setup() {
  Serial.begin(115200);

  bus.debug=true;
  bus.on(Button::TYPE,1,Button::ACTION_KLICK,[]() { led1.toggle(); });
  bus.on(Button::TYPE,1,Button::ACTION_HOLD,[]() { led1.dim_start(); });
  bus.on(Button::TYPE,1,Button::ACTION_RELEASE,[]() { led1.dim_stop(); });
}


void loop() {
  MainLoop::loop();
}