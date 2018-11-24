#include <Arduino.h>
#include <smartio_doublebutton.h>

DoubleButton::DoubleButton(Bus* bus, unsigned int pin1, unsigned int pin2,
			byte button1_id, byte button2_id, byte button3_id):
		bus(bus),pin1(pin1),pin2(pin2),
		button1_id(button1_id),button2_id(button2_id),button3_id(button3_id) {
	pinMode(pin1,INPUT_PULLUP);
	pinMode(pin2,INPUT_PULLUP);
	boolean state1=!digitalRead(pin1);
	boolean state2=!digitalRead(pin2);
	debounce1.init(state1);
	debounce2.init(state2);
	state=combine_states(state1,state2);
}


byte DoubleButton::combine_states(boolean state1, boolean state2) {
	if (state1 && state2) return 3;
	if (state1) return 1;
	if (state2) return 2;
	return 0;
}

void DoubleButton::loop() {
	byte new_state=combine_states(
		debounce1.update(!digitalRead(pin1)),
		debounce2.update(!digitalRead(pin2)));

	if (new_state!=state) {
		if (new_state!=temp_state) {
			temp_state=new_state;
			temp_state_since=millis();
		} else if (millis()-temp_state_since>temp_state_duration_ms) {
			state=temp_state;
		}
	}

	handle_button_action(button1_id,action1.update(state==1));
	handle_button_action(button2_id,action2.update(state==2));
	handle_button_action(button3_id,action3.update(state==3));
}

void DoubleButton::handle_button_action(byte button_id, int new_action) {
	if (new_action>=0) {
		bus->send(TYPE,button_id,new_action);
	}
}
