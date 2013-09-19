#ifndef _CLVVIEW_H
#define _CLVVIEW_H
#include <gtkmm.h>
#include "unorder_tree.h"
#include "clvline.h"
class ClvLineArea : public Gtk::DrawingArea{
public:
protected:
	//Override default signal handler:
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
};

class ClvFileArea: public Gtk::DrawingArea{
	unorder_tree<line> &file_context;
	Pango::TabArray tab_width;
	
	GtkIMContext *im_context;
	
	Pango::FontDescription get_gnome_document_font_description();
 	void on_font_setting_changed (const Glib::ustring & key);
 	void update_custom_font_setting();
 	void modify_font_from_string (const std::string & fontString);
 	bool key_pressed (GdkEventKey * ev);
 	bool button_pressed (GdkEventButton * ev);
public:
	void draw(const Cairo::RefPtr<Cairo::Context>& cr,const Cairo::Rectangle &rect);
	ClvFileArea(unorder_tree<line> &file);
	virtual ~ClvFileArea(){
		g_object_unref (im_context);
	};
	
protected:
	bool on_draw(const ::Cairo::RefPtr< ::Cairo::Context>& cr) override;
	bool on_key_press_event(GdkEventKey* event) override;
	bool on_key_release_event(GdkEventKey* event) override;
	bool on_button_press_event(GdkEventButton* event) override;
	bool on_button_release_event(GdkEventButton* event) override;
	//bool on_scroll_event(GdkEventScroll* event) override;
};

class ClvThumArea : public Gtk::DrawingArea{
public:
	ClvThumArea();
	virtual ~ClvThumArea();
protected:
	//Override default signal handler:
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
};

#endif
