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
	//property_background_rgba() ;
	override_color(Gdk::RGBA{"yellow"});
	override_background_color(Gdk::RGBA{"black"});
	override_cursor(Gdk::RGBA{"green"},Gdk::RGBA{"blue"});
	auto pbuffer = get_buffer();
	pbuffer->set_text(static_cast<char*>(file.Get()));

	pbuffer->apply_tag_by_name("normal",pbuffer->begin(),pbuffer->end());
	//pbuffer->apply_tag_by_name("var:composite",pbuffer->begin(),pbuffer->end());
}
#include <iostream>
bool ClvFileArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	auto window = get_window(Gtk::TEXT_WINDOW_TEXT);
	cr->save();
	transform_cairo_context_to_window(cr,window);
	if(should_draw_window(cr,window)){
		std::cerr<<"should_draw_window\n";
		Gdk::Rectangle visible_rect;
		Gtk::TextBuffer::iterator text_itr_start,text_itr_end;
		int top_itr_start=0,top_itr_end=0;
		get_visible_rect(visible_rect);
		get_line_at_y(text_itr_start,visible_rect.get_y(),top_itr_start);
		get_line_at_y(text_itr_end,visible_rect.get_y()+visible_rect.get_height(),top_itr_end);

	}
	return Gtk::TextView::on_draw(cr);
}

bool ClvThumArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	Gtk::Allocation allocation = get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();
	draw_rectangle(cr, width, height);
	return true;
}