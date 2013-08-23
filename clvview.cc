#include "clvview.h"
#include <pangomm/glyphstring.h>

bool ClvLineArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	if(should_draw_window(cr,get_window())){
		cr->set_source_rgb(0.0, 0.0, 0.0);
		cr->paint();
	}
	return true;
}

ClvFileArea::ClvFileArea(unorder_tree<line> &file):
	file_context(file){
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

void ClvFileArea::draw(const Cairo::RefPtr<Cairo::Context>& cr,const Cairo::Rectangle &rect){
	auto itr = file_context.get_form_fill(rect.y);

	int height = 0;
	do{
		std::cout<<*itr<<std::endl;
		height += itr->get_fill();
		++itr;
	}while(height<rect.height);
}

bool ClvFileArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	//auto window = get_window(Gtk::TEXT_WINDOW_TEXT);
	cr->save();
	cr->set_source_rgb(0.5, 0.4, 0.5);
	cr->paint();
		//cr->stroke();
	std::vector<Cairo::Rectangle> clip_rects;
	cr->copy_clip_rectangle_list(clip_rects);
	//std::cerr<<"num rect = "<<clip_rects.size()<<std::endl;
	//PangoCairoRenderer* crenderer = g_object_new (PANGO_TYPE_CAIRO_RENDERER, NULL);

	//crrenderer->cr=cr->cobj();
	//Glib::wrap(crrenderer,false);
	auto pg = get_pango_context();
	Pango::AttrList attrs;
	Pango::TabArray tab_stop;
	auto attr_weight = Pango::Attribute::create_attr_weight(Pango::WEIGHT_NORMAL);
	attrs.insert_before(attr_weight);
	auto attr_family = Pango::Attribute::create_attr_family("source code pro");
	attrs.insert_before(attr_family);
	auto items = pg->itemize("normal size",attrs);
	std::cerr<<"item size:"<<items.size()<<std::endl;
	for(const auto& item:items){
		auto gl = item.shape("normal");
		auto font_map = pg->get_font_map();
		Pango::FontDescription font_desc{"source code pro"};
		auto font = font_map->load_font(pg,font_desc);
		if(font->gobj())
			std::cout<<"get font"<<std::endl;
		cr->save();
		cr->set_source_rgb(1, 1, 1);
		cr->move_to(10, 10);
		pango_cairo_show_glyph_string(cr->cobj(),font->gobj(),gl.gobj());
		cr->restore();
	}
	for(auto& clip_rect:clip_rects){
		//std::cerr<<"x"<<clip_rect.x<<"y"<<clip_rect.y<<"\tw="<<clip_rect.width<<" h="<<clip_rect.height<<std::endl;
		//draw(cr,clip_rect);
	}

	return true;//Gtk::TextView::on_draw(cr);
}

ClvThumArea::ClvThumArea(){

}

ClvThumArea::~ClvThumArea(){

}

bool ClvThumArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	return true;
}

