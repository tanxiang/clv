#ifndef _CLVTKMM_H
#define _CLVTKMM_H
#include <gtkmm.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <glibmm/ustring.h>
#include <gtkmm/drawingarea.h>
#include <vector>
#include <memory>
#include <fstream>
#include "clvview.h"

class ClvFViewBox :public Gtk::Container,virtual public Gtk::Scrollable {
	context<line> &file;
	ClvLineArea line_view;
	ClvFileArea file_view;
	//Glib::RefPtr<Gdk::Window> window;
protected:
	//void add(Widget& widget) override{}
	void on_realize() override;
	void on_unrealize() override;
	void on_size_allocate(Gtk::Allocation& allocation) override;

	void on_hadjustment();
	void on_vadjustment();
public:
	ClvFViewBox(context<line> &file_ref);
	virtual ~ClvFViewBox(){};
	void save();
	void close();
	void set_activates(bool setting=false){
		file_view.set_activates(setting);
	}
};

class ClvToolBox : public Gtk::Box{
	Gtk::ComboBoxText tab_size;
	Gtk::Button file_mode;
	Gtk::Button search;
	Gtk::Button options;
public:
	ClvToolBox();
	virtual ~ClvToolBox(){};
};

class ClvPageBox : public Gtk::Box{
	Glib::ustring file_name;
	std::fstream file_stream;
	context<line> file_context;
	Gtk::Box main_view;
	//Gtk::DrawingArea head_view;
	Gtk::ScrolledWindow scrolled_content_view;
	ClvFViewBox content_view;
	ClvThumArea thumb_view;

	ClvToolBox tool_bar;
	Gtk::Image save_icon;
	Gtk::Button bt_save;
	Gtk::Image close_icon;
	Gtk::Button bt_close;
	Gtk::Label tab_label;
	Gtk::Box tab_box;
	//Gtk::ScrolledWindow scrolled_view;
protected:
	void on_realize() override;
	void on_unrealize() override;
	//bool on_event(GdkEvent *event) override;
public:
	ClvPageBox(Glib::ustring fs="");
	virtual ~ClvPageBox(){};
	Gtk::Widget& get_tab_box(){
		return tab_box;
	}
	void set_activates(bool setting=false){
		content_view.set_activates(setting);
	}
	//static void setup_icons();
};

class ClvNotebook : public Gtk::Notebook{
	std::vector<std::unique_ptr<ClvPageBox> > f_boxs;
	ClvPageBox *current_page;
public:
	ClvNotebook(std::vector<std::string> &fn);
	virtual ~ClvNotebook(){
		while(get_n_pages()){
			remove_page();
		}
	};
protected:
	void on_page_switch ( Gtk::Widget *page, int page_num );
};

class ClvtkWindow : public Gtk::Window{
	ClvNotebook flist_notebook;
	bool on_key_release_event(GdkEventKey* event) override;
public:
	ClvtkWindow(std::vector<std::string> fn = std::vector<std::string>{});
	virtual ~ClvtkWindow(){}
protected:
	//void build_main_menu();
};

#endif // _CLVTKMM_H
