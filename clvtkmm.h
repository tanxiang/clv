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
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
private:
	void draw_rectangle(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);
	void draw_text(const Cairo::RefPtr<Cairo::Context>& cr, int rectangle_width, int rectangle_height);
};

//We derive our own class from PrintOperation,
//so we can put the actual print implementation here.
/*
class PrintFormOperation : public Gtk::PrintOperation
{
public:
	static Glib::RefPtr<PrintFormOperation> create();
	virtual ~PrintFormOperation(){};
	void set_name(const Glib::ustring& name) { m_Name = name; }
	void set_comments(const Glib::ustring& comments) { m_Comments = comments; }
protected:
	PrintFormOperation(){};
	//PrintOperation default signal handler overrides:
	void on_begin_print(const Glib::RefPtr<Gtk::PrintContext>& context);
	void on_draw_page(const Glib::RefPtr<Gtk::PrintContext>& context, int page_nr);
	Glib::ustring m_Name;
	Glib::ustring m_Comments;
	Glib::RefPtr<Pango::Layout> m_refLayout;
	std::vector<int> m_PageBreaks; // line numbers where a page break occurs
};
*/

class clvtkwindow : public Gtk::Window
{

public:
	clvtkwindow(){};
	virtual ~clvtkwindow(){}
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

#endif // GTKMM_EXAMPLE_HELLOWORLD_H
