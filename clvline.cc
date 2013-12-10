#include "clvline.h"

extern "C"{
typedef enum _cairo_lcd_filter {
	CAIRO_LCD_FILTER_DEFAULT,
	CAIRO_LCD_FILTER_NONE,
	CAIRO_LCD_FILTER_INTRA_PIXEL,
	CAIRO_LCD_FILTER_FIR3,
	CAIRO_LCD_FILTER_FIR5
} cairo_lcd_filter_t;

typedef enum _cairo_round_glyph_positions {
	CAIRO_ROUND_GLYPH_POS_DEFAULT,
	CAIRO_ROUND_GLYPH_POS_ON,
	CAIRO_ROUND_GLYPH_POS_OFF
} cairo_round_glyph_positions_t;


typedef struct _cairo_hash_table cairo_hash_table_t;

typedef struct _cairo_list {
	struct _cairo_list *next, *prev;
} cairo_list_t;

typedef struct _cairo_scaled_font_backend   cairo_scaled_font_backend_t;

struct _cairo_scaled_font {
	unsigned char padding[0x18c];
	const cairo_scaled_font_backend_t *backend;
	cairo_list_t link;
};

#define cairo_warn
typedef uint32_t cairo_int_status_t;//enum type

typedef struct _cairo_hash_entry cairo_hash_entry_t;
typedef int32_t cairo_fixed_t;
typedef struct _cairo_point {
	cairo_fixed_t x;
	cairo_fixed_t y;
} cairo_point_t;
typedef struct _cairo_line {
	cairo_point_t p1;
	cairo_point_t p2;
} cairo_line_t, cairo_box_t;
typedef struct _cairo_image_surface cairo_image_surface_t;
typedef struct _cairo_path_fixed cairo_path_fixed_t;
struct _cairo_hash_entry {
	unsigned long hash;
};
struct _cairo_scaled_glyph {
	cairo_hash_entry_t hash_entry;

	cairo_text_extents_t    metrics;		/* user-space metrics */
	cairo_text_extents_t    fs_metrics;		/* font-space metrics */
	cairo_box_t		    bbox;		/* device-space bounds */
	int16_t                 x_advance;		/* device-space rounded X advance */
	int16_t                 y_advance;		/* device-space rounded Y advance */

	unsigned int	    has_info;
	cairo_image_surface_t   *surface;		/* device-space image */
	cairo_path_fixed_t	    *path;		/* device-space outline */
	cairo_surface_t         *recording_surface;	/* device-space recording-surface */

	const void		   *dev_private_key;
	void		   *dev_private;
	cairo_list_t            dev_privates;
};
typedef struct _cairo_scaled_glyph cairo_scaled_glyph_t;
typedef enum _cairo_scaled_glyph_info{
	CAIRO_SCALED_GLYPH_INFO_METRICS	 = (1 << 0),
	CAIRO_SCALED_GLYPH_INFO_SURFACE	 = (1 << 1),
	CAIRO_SCALED_GLYPH_INFO_PATH	 = (1 << 2),
	CAIRO_SCALED_GLYPH_INFO_RECORDING_SURFACE = (1 << 3)
} cairo_scaled_glyph_info_t;
struct _cairo_scaled_font_backend {
	cairo_font_type_t type;

	void
		(*fini)		(void			*scaled_font);

	cairo_warn cairo_int_status_t
		(*scaled_glyph_init)	(void			     *scaled_font,
				cairo_scaled_glyph_t	     *scaled_glyph,
				cairo_scaled_glyph_info_t    info);

	/* A backend only needs to implement this or ucs4_to_index(), not
	 * both. This allows the backend to do something more sophisticated
	 * then just converting characters one by one.
	 */
	cairo_warn cairo_int_status_t
		(*text_to_glyphs) (void                       *scaled_font,
				double		           x,
				double		           y,
				const char	          *utf8,
				int		           utf8_len,
				cairo_glyph_t	         **glyphs,
				int		          *num_glyphs,
				cairo_text_cluster_t      **clusters,
				int		          *num_clusters,
				cairo_text_cluster_flags_t *cluster_flags);

	unsigned long
		(*ucs4_to_index)		(void			     *scaled_font,
				uint32_t		      ucs4);

	/* Read data from a sfnt font table.
	 * @scaled_font: font
	 * @tag: 4 byte table name specifying the table to read.
	 * @offset: offset into the table
	 * @buffer: buffer to write data into. Caller must ensure there is sufficient space.
	 *          If NULL, return the size of the table in @length.
	 * @length: If @buffer is NULL, the size of the table will be returned in @length.
	 *          If @buffer is not null, @length specifies the number of bytes to read.
	 *
	 * If less than @length bytes are available to read this function
	 * returns CAIRO_INT_STATUS_UNSUPPORTED. Note that requesting more
	 * bytes than are available in the table may continue reading data
	 * from the following table and return success. If this is
	 * undesirable the caller should first query the table size. If an
	 * error occurs the output value of @length is undefined.
	 *
	 * Returns CAIRO_INT_STATUS_UNSUPPORTED if not a sfnt style font or table not found.
	 */
	cairo_warn cairo_int_status_t
		(*load_truetype_table)(void		        *scaled_font,
				unsigned long         tag,
				long                  offset,
				unsigned char        *buffer,
				unsigned long        *length);

	/* ucs4 is set to -1 if the unicode character could not be found
	 * for the glyph */
	cairo_warn cairo_int_status_t
		(*index_to_ucs4)(void                       *scaled_font,
				unsigned long               index,
				uint32_t                   *ucs4);

	cairo_warn cairo_bool_t
		(*is_synthetic)(void                       *scaled_font);

	/* For type 1 fonts, return the glyph name for a given glyph index.
	 * A glyph index and list of glyph names in the Type 1 fonts is provided.
	 * The function returns the index of the glyph in the list of glyph names.
	 * @scaled_font: font
	 * @glyph_names: the names of each glyph in the Type 1 font in the
	 *   order they appear in the CharStrings array
	 * @num_glyph_names: the number of names in the glyph_names array
	 * @glyph_index: the given glyph index
	 * @glyph_array_index: (index into glyph_names) the glyph name corresponding
	 *  to the glyph_index
	 */

	cairo_warn cairo_int_status_t
		(*index_to_glyph_name)(void                 *scaled_font,
				char                **glyph_names,
				int                   num_glyph_names,
				unsigned long         glyph_index,
				unsigned long        *glyph_array_index);

	/* Read data from a PostScript font.
	 * @scaled_font: font
	 * @offset: offset into the table
	 * @buffer: buffer to write data into. Caller must ensure there is sufficient space.
	 *          If NULL, return the size of the table in @length.
	 * @length: If @buffer is NULL, the size of the table will be returned in @length.
	 *          If @buffer is not null, @length specifies the number of bytes to read.
	 *
	 * If less than @length bytes are available to read this function
	 * returns CAIRO_INT_STATUS_UNSUPPORTED. If an error occurs the
	 * output value of @length is undefined.
	 *
	 * Returns CAIRO_INT_STATUS_UNSUPPORTED if not a Type 1 font.
	 */
	cairo_warn cairo_int_status_t
		(*load_type1_data)    (void		        *scaled_font,
				long                  offset,
				unsigned char        *buffer,
				unsigned long        *length);
};


#define UTF8_COMPUTE(Char, Mask, Len)					      \
	if (Char < 128)							      \
{									      \
	Len = 1;								      \
	Mask = 0x7f;							      \
}									      \
else if ((Char & 0xe0) == 0xc0)					      \
{									      \
	Len = 2;								      \
	Mask = 0x1f;							      \
}									      \
else if ((Char & 0xf0) == 0xe0)					      \
{									      \
	Len = 3;								      \
	Mask = 0x0f;							      \
}									      \
else if ((Char & 0xf8) == 0xf0)					      \
{									      \
	Len = 4;								      \
	Mask = 0x07;							      \
}									      \
else if ((Char & 0xfc) == 0xf8)					      \
{									      \
	Len = 5;								      \
	Mask = 0x03;							      \
}									      \
else if ((Char & 0xfe) == 0xfc)					      \
{									      \
	Len = 6;								      \
	Mask = 0x01;							      \
}									      \
else									      \
Len = -1;

#define UTF8_LENGTH(Char)              \
	((Char) < 0x80 ? 1 :                 \
	 ((Char) < 0x800 ? 2 :               \
	  ((Char) < 0x10000 ? 3 :            \
	   ((Char) < 0x200000 ? 4 :          \
		((Char) < 0x4000000 ? 5 : 6)))))

#define UTF8_GET(Result, Chars, Count, Mask, Len)			      \
	(Result) = (Chars)[0] & (Mask);					      \
for ((Count) = 1; (Count) < (Len); ++(Count))				      \
{									      \
	if (((Chars)[(Count)] & 0xc0) != 0x80)				      \
	{								      \
		(Result) = -1;						      \
		break;							      \
	}								      \
	(Result) <<= 6;							      \
	(Result) |= ((Chars)[(Count)] & 0x3f);				      \
}

#define UNICODE_VALID(Char)                   \
	((Char) < 0x110000 &&                     \
	 (((Char) & 0xFFFFF800) != 0xD800) &&     \
	 ((Char) < 0xFDD0 || (Char) > 0xFDEF) &&  \
	 ((Char) & 0xFFFE) != 0xFFFE)

static const char utf8_skip_data[256] = {
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
};

#define UTF8_NEXT_CHAR(p) ((p) + utf8_skip_data[*(unsigned char *)(p)])

int _cairo_utf8_get_char_validated (const char *p,
		uint32_t   *unicode)
{
	int i, mask = 0, len;
	uint32_t result;
	unsigned char c = (unsigned char) *p;

	UTF8_COMPUTE (c, mask, len);
	if (len == -1) {
		if (unicode)
			*unicode = (uint32_t)-1;
		return 1;
	}
	UTF8_GET (result, p, i, mask, len);

	if (unicode)
		*unicode = result;
	return len;
}

	int
_cairo_ucs4_to_utf8 (uint32_t  unicode,
		char     *utf8)
{
	int bytes;
	char *p;

	if (unicode < 0x80) {
		if (utf8)
			*utf8 = unicode;
		return 1;
	} else if (unicode < 0x800) {
		bytes = 2;
	} else if (unicode < 0x10000) {
		bytes = 3;
	} else if (unicode < 0x200000) {
		bytes = 4;
	} else {
		return 0;
	}

	if (!utf8)
		return bytes;

	p = utf8 + bytes;
	while (p > utf8) {
		*--p = 0x80 | (unicode & 0x3f);
		unicode >>= 6;
	}
	*p |= 0xf0 << (4 - bytes);

	return bytes;
}

}
#include <iostream>

void line::sync_glyphs(const Cairo::RefPtr<Cairo::Context>& cr,int y,unsigned int g_index){
	cr->set_source_rgb(1,1,1);
	cr->select_font_face("Source Code Pro",Cairo::FONT_SLANT_NORMAL,Cairo::FONT_WEIGHT_NORMAL );
	cr->set_font_size(16);
	if(synced)
		return;
	std::vector<glyphs_group>::size_type groups_index=0;
	glyphs_group::size_type glyphs_index=0;
	for(auto& group:line_glyphs){
		if(group.size()>=g_index){
			glyphs_index=g_index;
			break;
		}
		g_index-=group.size();
		groups_index++;
	}
	if(groups_index == line_glyphs.size()){//groups_index cannot > line_glyphs.size() 
		line_glyphs.push_back(glyphs_group{});
		line_index.push_back(std::vector<int>{});
	}
	else{//delete to end
		line_glyphs.erase(line_glyphs.begin()+groups_index+1,line_glyphs.end());
		line_index.erase(line_index.begin()+groups_index+1,line_index.end());
		line_glyphs[groups_index].erase(line_glyphs[groups_index].begin()+glyphs_index,line_glyphs[groups_index].end());
		line_index[groups_index].erase(line_index[groups_index].begin()+glyphs_index,line_index[groups_index].end());
	}
	//line_glyphs.clear();
	//glyphs_index.clear();
	size_t c_index=0;
	if(glyphs_index)
		c_index=line_index[groups_index][glyphs_index-1];
	else if(groups_index)
		c_index=*line_index[groups_index-1].rbegin();
	//return;

	auto scaled_font = cr->get_scaled_font();
	auto backend = scaled_font->cobj()->backend;
#if 0
	printf(" %08x\n",scaled_font->cobj());
	printf(": %08x\n",&(scaled_font->cobj()->backend));
	printf(":: %08x\n",&(backend->text_to_glyphs));
#endif
	int i=0;
	uint32_t ucs4;
	cairo_scaled_glyph_t scaled_glyph;
	while(size()>c_index){
		c_index += _cairo_utf8_get_char_validated(&(c_str()[c_index]),&ucs4);	
		//if(ucs4 is Correct)
		line_index[groups_index].push_back(c_index);
		uint32_t glyph = backend->ucs4_to_index(scaled_font->cobj(),ucs4);
		if(glyph){
			scaled_glyph.hash_entry.hash=glyph;
			//FIXME:need cairo hash table acc
			backend->scaled_glyph_init(scaled_font->cobj(),&scaled_glyph,CAIRO_SCALED_GLYPH_INFO_METRICS);
			line_glyphs[groups_index].push_back(Cairo::Glyph{glyph,i,y});
			i+=scaled_glyph.x_advance;
		}
		else switch(ucs4){//u
			case '\t':
				glyph = backend->ucs4_to_index(scaled_font->cobj(),' ');
				scaled_glyph.hash_entry.hash=glyph;
				backend->scaled_glyph_init(scaled_font->cobj(),&scaled_glyph,CAIRO_SCALED_GLYPH_INFO_METRICS);
				line_glyphs[groups_index].push_back(Cairo::Glyph{glyph,i+=scaled_glyph.x_advance*4,y});
				break;
			case '\r':
				break;
			case '\n':
				break;
			default: //fallback font;
				;
				//std::cout<<"unrec crect\n";
		}
	}
	synced=true;
	//std::cout<<std::endl;
}
#include <algorithm>
/*
int line::x_to_index(int x){
	auto lower_group = std::lower_bound(line_glyphs.begin(),line_glyphs.end(),x,
			[](glyphs_group &group,int x){return group.empty() ? true : group.rbegin()->x < x;});
	if(lower_group==line_glyphs.end())
		return 0;
	auto lower_gl = std::lower_bound(lower_group->begin(),lower_group->end(),x,
			[](Cairo::Glyph &gl ,int x){return gl.x < x;});
	std::cerr<<"x:"<<lower_gl->x<<"\n";
	return 0;
}
*/
glyphs_group::iterator line::x_to_glyph_itr(int x){
	auto lower_group = std::lower_bound(line_glyphs.begin(),line_glyphs.end(),x,
			[](glyphs_group &group,int x){return group.empty() ? true : group.rbegin()->x < x;});
	if(lower_group==line_glyphs.end()){
		return --(--lower_group)->end();//FIXME bug
	}
	auto lower_gl = std::lower_bound(lower_group->begin(),lower_group->end(),x,
			[](Cairo::Glyph &gl ,int x){return gl.x < x;});
	std::cerr<<"x:"<<lower_gl->x<<"\n";
	return lower_gl;
}

bool line::draw_to_context(const Cairo::RefPtr<Cairo::Context> &cr,int y, const Cairo::Rectangle &rect){
	cr->save();
	sync_glyphs(cr,y);
	for(auto& glyphs:line_glyphs)
		glyphs.draw_to_context(cr);
	cr->restore();
	return true;
}


bool glyphs_group::draw_to_context(const Cairo::RefPtr<Cairo::Context> &cr){
	cr->save();
	cr->stroke();
	cr->show_glyphs(*this);
	cr->restore();
	return true;
}
