#include "dbg.h"
#include <vector>
#include <string>
#include <cairomm/cairomm.h>

class ClvFontFace : public Cairo::UserFontFace{
protected:
	Cairo::ErrorStatus init(const Cairo::RefPtr<Cairo::ScaledFont>& /*scaled_font*/,
		const Cairo::RefPtr<Cairo::Context>& /*cr*/,
		Cairo::FontExtents &extents) override;

	Cairo::ErrorStatus text_to_glyphs(const Cairo::RefPtr<Cairo::ScaledFont>& scaled_font,
		const std::string& utf8,
		std::vector<Cairo::Glyph>& glyphs,
		std::vector<Cairo::TextCluster>& clusters,
		Cairo::TextClusterFlags& cluster_flags) override;

	Cairo::ErrorStatus unicode_to_glyph(const Cairo::RefPtr<Cairo::ScaledFont>& scaled_font,
		unsigned long unicode,
		unsigned long &glyph) override;

	Cairo::ErrorStatus render_glyph(const Cairo::RefPtr<Cairo::ScaledFont>& scaled_font,
		unsigned long glyph,
		const Cairo::RefPtr<Cairo::Context>& cr,
		Cairo::TextExtents& metrics) override;

	ClvFontFace() : UserFontFace() {
		// constructor implementation
	}
public:
	static Cairo::RefPtr<ClvFontFace> create() {
		return Cairo::RefPtr<ClvFontFace>(new ClvFontFace());
	}

};

unsigned long ucs4_to_index(void *abstract_font, uint32_t ucs4);