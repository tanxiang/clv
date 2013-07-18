#ifndef _CLVTKMM_H
#define _CLVTKMM_H
#include <gtkmm.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <glibmm/ustring.h>
#include <gtkmm/drawingarea.h>
#include <vector>
#include <memory>
#include "clvview.h"

class ClvFViewBox : public Gtk::Box{
	FileMap<MBLineRef<CharRef> > &file;
	ClvLineArea lineview;
	ClvFileArea fileview;
	ClvThumArea thumview;
	Gtk::ScrolledWindow scrolledview;
public:
	ClvFViewBox(FileMap<MBLineRef<CharRef> > &file_ref);
	virtual ~ClvFViewBox(){};
	void save();
	void close();
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

class ClvFileBox : public Gtk::Box{
	Glib::ustring filename;
	FileMap<MBLineRef<CharRef> > file;
	ClvFViewBox view;
	ClvToolBox tool_bar;
	Gtk::Image save_icon;
	Gtk::Button bt_save;
	Gtk::Image close_icon;
	Gtk::Button bt_close;
	Gtk::Label tab_label;
	Gtk::Box tab_box;
public:
	ClvFileBox(Glib::ustring fs="");
	virtual ~ClvFileBox(){};
	Gtk::Widget& get_tab_box(){
		return tab_box;
	}
	//static void setup_icons();
};

class ClvNotebook : public Gtk::Notebook{
	std::vector<std::unique_ptr<ClvFileBox> > f_boxs;

public:
	ClvNotebook(std::vector<std::string> fn);
	virtual ~ClvNotebook(){
		while(get_n_pages()){
			remove_page();
		}
	};
};

class ClvtkWindow : public Gtk::Window{
	ClvNotebook flist_notebook;
public:
	ClvtkWindow(std::vector<std::string> fn = std::vector<std::string>{});
	virtual ~ClvtkWindow(){}
protected:
	//void build_main_menu();
};

#endif // _CLVTKMM_H
