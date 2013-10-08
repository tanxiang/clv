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
	enum{
		STATUS_NONE,
		STATUS_NORMAL,
		STATUS_SELECTED,
		STATUS_S_CLICKED,
		STATUS_D_CLICKED,
		STATUS_R_CLICKED,
		STATUS_R_MENU,
		STATUS_M_CLICKED,
		STATUS_INPUT,
		STATUS_DELETE,
		STATUS_ENTER,
		STATUS_CMD,
	}input_status;
	unorder_tree<line> &file_context;
	
	GtkIMContext *im_context;
	
 	void on_font_setting_changed (const Glib::ustring & key);
 	void update_custom_font_setting();
 	void modify_font_from_string (const std::string & fontString);
public:
	void draw(const Cairo::RefPtr<Cairo::Context>& cr,const Cairo::Rectangle &rect);

	ClvFileArea(unorder_tree<line> &file);
	virtual ~ClvFileArea(){
		g_object_unref (im_context);
	};
	
protected:

	static void im_commit_proxy(GtkIMContext *context,const gchar *str,ClvFileArea* pobj);
	void im_commit(GtkIMContext *context,const gchar *str);

	static void preedit_changed_proxy(GtkIMContext *context,ClvFileArea* pobj);
	void preedit_changed(GtkIMContext *context);
	
	static void retrieve_surrounding_proxy(GtkIMContext *context,ClvFileArea* pobj);
	
	static void delete_surrounding_proxy(GtkIMContext *context,gint offset,gint n_chars,ClvFileArea* pobj);

	bool on_focus_in_event(GdkEventFocus* event) override;
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
	bool on_key_press_event(GdkEventKey* event) override;
	bool on_key_release_event(GdkEventKey* event) override;
	bool on_button_press_event(GdkEventButton* event) override;
	bool on_button_release_event(GdkEventButton* event) override;
	//bool on_scroll_event(GdkEventScroll* event) override;
	void on_realize() override;
	void on_unrealize() override;
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
