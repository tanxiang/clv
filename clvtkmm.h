#ifndef _CLVTKMM_H
#define _CLVTKMM_H
#include <gtkmm.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <glibmm/ustring.h>
#include <gtkmm/drawingarea.h>
#include <vector>
#include <memory>

class ClvLineArea : public Gtk::DrawingArea{
public:
protected:
};

class ClvFileArea : public Gtk::DrawingArea
{

	void draw_rectangle(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);
	void draw_text(const Cairo::RefPtr<Cairo::Context>& cr, int rectangle_width, int rectangle_height);
public:
	ClvFileArea(){};
	virtual ~ClvFileArea(){};
protected:
	//Override default signal handler:
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
};

class ClvThumArea : public Gtk::DrawingArea{
public:
protected:
};

class ClvFViewBox : public Gtk::HBox{
	Glib::ustring filename;
	ClvLineArea lineview;
	ClvFileArea fileview;
	ClvThumArea thumview;
public:
	ClvFViewBox(Glib::ustring fs="");
	virtual ~ClvFViewBox();
};

class ClvtkWindow : public Gtk::Window
{
	Gtk::VBox main_box;
	Gtk::Notebook flist_notebook;
	std::vector<std::unique_ptr<ClvFViewBox> > fview_boxs;
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
