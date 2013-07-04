#ifndef _CLVTKMM_H
#define _CLVTKMM_H
#include <gtkmm.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>

#include <gtkmm/drawingarea.h>

class ClvArea : public Gtk::DrawingArea
{
public:
	ClvArea(){};
	virtual ~ClvArea(){};
protected:
	//Override default signal handler:
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
private:
	void draw_rectangle(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);
	void draw_text(const Cairo::RefPtr<Cairo::Context>& cr, int rectangle_width, int rectangle_height);
};


class ClvtkWindow : public Gtk::Window
{
	Gtk::VBox main_box;
	Gtk::Notebook flist_notebook;
	Gtk::HBox fview_box;
public:
	ClvtkWindow();
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
