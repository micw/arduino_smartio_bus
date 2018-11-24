#ifndef Included_SmartIO_Bus_H
#define Included_SmartIO_Bus_H

#include <Arduino.h>
#include <vector.h>

typedef void (*BusActionCallback) ();
struct BusAction {
	char input_type;
	byte input_id;
	byte state;
	BusActionCallback callback;
};

class Bus {
	public:
		Bus();
		void send(char input_type, byte input_id, byte state);
		void on(char input_type, byte input_id, byte state, BusActionCallback bus_action_function);
		boolean debug;
	private:
		Vector<BusAction> actions;
		void debug_log(char input_type, byte input_id, byte state);
};

#endif // Included_SmartIO_Bus_H 
