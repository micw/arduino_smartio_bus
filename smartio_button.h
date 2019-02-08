#ifndef Included_SmartIO_Button_H
#define Included_SmartIO_Button_H

#include <smartio_main_loop.h>
#include <smartio_bus.h>

/**
 * Debounce-Handling for a single physical button
 */
class ButtonDebounce {
	public:
		void init(boolean pressed);
		boolean update(boolean pressed);

	private:
		boolean state; // current state (0 or 1)
		boolean debounce_state; // temporary state while debouncing
		unsigned long debounce_state_since;
		const unsigned long debounce_delay_ms=15;
};
/**
 * Action-Handling (klick, hold, release) for a single logical button
 */
class ButtonAction {
	public:
		void init(boolean initial_state);
		int update(boolean new_state);

	private:
		boolean state; // current state (0 or 1)
		unsigned long state_since; // state since when - required for klick/hold logic

		byte holding; // 0 if button is not pressed, 1 if button is pressed less than hold_time, 2 if it's pressed longer than hold time
		const unsigned long holding_delay_ms=500; // if pressed longer than this, it's holding, otherwise klicked
};

class Button : MainLoopListener {
	public:
		static const char TYPE='b';
		static const int ACTION_KLICK=1;
		static const int ACTION_HOLD=255;
		static const int ACTION_RELEASE=0;

		Button(Bus* bus, unsigned int pin, unsigned long button_id);

	private:
		Bus* bus;
		unsigned int pin; // pin of this button
		unsigned long button_id;
		ButtonDebounce debounce;
		ButtonAction action;

		void loop();
};

#endif // Included_SmartIO_Button_H
