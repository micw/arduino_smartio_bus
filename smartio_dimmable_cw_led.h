#ifndef Included_SmartIO_DimmableCWLed_H
#define Included_SmartIO_DimmableCWLed_H

#include <smartio_dimmable_led.h>

/**
 * Dimmable control for dual-white (cold/warm white) leds
 */
class DimmableCWLed : public DimmableLedBase {
	public:
		DimmableCWLed(unsigned int pin1, unsigned int pin2);
		void fade_start();
		void fade_stop();
		void loop();

		void set_fade_value(int fade_value);
		int get_fade_value();
	protected:
		boolean perform_fade();
		void dim_to(int dim_value);

		unsigned int pin1;
		unsigned int pin2;
		int fade_value=511;
		short fade_direction=1;
		boolean fading=false;
		unsigned long last_fade=0;
};

#endif // Included_SmartIO_DimmableCWLed_H