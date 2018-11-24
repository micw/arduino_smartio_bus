#include <Arduino.h>
#include <smartio_button.h>

Button::Button(unsigned int pin, Bus* bus, byte button_id) :
	pin(pin), bus(bus),	button_id(button_id) {
	
	pinMode(pin,INPUT_PULLUP);
	state=!digitalRead(pin);
	state_since=millis();
	debounce_state=this->state;
	holding=0;	
}

void Button::loop() {

	boolean _temp_state=!digitalRead(pin);
	// debounce: a state change must be pressent for at least 50ms
	if (_temp_state!=state) {
		if (_temp_state!=debounce_state) {
			debounce_state=_temp_state;
			debounce_state_since=millis();
		} else if (millis()-debounce_state_since>debounce_delay_ms) {
			state=debounce_state;
			state_since=millis();
		}
	}

	// a button is "klicked" if it's pressed and released within "holding_delay_ms"
	// otherwise it's "hold" and "released"

	if (state) { // button pressed
		if (holding==0) {
			holding=1;
		} else if ((holding==1) && (millis()-state_since>holding_delay_ms)) {
			holding=2;
			bus->send(TYPE,button_id,ACTION_HOLD);
		}
	} else if (holding>0) { // button released
		if (holding==1) { // klick
			bus->send(TYPE,button_id,ACTION_KLICK);
		} else { // holding ended
			bus->send(TYPE,button_id,ACTION_RELEASE);
		}
		holding=0;
	}
}