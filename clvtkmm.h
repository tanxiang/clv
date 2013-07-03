#ifndef _CLVTKMM_H
#define _CLVTKMM_H
#include <gtkmm.h>
#include <pangomm.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>
class clvtkwindow : public Gtk::Window
{

public:
	clvtkwindow();
	virtual ~clvtkwindow(){}
protected:
	//Signal handlers:
	void on_button_clicked();
	//Member widgets:
	Gtk::Button m_button;
};

//We derive our own class from PrintOperation,
//so we can put the actual print implementation here.
class PrintFormOperation : public Gtk::PrintOperation
{
public:
	static Glib::RefPtr<PrintFormOperation> create();
	virtual ~PrintFormOperation();
	void set_name(const Glib::ustring& name) { m_Name = name; }
	void set_comments(const Glib::ustring& comments) { m_Comments = comments; }
protected:
	PrintFormOperation();
	//PrintOperation default signal handler overrides:
	virtual void on_begin_print(const Glib::RefPtr<Gtk::PrintContext>& context);
	virtual void on_draw_page(const Glib::RefPtr<Gtk::PrintContext>& context, int page_nr);
	Glib::ustring m_Name;
	Glib::ustring m_Comments;
	Glib::RefPtr<Pango::Layout> m_refLayout;
	std::vector<int> m_PageBreaks; // line numbers where a page break occurs
};
#endif // GTKMM_EXAMPLE_HELLOWORLD_H
