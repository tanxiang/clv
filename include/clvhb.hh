#pragma once
#include <hb.h>
#include <hb-ot.h>
#include <cairo-ft.h>
#include <hb-ft.h>
#include "config.hh"
#include <cairomm/cairomm.h>
#include <vector>

namespace clv{
class hbbuffer{
	hb_buffer_t *buffer = hb_buffer_create ();
public:

	hbbuffer(){
		hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
	}
	~hbbuffer(){
		hb_buffer_destroy(buffer);
	}
	void set_language(hb_language_t language){
		hb_buffer_set_language(buffer,language);
	}
	size_t length(){
		return hb_buffer_get_length (buffer);
	}
	auto get(){
		return buffer;
	}
	
	auto add_utf32 (const uint32_t* utf32,size_t len){
		hb_buffer_add_utf32(buffer,utf32,len,0,len);
		return len;
	}
	
	auto add_utf16 (const uint16_t* utf16,size_t len){
		hb_buffer_add_utf16(buffer,utf16,len,0,len);
		return len;
	}
	
	auto add_utf8 (const char* utf8,size_t len){
		hb_buffer_add_utf8(buffer,utf8,len,0,len);
		return len;
	}

	auto get_glyph_infos(){
		return hb_buffer_get_glyph_infos (buffer, NULL);
	}
	auto get_glyph_positions(){
		return hb_buffer_get_glyph_positions (buffer, NULL);
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
