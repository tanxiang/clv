#include "clvtkmm.h"
#include <gtkmm/application.h>

int main(int argc, char *argv[]){
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.ttand.clv");
	ClvtkWindow window;
	window.set_title("clv");
	return app->run(window);
}
