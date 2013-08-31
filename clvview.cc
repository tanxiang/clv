#include "clvview.h"

bool ClvLineArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	if(should_draw_window(cr,get_window())){
		cr->set_source_rgb(0.0, 0.0, 0.0);
		cr->paint();
	}
	return true;
}

ClvFileArea::ClvFileArea(unorder_tree<line> &file):
	file_context(file){
	//auto style = get_style_context();
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

void ClvFileArea::draw(const Cairo::RefPtr<Cairo::Context>& cr,const Cairo::Rectangle &rect){
	cr->save();//need RAII mode restore
	cr->set_source_rgb(1,1,1);
	cr->select_font_face("Source Code Pro",Cairo::FONT_SLANT_NORMAL,Cairo::FONT_WEIGHT_NORMAL );
	auto itr = file_context.get_form_fill(rect.y);
	//file_context.dump();
	int height = 0;
	cr->set_font_size(16);
	auto scaled_font = cr->get_scaled_font();
	std::vector<Cairo::Glyph> glyphs;
	std::vector<Cairo::TextCluster> text_clusters;
	Cairo::TextClusterFlags flags;
	while(height<rect.height && itr != file_context.end())
	{
		//std::cout<<*itr<<std::endl;
		//cr->move_to(0,itr.get_fill_offset()+itr->get_fill());//y can self-add by loop
		//cr->show_text(*itr);
		//std::cout<<"fill offset:"<<itr.get_fill_offset()<<std::endl;
		scaled_font->text_to_glyphs(0,itr.get_fill_offset()+itr->get_fill(), *itr, glyphs, text_clusters , flags);

		for(auto& glyph : glyphs){
			std::cout<<glyph.index<<' ';
		}
		std::cout<<'\n';
		for(auto& cluster:text_clusters)
			std::cout<<cluster.num_bytes<<':'<<cluster.num_glyphs<<' ';
		std::cout<<'\n';

		cr->show_glyphs(glyphs);
		height += itr->get_fill();
		++itr;
	}
	cr->restore();
}



bool ClvFileArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	//auto window = get_window(Gtk::TEXT_WINDOW_TEXT);
	cr->save();
	cr->set_source_rgb(0.3, 0.4, 0.5);
	cr->paint();
	//cr->stroke();
	std::vector<Cairo::Rectangle> clip_rects;
	cr->copy_clip_rectangle_list(clip_rects);
	//std::cerr<<"num rect = "<<clip_rects.size()<<std::endl;
	for(auto& clip_rect:clip_rects){
		std::cerr<<"x"<<clip_rect.x<<"y"<<clip_rect.y<<"\tw="<<clip_rect.width<<" h="<<clip_rect.height<<std::endl;
		draw(cr,clip_rect);
	}

	return true;//Gtk::TextView::on_draw(cr);
}

bool ClvFileArea::on_key_press_event(GdkEventKey *event){
	std::cout<<"press"<<event->string<<std::endl;
	return true;
}

bool ClvFileArea::on_key_release_event(GdkEventKey *event){
	std::cout<<"release"<<event->string<<std::endl;
	return true;
}


ClvThumArea::ClvThumArea(){

}

ClvThumArea::~ClvThumArea(){

}

bool ClvThumArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	return true;
}

