#include "clvhb.hh"
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

namespace clv{

hbfont::hbfont(const char* font_file_name){
	int font_file_fd = open(font_file_name,O_RDONLY);
	struct stat font_file_stat;
	if(fstat(font_file_fd,&font_file_stat)==-1){
		throw;
	}
	font_data = mmap(NULL,font_file_stat.st_size,PROT_READ,MAP_PRIVATE,font_file_fd,0);

	close(font_file_fd);

	blob = hb_blob_create((const char*)font_data,font_file_stat.st_size,HB_MEMORY_MODE_READONLY,font_data,(hb_destroy_func_t)munmap);
	hb_face_t *face = hb_face_create (blob, 0);
	font = hb_font_create (face);

	unsigned int upem = hb_face_get_upem (face);
	hb_font_set_scale (font, upem, upem);
	hb_face_destroy (face);
	hb_ot_font_set_funcs (font);
}

hbfont::~hbfont(){

}

}