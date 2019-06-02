#include <Arduino.h>
#include <smartio_homewizard.h>
#include <ext_HomeWizard.h>

HomewizardReceiver::HomewizardReceiver(Bus* bus, unsigned int pin):
		bus(bus),pin(pin) {
	pinMode(pin,INPUT);
	// works for ESP8266 where pin==interrupt. Not sure if it works for other platforms too
	HomeWizard::startReceiving(pin); // pin == interrupt!
}

void HomewizardReceiver::loop() {
  if (HomeWizard::receiveData(&data))
  {
  	unsigned long button_id=(data.id&0xFFFFFF) << 8 | (data.unit&0xFF);
//  	Serial.println(button_id,HEX);
  	// States are switched - at least with my buttons
  	bus->send(TYPE_BUTTON,button_id,data.state?ACTION_BUTTON_OFF:ACTION_BUTTON_ON);
  }
}
