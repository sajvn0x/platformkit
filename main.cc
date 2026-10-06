#include <x11/x11_window.hh>

int main() {
	WindowSystemX11* ws = new WindowSystemX11();
	ws->process_events(); // loop
}

