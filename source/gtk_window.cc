#include "clvtkmm.h"
#include <gtkmm/application.h>

int main(int argc, char *argv[]){
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.ttand.clv");
	//if(!Glib::thread_supported()) Glib::thread_init();
	clv::tkWindow window{std::vector<std::string>{"source/view.cc"}};
	//ClvtkWindow window{std::vector<std::string>{"../Desktop/t.cc"}};
	window.set_title("clv");
	return app->run(window,argc,argv);
}
