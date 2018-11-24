#ifndef Included_SmartIO_Button_H
#define Included_SmartIO_Button_H

#include <smartio_bus.h>

class Button {
	public:
		static const char TYPE='b';
		static const int ACTION_KLICK=1;
		static const int ACTION_HOLD=255;
		static const int ACTION_RELEASE=0;

		Button(unsigned int pin, Bus* bus, byte button_id);
		void loop();

	private:
		unsigned int pin; // pin of this button
		Bus* bus;
		byte button_id;
		boolean state; // current state (0 or 1)
		unsigned long state_since; // state since when - required for klick/hold logic

		byte holding; // 0 if button is not pressed, 1 if button is pressed less than hold_time, 2 if it's pressed longer than hold time
		const unsigned long holding_delay_ms=500; // if pressed longer than this, it's holding, otherwise klicked

		boolean debounce_state; // for debounce
		unsigned long debounce_state_since; // for debounce
		const unsigned long debounce_delay_ms=50;
};

#endif // Included_SmartIO_Button_H
