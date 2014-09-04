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

	unsigned long upem = hb_face_get_upem (hb_font_get_face(font));

	hb_ft_font_set_funcs (font);
}

hbfont::~hbfont(){

}

Cairo::RefPtr<Cairo::ScaledFont> hbfont::ScaledFont(Cairo::Matrix scale_mat){
	auto cr_font = Cairo::FtFontFace::create(hb_ft_font_get_face(font),0);
	return Cairo::ScaledFont::create(cr_font,scale_mat,Cairo::identity_matrix());
}

int hbfont::shape(std::vector<Cairo::Glyph> &glyphs,hbbuffer,Cairo::RefPtr<Cairo::ScaledFont> scaled_font){

	return 0;
}


}//namespace