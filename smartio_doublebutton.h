#ifndef Included_SmartIO_DoubleButton_H
#define Included_SmartIO_DoubleButton_H

#include <smartio_button.h>

class DoubleButton : MainLoopListener {
	public:
		static const char TYPE='b';
		static const int ACTION_KLICK=1;
		static const int ACTION_HOLD=255;
		static const int ACTION_RELEASE=0;

		DoubleButton(Bus* bus, unsigned int pin1, unsigned int pin2,
			byte button1_id, byte button2_id, byte button3_id);

	private:
		Bus* bus;
		unsigned int pin1; // pin of pysical button1
		unsigned int pin2; // pin of pysical button2
		byte button1_id; // id of logical button1
		byte button2_id; // id of logical button2
		byte button3_id; // id of logical button3 (pressed phisical 1+2 together)

		ButtonDebounce debounce1;
		ButtonDebounce debounce2;

		byte state; // state (0=no button, 1=button1, 2=button2, 3=both)
		byte temp_state; // Temporary state (0=no button, 1=button1, 2=button2, 3=both)
		unsigned long temp_state_since; // temporary state since when
		const unsigned long temp_state_duration_ms=100; // ignore states less than this (e.g. when 2 buttons are pressed together with a slight delay)

		ButtonAction action1;
		ButtonAction action2;
		ButtonAction action3;

		void loop();

		byte combine_states(boolean state1, boolean state2);

		void handle_button_action(byte button_id, int new_action);
};

#endif // Included_SmartIO_DoubleButton_H
