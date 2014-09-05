#pragma once
#include <hb.h>
#include <hb-ot.h>
#include <cairo-ft.h>
#include <hb-ft.h>
#include "config.hh"
#include <cairomm/cairomm.h>
#include <vector>

namespace clv{
class hbbuffer;
class hbfont{
	hb_font_t *font;
public:
	hbfont():hbfont(DEF_FONT_FILE){}

	hbfont(const char* font_file_name);
	Cairo::RefPtr<Cairo::ScaledFont> ScaledFont(Cairo::Matrix scale_mat = Cairo::scaling_matrix(16,16));
	int shape(std::vector<Cairo::Glyph> &glyphs,hbbuffer buffer,Cairo::RefPtr<Cairo::ScaledFont> scaled_font,int x0,int y0);
	~hbfont();
};

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
	int add_utf16 (){
		return 0;
	}
	int add_utf8 (){
		return 0;
	}

	auto get_glyph_infos(){
		return hb_buffer_get_glyph_infos (buffer, NULL);
	}
	auto get_glyph_positions(){
		return hb_buffer_get_glyph_positions (buffer, NULL);
	}
};

}