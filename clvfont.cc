#include "clvfont.h"
//#include <fontconfig.h>
//#include <fcfreetype.h>

Cairo::ErrorStatus init(const Cairo::RefPtr<Cairo::ScaledFont>& /*scaled_font*/,
	const Cairo::RefPtr<Cairo::Context>& /*cr*/,
	Cairo::FontExtents &extents) {
	
	return CAIRO_STATUS_SUCCESS;
}

Cairo::ErrorStatus ClvFontFace::unicode_to_glyph (const Cairo::RefPtr<Cairo::ScaledFont>& /*scaled_font*/,
	unsigned long unicode, unsigned long& glyph) {

	return CAIRO_STATUS_SUCCESS;
}

Cairo::ErrorStatus ClvFontFace::render_glyph(const Cairo::RefPtr<Cairo::ScaledFont>& /*scaled_font*/,
	unsigned long glyph,
	const Cairo::RefPtr<Cairo::Context>& cr,
	Cairo::TextExtents& metrics){

	return CAIRO_STATUS_SUCCESS;
}