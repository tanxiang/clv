#pragma once
#include <gtkmm.h>
#include "clvcontext.h"
#include "clvline.h"
#include <chrono>

namespace clv{

class LineArea : public Gtk::DrawingArea{
public:
protected:
	//Override default signal handler:
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
};

class FileArea: public Gtk::DrawingArea{
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
	clv::context<clv::line> &file_context;
	
	GtkIMContext *im_context = gtk_im_multicontext_new();
	Cairo::RefPtr<Cairo::Surface> glyphs_surface_ptr;
	sigc::connection blink_time_out;
	std::chrono::steady_clock::time_point refresh_time = std::chrono::steady_clock::now();
	
 	void on_font_setting_changed (const Glib::ustring & key);
 	void update_custom_font_setting();
 	void modify_font_from_string (const std::string & fontString);
	void draw_rect(const Cairo::RefPtr<Cairo::Context>& cr,const Cairo::Rectangle &rect);
public:

	void set_activates(bool setting=false);
	FileArea(clv::context<clv::line> &file);
	virtual ~FileArea();
	
protected:

	static void im_commit_proxy(GtkIMContext *context,const gchar *str,FileArea* pobj);
	void im_commit(GtkIMContext *context,const gchar *str);

	static void preedit_changed_proxy(GtkIMContext *context,FileArea* pobj);
	void preedit_changed(GtkIMContext *context);
	
	static void retrieve_surrounding_proxy(GtkIMContext *context,FileArea* pobj);
	
	static void delete_surrounding_proxy(GtkIMContext *context,gint offset,gint n_chars,FileArea* pobj);

	bool on_timer();
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

class ThumArea : public Gtk::DrawingArea{
public:
	ThumArea();
	virtual ~ThumArea();
protected:
	//Override default signal handler:
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
};
};//namespace
