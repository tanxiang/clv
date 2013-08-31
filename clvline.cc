#include "clvline.h"

int _cairo_utf8_get_char_validated (const char *p,uint32_t *unicode);

cairo_status_t _cairo_utf8_to_ucs4 (const char *str,int len, uint32_t **result, int	*items_written);

int _cairo_ucs4_to_utf8 (uint32_t unicode,char *utf8);

void line::sync_glyphs(unsigned int g_index){
	if(g_index>glyphs.size())
		g_index=0;
	unsigned int c_index=glyphs_index[g_index];
	uint32_t ucs4;
	while(size()>c_index){
		c_index += _cairo_utf8_get_char_validated(&(this->c_str()[c_index]),&ucs4);	
		glyphs_index[g_index+1]=c_index;
	}
}

