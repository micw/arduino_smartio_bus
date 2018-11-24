/**
 * Example:
 * - one button on D1
 * - one button on D2
 * - one led on D4, D5 and D6
 * - ON/OFF with tapping on button
 * - DIM with holding of button
 * - Led on D4 controlled with button1
 * - Led on D5 controlled with button2
 * - Led on D6 controlled with button1+2 together
 */

#define SMARTIO_DEBUG 1
#include <smartio.h>

Bus bus;
DoubleButton b(&bus,D1,D2,1,2,3);
DimmableLed led1(D4);
DimmableLed led2(D5);
DimmableLed led3(D6);

void setup() {
  Serial.begin(115200);

  bus.debug=true;
  bus.on(Button::TYPE,1,Button::ACTION_KLICK,[]() { led1.toggle(); });
  bus.on(Button::TYPE,1,Button::ACTION_HOLD,[]() { led1.dim_start(); });
  bus.on(Button::TYPE,1,Button::ACTION_RELEASE,[]() { led1.dim_stop(); });

  bus.on(Button::TYPE,2,Button::ACTION_KLICK,[]() { led2.toggle(); });
  bus.on(Button::TYPE,2,Button::ACTION_HOLD,[]() { led2.dim_start(); });
  bus.on(Button::TYPE,2,Button::ACTION_RELEASE,[]() { led2.dim_stop(); });

  bus.on(Button::TYPE,3,Button::ACTION_KLICK,[]() { led3.toggle(); });
  bus.on(Button::TYPE,3,Button::ACTION_HOLD,[]() { led3.dim_start(); });
  bus.on(Button::TYPE,3,Button::ACTION_RELEASE,[]() { led3.dim_stop(); });

}


void loop() {
  MainLoop::loop();
}