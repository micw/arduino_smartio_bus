#include <Arduino.h>
#include <smartio_bus.h>

Bus::Bus() {

}

void Bus::send(char input_type, byte input_id, byte state) {
	Serial.print(input_type);
	Serial.println(" ");
	Serial.println(actions.size());
	for (int i=0;i<actions.size();i++) {
		BusAction* action=actions[i];
		if (action->input_type!=input_type) continue;
		if (action->input_id!=input_id) continue;
		if (action->state!=state) continue;
		action->callback();
	}
}

void Bus::on(char input_type, byte input_id, byte state, BusActionCallback bus_action_function) {
	BusAction action=BusAction {
		input_type,
		input_id,
		state,
		bus_action_function
	};
	actions.push_back(&action);
}
