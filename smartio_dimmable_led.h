#ifndef Included_SmartIO_DimmableLed_H
#define Included_SmartIO_DimmableLed_H

#include <smartio_main_loop.h>

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
	protected:
		int dim_value=1023;
		boolean on_off=false;
		unsigned long last_dim=0;
		short dim_direction=1;
		boolean dimming=false;

		boolean perform_dim();
		int normalize(int value, int max_value);
		int value_to_pwm(int value, int max_value);
    
    	void update();
    	virtual void dim_to(int dim_value)=0;

// Arduino dims from 0...255, ESP8266 from 0..1023
#if defined(ESP8266)
    	const int pwm_max_value=1023;
#else
    	const int pwm_max_value=255;
#endif

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