#ifndef Included_SmartIO_MainLoop_H
#define Included_SmartIO_MainLoop_H

#include <vector.h>

class MainLoop;
class MainLoopListener {
	public:
		MainLoopListener();
		virtual void loop()=0;
};

class MainLoop {
	public:
		static const void loop() {
			for (int i=0;i<listeners.size();i++) {
				listeners[i]->loop();
			}
		}
		static void add(MainLoopListener* listener) {
			listeners.push_back(listener);
		}
		static Vector<MainLoopListener*> listeners;
};

#endif // Included_SmartIO_MainLoop_H