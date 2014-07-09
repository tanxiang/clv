#include "clvhb.hh"
#include "config.hh"

namespace clv{

hbfont::hbfont(){
	blob=hb_blob_create();
	hb_face_t *face = hb_face_create (blob, face_index);
	  font = hb_font_create (face);

  unsigned int upem = hb_face_get_upem (face);
  hb_font_set_scale (font, upem, upem);
	hb_face_destroy (face);
	  hb_ot_font_set_funcs (font);
}

hbfont::~hbfont(){

}

}