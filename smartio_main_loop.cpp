#include <smartio_main_loop.h>

MainLoopListener::MainLoopListener() {
	MainLoop::add(this);
}

Vector<MainLoopListener*> MainLoop::listeners;
