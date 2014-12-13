#pragma once
#include <gtkmm.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <glibmm/ustring.h>
#include <gtkmm/drawingarea.h>
#include <vector>
#include <memory>
#include <fstream>
#include "clvview.h"
#include "filemap.h"

namespace clv{
class FViewBox :public Gtk::Container,virtual public Gtk::Scrollable {
	clv::context<clv::line> &file;
	LineArea line_view;
	FileArea edit_view;
	//Glib::RefPtr<Gdk::Window> window;

	Cairo::RefPtr<Cairo::Surface> backing_surface_ptr;
	int backing_surface_x,backing_surface_y,backing_surface_w,backing_surface_h;
	static int extra_width,extra_height;

protected:
	void on_realize() override;
	void on_unrealize() override;
	Gtk::SizeRequestMode get_request_mode_vfunc() const override{
		return Gtk::SIZE_REQUEST_HEIGHT_FOR_WIDTH;
	}
	void get_preferred_width_vfunc(int& minimum_width, int& natural_width) const override{
		minimum_width=300;natural_width=1000;
	}
	void get_preferred_height_for_width_vfunc(int width, int& minimum_height, int& natural_height) const override{
		get_preferred_height_vfunc(minimum_height,natural_height);
	}
	void get_preferred_height_vfunc(int& minimum_height, int& natural_height) const override{
		minimum_height=200;natural_height=700;
	}
	void get_preferred_width_for_height_vfunc(int height, int& minimum_width, int& natural_width) const override{
		get_preferred_width_vfunc(minimum_width,natural_width);
	}
	GType child_type_vfunc() const override{
		return Gtk::Widget::get_type();
	}
	void on_size_allocate(Gtk::Allocation& allocation) override;
	void on_add(Gtk::Widget* widget) override{
		//Gtk::Container::on_add(widget);
	}
	virtual void on_remove(Gtk::Widget* child) override{

	}
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

	void forall_vfunc(gboolean include_internals, GtkCallback callback, gpointer callback_data) override;
	//GType child_type_vfunc() const override;
	void on_hadjustment();
	void on_vadjustment();
public:
	FViewBox(clv::context<clv::line> &file_ref);
	virtual ~FViewBox(){};
	void save();
	void close();
	void set_activates(bool setting=false){
		edit_view.set_activates(setting);
	}
};

class ToolBox : public Gtk::Box{
	Gtk::ComboBoxText tab_size;
	Gtk::Button file_mode;
	Gtk::Button search;
	Gtk::Button options;
public:
	ToolBox();
	virtual ~ToolBox(){};
};

class PageBox : public Gtk::Box{
	Glib::ustring file_name;
	std::fstream file_stream;
	clv::FileMap<clv::RawLineRef<char>> file_io_map;
	clv::context<clv::line> file_context;
	Gtk::Box main_view;
	//Gtk::DrawingArea head_view;
	Gtk::ScrolledWindow scrolled_content_view;
	FViewBox content_view;
	ThumArea thumb_view;

	ToolBox tool_bar;
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
	PageBox(Glib::ustring fs="");
	virtual ~PageBox(){};
	Gtk::Widget& get_tab_box(){
		return tab_box;
	}
	void set_activates(bool setting=false){
		content_view.set_activates(setting);
	}
	//static void setup_icons();
};

class Notebook : public Gtk::Notebook{
	std::vector<std::unique_ptr<PageBox> > f_boxs;
	PageBox *current_page;
public:
	Notebook(std::vector<std::string> &fn);
	virtual ~Notebook(){
		while(get_n_pages()){
			remove_page();
		}
	};
protected:
	void on_page_switch ( Gtk::Widget *page, int page_num );
};

class tkWindow : public Gtk::Window{
	Notebook flist_notebook;
	bool on_key_release_event(GdkEventKey* event) override;
public:
	tkWindow(std::vector<std::string> fn = std::vector<std::string>{});
	virtual ~tkWindow(){}
protected:
	//void build_main_menu();
};
};//namespace
