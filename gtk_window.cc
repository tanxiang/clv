#include "clvtkmm.h"

int main(int argc, char *argv[])
{
	Gtk::Main kit(argc, argv);
	clvtkwindow window;
	Gtk::Main::run(window);
	return EXIT_SUCCESS;
}
