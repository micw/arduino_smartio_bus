#ifndef Included_SmartIO_DimmableLed_H
#define Included_SmartIO_DimmableLed_H

#include <smartio_main_loop.h>

/**
 * A pwm-dimmable led. Pwm dim range is from 0...1023.
 * This value is gamma-corrected (so that 511 is ~50% of visible brightness)
 * and normalized to the device's dim range (0..255 on arduino, 0...1023 on esp8266)
 */
class DimmableLed : MainLoopListener {
	public:
		DimmableLed(unsigned int pin);
		void toggle();
		void dim_start();
		void dim_stop();
		void loop();
	private:
		unsigned int pin;
		int dim_value;
		int old_dim_value;
		boolean on_off;
		unsigned long last_dim=0;
		short dim_direction=1;
		boolean dimming;

		int normalize(int value, int max_value);
		int value_to_pwm(int value, int max_value);
    
    	void update();
};

#endif // Included_SmartIO_DimmableLed_H