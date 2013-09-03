#include "clvline.h"

extern "C"{
struct _cairo_hash_entry {
	unsigned long hash;
};
typedef struct _cairo_hash_entry cairo_hash_entry_t;
typedef int32_t cairo_atomic_int_t;
typedef struct {
	cairo_atomic_int_t ref_count;
} cairo_reference_count_t;

struct _cairo_array {
	unsigned int size;
	unsigned int num_elements;
	unsigned int element_size;
	char *elements;
};
typedef struct _cairo_array cairo_user_data_array_t;

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

struct _cairo_font_options {
	cairo_antialias_t antialias;
	cairo_subpixel_order_t subpixel_order;
	cairo_lcd_filter_t lcd_filter;
	cairo_hint_style_t hint_style;
	cairo_hint_metrics_t hint_metrics;
	cairo_round_glyph_positions_t round_glyph_positions;
};

typedef int cairo_mutex_t;

typedef struct _cairo_hash_table cairo_hash_table_t;

typedef struct _cairo_list {
	struct _cairo_list *next, *prev;
} cairo_list_t;

typedef struct _cairo_scaled_font_backend   cairo_scaled_font_backend_t;

struct _cairo_scaled_font {
	/* For most cairo objects, the rule for multiple threads is that
	 * the user is responsible for any locking if the same object is
	 * manipulated from multiple threads simultaneously.
	 *
	 * However, with the caching that cairo does for scaled fonts, a
	 * user can easily end up with the same cairo_scaled_font object
	 * being manipulated from multiple threads without the user ever
	 * being aware of this, (and in fact, unable to control it).
	 *
	 * So, as a special exception, the cairo implementation takes care
	 * of all locking needed for cairo_scaled_font_t. Most of what is
	 * in the scaled font is immutable, (which is what allows for the
	 * sharing in the first place). The things that are modified and
	 * the locks protecting them are as follows:
	 *
	 * 1. The reference count (scaled_font->ref_count)
	 *
	 *    Modifications to the reference count are protected by the
	 *    _cairo_scaled_font_map_mutex. This is because the reference
	 *    count of a scaled font is intimately related with the font
	 *    map itself, (and the magic holdovers array).
	 *
	 * 2. The cache of glyphs (scaled_font->glyphs)
	 * 3. The backend private data (scaled_font->surface_backend,
	 *				    scaled_font->surface_private)
	 *
	 *    Modifications to these fields are protected with locks on
	 *    scaled_font->mutex in the generic scaled_font code.
	 */

	cairo_hash_entry_t hash_entry;

	/* useful bits for _cairo_scaled_font_nil */
	cairo_status_t status;
	cairo_reference_count_t ref_count;
	cairo_user_data_array_t user_data;

	cairo_font_face_t *original_font_face; /* may be NULL */

	/* hash key members */
	cairo_font_face_t *font_face; /* may be NULL */
	cairo_matrix_t font_matrix;	  /* font space => user space */
	cairo_matrix_t ctm;	          /* user space => device space */
	cairo_font_options_t options;

	unsigned int placeholder : 1; /*  protected by fontmap mutex */
	unsigned int holdover : 1;
	unsigned int finished : 1;

	/* "live" scaled_font members */
	cairo_matrix_t scale;	     /* font space => device space */
	cairo_matrix_t scale_inverse;    /* device space => font space */
	double max_scale;		     /* maximum x/y expansion of scale */
	cairo_font_extents_t extents;    /* user space */
	cairo_font_extents_t fs_extents; /* font space */

	/* The mutex protects modification to all subsequent fields. */
	cairo_mutex_t mutex;

	cairo_hash_table_t *glyphs;
	cairo_list_t glyph_pages;
	cairo_bool_t cache_frozen;
	cairo_bool_t global_cache_frozen;

	cairo_list_t dev_privates;

	/* font backend managing this scaled font */
	const cairo_scaled_font_backend_t *backend;
	cairo_list_t link;
};

#define cairo_warn
typedef uint32_t cairo_int_status_t;//enum type
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

void line::sync_glyphs(unsigned int g_index){
	if(g_index>glyphs.size())
		g_index=0;
	size_t c_index=glyphs_index[g_index];
	uint32_t ucs4;
	while(size()>c_index){
		c_index += _cairo_utf8_get_char_validated(&(this->c_str()[c_index]),&ucs4);	
		glyphs_index[g_index+1]=c_index;
	}
}

bool line::draw_to_context(const Cairo::RefPtr<Cairo::Context> &cr,int y, const Cairo::Rectangle &rect){
	cr->save();
	cr->restore();
	return true;
}
