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
	int shape(hbbuffer,std::vector<Cairo::Glyph> glyphs);
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
	int add_utf16 (){
		return 0;
	}
	int add_utf8 (){
		return 0;
	}

};

}