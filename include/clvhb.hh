#pragma once
#include <hb.h>
#include <hb-ot.h>
#include <cairo-ft.h>
#include <hb-ft.h>
#include "config.hh"

namespace clv{

class hbfont{
	hb_font_t *font;
public:
	hbfont():hbfont(DEF_FONT_FILE){}

	hbfont(const char* font_file_name);

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
	int add_glyph(unsigned int glyph,unsigned int cluster){
		hb_buffer_add(buffer,glyph,cluster);
		return 0;
	}
	int add_utf16 (){
		return 0;
	}
	int add_utf8 (){
		return 0;
	}
};

}