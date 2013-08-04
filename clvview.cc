#include "clvview.h"

bool ClvLineArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	if(should_draw_window(cr,get_window())){
		cr->set_source_rgb(0.0, 0.0, 0.0);
		cr->paint();
	}
	return true;
}

ClvFileArea::ClvFileArea(FileMap<MBLineRef<CharRef> > &file_ref):
	file(file_ref){
	auto style = get_style_context();
	//style->set_background();
	//style->render_background(cr, 0, 0, 0, 0)
	//override_cursor(Gdk::RGBA{"green"},Gdk::RGBA{"blue"});
	//auto pbuffer = get_buffer();
	//pbuffer->set_text(static_cast<char*>(file.Get()));

	//pbuffer->apply_tag_by_name("normal",pbuffer->begin(),pbuffer->end());
#ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
	this->signal_draw().connect(sigc::mem_fun(*this,&ClvFileArea::on_draw));
#endif
	//pbuffer->apply_tag_by_name("var:composite",pbuffer->begin(),pbuffer->end());
}

#include <iostream>
bool ClvFileArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	//auto window = get_window(Gtk::TEXT_WINDOW_TEXT);
	cr->save();
		cr->set_source_rgb(0, 0, 0);
		cr->paint();
		//cr->stroke();
#if 1
		std::vector<Cairo::Rectangle> clip_rects;
		cr->copy_clip_rectangle_list(clip_rects);
		std::cerr<<"num rect = "<<clip_rects.size()<<std::endl;
		for(auto& clip_rect:clip_rects){
			std::cerr<<"\tw="<<clip_rect.width<<" h="<<clip_rect.height<<std::endl;
		}
#endif
	return true;//Gtk::TextView::on_draw(cr);
}

ClvThumArea::ClvThumArea(){

}

ClvThumArea::~ClvThumArea(){

}

bool ClvThumArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	return true;
}

