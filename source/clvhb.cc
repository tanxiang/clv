#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <glib.h>
#include "clvhb.hh"
#include "dbg.h"
namespace clv{

hbfont::hbfont(const char* font_file_name){
	GError *error = NULL;
	GMappedFile *mf = g_mapped_file_new (font_file_name, false, &error);
	if (!mf) {
		debug<<error->message;
		g_error_free (error);
		return;
	}

	hb_blob_t* blob = hb_blob_create (g_mapped_file_get_contents (mf),g_mapped_file_get_length (mf),
		HB_MEMORY_MODE_READONLY_MAY_MAKE_WRITABLE,(void *) mf, (hb_destroy_func_t) g_mapped_file_unref);
	hb_face_t *face = hb_face_create (blob, 0);
	font = hb_font_create (face);
	hb_blob_destroy(blob);

	unsigned int upem = hb_face_get_upem (face);
	hb_font_set_scale (font, upem, upem);
	hb_face_destroy (face);
	hb_ot_font_set_funcs (font);
}

hbfont::~hbfont(){

}


int hbbuffer::draw_to_cairo(const Cairo::RefPtr<Cairo::Context>& cr){
	
	return 0;
}


}