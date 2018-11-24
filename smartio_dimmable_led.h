#ifndef Included_SmartIO_DimmableLed_H
#define Included_SmartIO_DimmableLed_H

#include <smartio_main_loop.h>

class DimmableLed : MainLoopListener {
	public:
		DimmableLed(unsigned int pin);
		void toggle();
		void dim_start();
		void dim_stop();
		void loop();
	private:
		unsigned int pin;
		int pwm_value;
		boolean on_off;
		unsigned long last_dim=0;
		short dim_direction=1;
		boolean dimming;
    
    	void update();
};

#endif // Included_SmartIO_DimmableLed_H