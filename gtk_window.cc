#include "clvtkmm.h"
#include <gtkmm/application.h>

int main(int argc, char *argv[]){
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.gtkmm.example");
	clvtkwindow window;
	window.set_title("Drawing text example");
	ClvArea area;
	window.add(area);
	area.show();
	return app->run(window);
}
