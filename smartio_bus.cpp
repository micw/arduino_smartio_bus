#include <Arduino.h>
#include <smartio_bus.h>
#include <smartio_button.h>

Bus::Bus() {

}

void Bus::send(char input_type, unsigned long input_id, byte state) {
	if (debug) {
		debug_log(input_type, input_id, state);
	}
	for (int i=0;i<actions.size();i++) {
		BusAction* action=actions[i];
		if (action->input_type!=input_type) continue;
		if (action->input_id!=input_id) continue;
		if (action->state!=state) continue;
		action->callback();
	}
}

void Bus::debug_log(char input_type, unsigned long input_id, byte state) {
	if (input_type=='b') { // a button
		Serial.print("BUTTON ");
		Serial.print(input_id,HEX);
		Serial.print(": ");
		if (state==Button::ACTION_KLICK) {
			Serial.println("klicked");
		} else if (state==Button::ACTION_RELEASE) {
			Serial.println("released");
		} else if (state==Button::ACTION_HOLD) {
			Serial.println("pressed");
		}
	} else if (input_type=='h') { // a woomewizard button
		Serial.print("HOMEWIZARD BUTTON ");
		Serial.print(input_id,HEX);
		Serial.print(": ");
		if (state==0) {
			Serial.println("OFF");
		} else if (state==1) {
			Serial.println("ON");
		}
	} else {
		Serial.print("INPUT ");
		Serial.print(input_type);
		Serial.print(" ");
		Serial.print(input_id,HEX);
		Serial.print(": ");
		Serial.println(state);
	}
}

void Bus::on(char input_type, unsigned long input_id, byte state, BusActionCallback bus_action_function) {
	actions.push_back(new BusAction {
		input_type,
		input_id,
		state,
		bus_action_function
	});
}
