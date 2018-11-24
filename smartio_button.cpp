#include <Arduino.h>
#include <smartio_button.h>

Button::Button(Bus* bus, unsigned int pin, byte button_id):
		bus(bus),pin(pin),button_id(button_id) {
	pinMode(pin,INPUT_PULLUP);
	boolean state=!digitalRead(pin);
	action.init(state);
	debounce.init(state);
}

void Button::loop() {
	boolean new_state=debounce.update(!digitalRead(pin));
	int new_action=action.update(new_state);
	if (new_action>=0) {
		bus->send(TYPE,button_id,new_action);
	}
}

void ButtonDebounce::init(boolean pressed) {
	state=pressed;
	debounce_state=state;
}

boolean ButtonDebounce::update(boolean pressed) {

	// debounce: a state change must be pressent for at least 50ms
	if (pressed!=state) {
		if (pressed!=debounce_state) {
			debounce_state=pressed;
			debounce_state_since=millis();
		} else if (millis()-debounce_state_since>debounce_delay_ms) {
			state=debounce_state;
		}
	}
	return state;
}

void ButtonAction::init(boolean initial_state) {
	state=initial_state;
	state_since=millis();
}

int ButtonAction::update(boolean new_state) {

	if (new_state!=state) {
		state=new_state;
		state_since=millis();
	}

	// a button is "klicked" if it's pressed and released within "holding_delay_ms"
	// otherwise it's "hold" and "released"

	if (state) { // button pressed
		if (holding==0) {
			holding=1;
		} else if ((holding==1) && (millis()-state_since>holding_delay_ms)) {
			holding=2;
			return Button::ACTION_HOLD;
		}
	} else if (holding>0) { // button released
		if (holding==1) { // klick
			holding=0;
			return Button::ACTION_KLICK;
		} else { // holding ended
			holding=0;
			return Button::ACTION_RELEASE;
		}
	}
	return -1;
}
