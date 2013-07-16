#ifndef _CLVTKMM_H
#define _CLVTKMM_H
#include <gtkmm.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <glibmm/ustring.h>
#include <gtkmm/drawingarea.h>
#include <vector>
#include <memory>
#include "filemap.h"


class ClvLineArea : public Gtk::DrawingArea{
public:
protected:
	//Override default signal handler:
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
};

class ClvFileArea : public Gtk::TextView{
	FileMap<MBLineRef<CharRef> > &file;

	Pango::FontDescription get_gnome_document_font_description();
 	void on_font_setting_changed (const Glib::ustring & key);
 	void update_custom_font_setting();
 	void modify_font_from_string (const std::string & fontString);
 	bool key_pressed (GdkEventKey * ev);
 	bool button_pressed (GdkEventButton * ev);
public:
	ClvFileArea(FileMap<MBLineRef<CharRef> > &file_ref,const Glib::RefPtr<Gtk::TextBuffer> buffer);
	virtual ~ClvFileArea(){};
	
	/** Sets the default tab stops for paragraphs in @a text_view.
	* Tags in the buffer may override the default.
	* @param tabs Tabs as a Pango::TabArray.
	*/
	//void set_tabs(Pango::TabArray& tabs);
	//Pango::TabArray get_tabs() const;
	//bool im_context_filter_keypress (GdkEventKey* event);
protected:

	/*
	virtual void on_drag_data_received (Glib::RefPtr<Gdk::DragContext> & context,
	int x, int y,
	const Gtk::SelectionData & selection_data,
	guint info,  guint time);
	*/
	//Override default signal handler:
};

class ClvThumArea : public Gtk::DrawingArea{
public:
protected:
	//Override default signal handler:
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
};

class ClvFViewBox : public Gtk::Box,
	public Gtk::Scrollable{
	FileMap<MBLineRef<CharRef> > &file;
	ClvLineArea lineview;
	ClvFileArea fileview;
	ClvThumArea thumview;
	Gtk::ScrolledWindow scrolledview;
public:
	ClvFViewBox(FileMap<MBLineRef<CharRef> > &file_ref);
	virtual ~ClvFViewBox(){};
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

	//Gtk::Image cpp_icon;
	//Gtk::Image c_icon;
	//Gtk::Image h_icon;
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
	//void print_or_preview(Gtk::PrintOperationAction print_action);
	//PrintOperation signal handlers.
	//We handle these so can get necessary information to update the UI or print settings.
	//Our derived PrintOperation class also overrides some default signal handlers.
	//void on_printoperation_status_changed(const Glib::RefPtr<PrintFormOperation>& operation);
	//void on_printoperation_done(Gtk::PrintOperationResult result, const Glib::RefPtr<PrintFormOperation>& operation);

	//Signal handlers:
	//void on_button_clicked();
	//Member widgets:
	//Gtk::Button m_button;
};

#endif // _CLVTKMM_H
