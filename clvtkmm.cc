#include "clvtkmm.h"
#include <pangomm.h>
#include <iostream>

void draw_rectangle(const Cairo::RefPtr<Cairo::Context>& cr,
                            int width, int height){
  //FIXME color conf
	cr->set_source_rgb(0.0, 0.0, 0.0);
	cr->rectangle(0, 0, width, height);
	cr->fill();
}

bool ClvLineArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	Gtk::Allocation allocation = get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();
	//draw_rectangle(cr, width, height);
	return true;
}

bool ClvFileArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	Gtk::Allocation allocation = get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();

	draw_rectangle(cr, width, height);
	draw_text(cr, width, height);
	return true;
}

void ClvFileArea::draw_text(const Cairo::RefPtr<Cairo::Context>& cr,
                       int rectangle_width, int rectangle_height){
	//FIXME need conf
	cr->set_source_rgb(1.0, 1.0, 1.0);
	Pango::FontDescription font;
	font.set_family("Source Code Pro");
	font.set_weight(Pango::WEIGHT_BOLD);
	Glib::RefPtr<Pango::Layout> layout = create_pango_layout("Hi there!");

	layout->set_font_description(font);

	int text_width, text_height;

	//get the text dimensions (it updates the variables -- by reference)
	layout->get_pixel_size(text_width, text_height);

	// Position the text in the middle
	cr->move_to((rectangle_width-text_width)/2, (rectangle_height-text_height)/2);

	layout->show_in_cairo_context(cr);
}

bool ClvThumArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	Gtk::Allocation allocation = get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();
	//draw_rectangle(cr, width, height);
	return true;
}

ClvFViewBox::ClvFViewBox(Glib::ustring fs):filename(fs){
	//set_homogeneous(false);
	//lineview.queue_draw_area(0,0,30,30);
	//FIXME:reconfig w,h by font w,h
	set_size_request(500,400);
	lineview.set_size_request(50);
	pack_start(lineview,Gtk::PACK_SHRINK);

	pack_start(fileview);

	thumview.set_size_request(100);
	pack_start(thumview,Gtk::PACK_SHRINK);

}

ClvFViewBox::~ClvFViewBox(){
		std::cout<<"free vbox"<<std::endl;
}

ClvtkWindow::ClvtkWindow(){
	add(main_box);
	main_box.pack_start(flist_notebook);
	fview_boxs.push_back(std::unique_ptr<ClvFViewBox>{new ClvFViewBox{}});
	flist_notebook.prepend_page(*fview_boxs[0], "*UnSaved");

	//fview_boxs[0]->show();
	//flist_notebook.show();
	show_all_children();
};