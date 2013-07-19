#include "clvview.h"

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
	draw_rectangle(cr, width, height);
	return true;
}

ClvFileArea::ClvFileArea(FileMap<MBLineRef<CharRef> > &file_ref,const Glib::RefPtr<Gtk::TextBuffer> buffer):
	file(file_ref),Gtk::TextView(buffer){
	//std::cout<<static_cast<char*>(file.Get());
	get_style_context();
	override_background_color(Gdk::RGBA{"black"});
	auto pbuffer = get_buffer();
	pbuffer->set_text(static_cast<char*>(file.Get()));

	pbuffer->apply_tag_by_name("normal",pbuffer->begin(),pbuffer->end());
	//pbuffer->apply_tag_by_name("var:composite",pbuffer->begin(),pbuffer->end());
}

bool ClvThumArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	Gtk::Allocation allocation = get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();
	draw_rectangle(cr, width, height);
	return true;
}