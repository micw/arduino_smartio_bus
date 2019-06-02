#ifndef Included_SmartIO_DimmableLed_H
#define Included_SmartIO_DimmableLed_H

#include <smartio_main_loop.h>

#if !defined(PWMRANGE)
#define PWMRANGE 255
#endif

/**
 * Base class for pwm dimmable LEDs
 * A pwm-dimmable led. Pwm dim range is from 0...1023.
 * This value is gamma-corrected (so that 511 is ~50% of visible brightness)
 * and normalized to the device's dim range (0..255 on arduino, 0...1023 on esp8266)
 */
class DimmableLedBase : MainLoopListener {
	public:
		void toggle();
		void dim_start();
		void dim_stop();
		void loop();

		bool get_state();
		void set_state(bool on_off);

		void set_dim_value(int dim_value);
		int get_dim_value();

	protected:
		int dim_value=700;
		boolean on_off=false;
		unsigned long last_dim=0;
		short dim_direction=1;
		boolean dimming=false;

		boolean perform_dim();
		int normalize(int value, int max_value);
		int value_to_pwm(int value, int max_value);
    
    	void update();
    	virtual void dim_to(int dim_value)=0;

    	const int pwm_max_value=PWMRANGE;

};


/**
 * A pwm-dimmable led. Pwm dim range is from 0...1023.
 * This value is gamma-corrected (so that 511 is ~50% of visible brightness)
 * and normalized to the device's dim range (0..255 on arduino, 0...1023 on esp8266)
 */
class DimmableLed : public DimmableLedBase {
	public:
		DimmableLed(unsigned int pin);
	protected:
		void dim_to(int dim_value);
		unsigned int pin;
};

#endif // Included_SmartIO_DimmableLed_H