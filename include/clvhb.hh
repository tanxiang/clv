#pragma once
#include <hb.h>
#include <hb-ot.h>
#include <cairo-ft.h>
#include <hb-ft.h>
#include "config.hh"
#include <cairomm/cairomm.h>
#include <vector>
#include <memory>

namespace clv{
class hbbuffer{
	std::unique_ptr<hb_buffer_t,decltype(&hb_buffer_destroy)> buffer {hb_buffer_create(),hb_buffer_destroy};
public:
	
	auto get(){
		return buffer.get();
	}
	
	hbbuffer(){
		hb_buffer_set_direction(get(), HB_DIRECTION_LTR);
	}
	//~hbbuffer(){
		//hb_buffer_destroy(buffer);
	//}
	void set_language(hb_language_t language){
		hb_buffer_set_language(get(),language);
	}
	size_t length(){
		return hb_buffer_get_length (get());
	}
	
	auto add_utf32 (const uint32_t* utf32,size_t len){
		hb_buffer_add_utf32(get(),utf32,len,0,len);
		return len;
	}
	
	auto add_utf16 (const uint16_t* utf16,size_t len){
		hb_buffer_add_utf16(get(),utf16,len,0,len);
		return len;
	}
	
	auto add_utf8 (const char* utf8,size_t len){
		hb_buffer_add_utf8(get(),utf8,len,0,len);
		return len;
	}

	auto get_glyph_infos(){
		return hb_buffer_get_glyph_infos (get(), NULL);
	}
	auto get_glyph_positions(){
		return hb_buffer_get_glyph_positions (get(), NULL);
	}
};

class hbfont{
	hb_font_t *font;
public:
	hbfont():hbfont(DEF_FONT_FILE){}
	~hbfont();

	hbfont(const char* font_file_name);
	Cairo::RefPtr<Cairo::ScaledFont> ScaledFont(Cairo::Matrix scale_mat = Cairo::scaling_matrix(16,16));
	int shape(std::vector<Cairo::Glyph> &glyphs,Cairo::RefPtr<Cairo::ScaledFont> scaled_font,int x0,int y0,hbbuffer &buffer);
	auto shape(std::vector<Cairo::Glyph> &glyphs,Cairo::RefPtr<Cairo::ScaledFont> scaled_font,int x0,int y0,const char* utf8,size_t len){
		hbbuffer buffer;
		buffer.add_utf8(utf8,len);
		return shape(glyphs,scaled_font,x0,y0,buffer);
	}
	
	int shape(std::vector<Cairo::Glyph> &glyphs,Cairo::RefPtr<Cairo::ScaledFont> scaled_font,int x0,int y0,const uint32_t* utf32,size_t len){
		hbbuffer buffer;
		buffer.add_utf32(utf32,len);
		return shape(glyphs,scaled_font,x0,y0,buffer);
	}
};

}
