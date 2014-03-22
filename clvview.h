#ifndef _CLVVIEW_H
#define _CLVVIEW_H
#include <gtkmm.h>
#include "clvcontext.h"
#include "clvline.h"

class ClvLineArea : public Gtk::DrawingArea{
public:
protected:
	//Override default signal handler:
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
};

class ClvFileArea: public Gtk::DrawingArea{
	enum{
		STATUS_NONE=0,
		STATUS_NORMAL,
		STATUS_SELECTED,
		STATUS_S_CLICKED,
		STATUS_D_CLICKED,
		STATUS_R_CLICKED,
		STATUS_R_MENU,
		STATUS_M_CLICKED,
#ifdef CLV_RKET_EVENT
		STATUS_KEY_PRESS,
#endif
		STATUS_INPUT,
		STATUS_DELETE,
		STATUS_CMD,
	}input_status;
	context<line> &file_context;
	
	GtkIMContext *im_context;

#ifdef CLV_SURFACE_BLINK
	Cairo::RefPtr<Cairo::Surface> surface_ptr;
#endif

	Cairo::RefPtr<Cairo::Surface> cover_surface_ptr;
	
	sigc::connection blink_time_out;
	
 	void on_font_setting_changed (const Glib::ustring & key);
 	void update_custom_font_setting();
 	void modify_font_from_string (const std::string & fontString);
	void draw_rect(const Cairo::RefPtr<Cairo::Context>& cr,const Cairo::Rectangle &rect);
public:

	void set_activates(bool setting=false);
	ClvFileArea(context<line> &file);
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

	bool on_blink_time();
	bool on_configure_event(GdkEventConfigure* event) override;
	void on_size_allocate(Gtk::Allocation& allocation) override;
	bool on_focus_in_event(GdkEventFocus* event) override;
	bool on_focus_out_event(GdkEventFocus* event) override;
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
	bool on_key_press_event(GdkEventKey* event) override;
#ifdef CLV_RKET_EVENT
	bool on_key_release_event(GdkEventKey* event) override;
#endif
	bool on_button_press_event(GdkEventButton* event) override;
	bool on_button_release_event(GdkEventButton* event) override;
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
