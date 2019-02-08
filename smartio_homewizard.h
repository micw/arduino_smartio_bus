#ifndef Included_SmartIO_Homewizard_H
#define Included_SmartIO_Homewizard_H

#include <smartio_main_loop.h>
#include <smartio_bus.h>
#include <ext_HomeWizard.h>

class HomewizardReceiver : MainLoopListener {
	public:
		static const char TYPE_BUTTON='h';
		static const int ACTION_BUTTON_OFF=0;
		static const int ACTION_BUTTON_ON=1;

		HomewizardReceiver(Bus* bus, unsigned int pin);

	private:
		Bus* bus;
		unsigned int pin; // pin of this receiver
		HW_DATAGRAM data;
		void loop();
};

#endif // Included_SmartIO_Homewizard_H
