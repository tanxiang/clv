#include "clvtkmm.h"
#include <gtkmm/application.h>

int main(int argc, char *argv[]){
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.ttand.clv");
	char *fnv[20];
	//ClvtkWindow window{std::vector<std::string>{"../Desktop/t.cc","../Desktop/t.c"}};
	ClvtkWindow window{std::vector<std::string>{"../Desktop/t.cc"}};
	window.set_title("clv");
	return app->run(window);
}
