#ifndef _CLVVIEW_H
#define _CLVVIEW_H
#include <gtkmm.h>
#include "filemap.h"

class ClvLineArea : public Gtk::DrawingArea{
public:
protected:
	//Override default signal handler:
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
};

class ClvFileArea : public Gtk::TextView{
	FileMap<MBLineRef<CharRef> > &file;
	Pango::TabArray tab_width;

	Pango::FontDescription get_gnome_document_font_description();
 	void on_font_setting_changed (const Glib::ustring & key);
 	void update_custom_font_setting();
 	void modify_font_from_string (const std::string & fontString);
 	bool key_pressed (GdkEventKey * ev);
 	bool button_pressed (GdkEventButton * ev);
public:
	ClvFileArea(FileMap<MBLineRef<CharRef> > &file_ref,const Glib::RefPtr<Gtk::TextBuffer> buffer);
	virtual ~ClvFileArea(){};
	
protected:
	bool on_draw(const ::Cairo::RefPtr< ::Cairo::Context>& cr) override;
};

class ClvThumArea : public Gtk::DrawingArea{
public:
protected:
	//Override default signal handler:
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
};

#endif