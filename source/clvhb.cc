#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <glib.h>
#include "clvhb.hh"
#include "dbg.h"
namespace clv{

class HBCairoFace:public Cairo::UserFontFace{
public:
	static Cairo::RefPtr<HBCairoFace> create() {
		return Cairo::RefPtr<HBCairoFace>{new HBCairoFace};
	}
protected:
	// implement render_glyph() and any other virtual functions you want to override
	Cairo::ErrorStatus render_glyph(const Cairo::RefPtr<Cairo::ScaledFont>& scaled_font,
						unsigned long glyph,
						const Cairo::RefPtr<Cairo::Context>& cr,
						Cairo::TextExtents& metrics) {
	// render the glyph into cr here
		return CAIRO_STATUS_SUCCESS;
   }

	HBCairoFace() : UserFontFace() {
		// constructor implementation
	}
};

hbfont::hbfont(const char* font_file_name){
	GError *error = NULL;
	GMappedFile *mf = g_mapped_file_new (font_file_name, false, &error);
	if (!mf) {
		debug<<error->message<<"\n";
		g_error_free (error);
		return;
	}

	hb_blob_t* blob = hb_blob_create (g_mapped_file_get_contents (mf),g_mapped_file_get_length (mf),
		HB_MEMORY_MODE_READONLY_MAY_MAKE_WRITABLE,(void *) mf, (hb_destroy_func_t) g_mapped_file_unref);
	hb_face_t *face = hb_face_create (blob, 0);
	font = hb_font_create (face);
	hb_face_destroy (face);
	hb_blob_destroy(blob);
	hb_ft_font_set_funcs (font);
}

hbfont::~hbfont(){

}

Cairo::RefPtr<Cairo::ScaledFont> hbfont::ScaledFont(Cairo::Matrix scale_mat){
	auto cr_font = Cairo::FtFontFace::create(hb_ft_font_get_face(font),0);
	return Cairo::ScaledFont::create(cr_font,scale_mat,Cairo::identity_matrix());
}

int hbfont::shape(std::vector<Cairo::Glyph> &glyphs,hbbuffer buffer,Cairo::RefPtr<Cairo::ScaledFont> scaled_font,int x0,int y0){
	Cairo::Matrix scale_mat;
	scaled_font->get_font_matrix(scale_mat);
	unsigned long upem = hb_face_get_upem (hb_font_get_face(font));
	debug<<scale_mat.x0<<':'<<scale_mat.y0<<':'<<scale_mat.xx<<':'<<scale_mat.yy<<std::endl;
	(x0*=upem)/=16;
	(y0*=upem)/=16;
	hb_shape (font, buffer.get(), NULL, 0);
	auto hb_glyph = buffer.get_glyph_infos ();
	auto hb_position = buffer.get_glyph_positions ();
	for(size_t i=hb_buffer_get_length (buffer.get());i>0;i--){
		glyphs.push_back(
			Cairo::Glyph{hb_glyph->codepoint,
				(x0+hb_position->x_offset)*16/upem,
				(y0-hb_position->y_offset)*16/upem}
		);
		x0 += hb_position->x_advance;
		y0 -= hb_position->y_advance;
		hb_glyph++;
		hb_position++;
	}
	return 0;
}


}//namespace