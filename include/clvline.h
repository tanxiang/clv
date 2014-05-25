#pragma once
//#include <memory>
#include <vector>
#include <string>
#include <istream>
#include <cairomm/cairomm.h>

namespace clv{

class glyphs_group :public std::pair<std::vector<Cairo::Glyph>,std::vector<int> >
{
	Cairo::RefPtr<Cairo::ScaledFont> font_ptr;
public:
	glyphs_group(Cairo::RefPtr<Cairo::ScaledFont> ptr):font_ptr(ptr){
	}
	bool draw_to_context(const Cairo::RefPtr<Cairo::Context>& cr);
	void get_extents(Cairo::FontExtents& extents){
		font_ptr->get_extents(extents);
	}
};

class line :public std::string 
{
	bool synced=false;
	std::vector<glyphs_group> line_glyphs;
protected:
	//void sync_glyphs(const Cairo::RefPtr<Cairo::Context>& cr,int y,unsigned int s=0);
public:
	typedef unsigned fill_t;
	
	struct glyph_iter :public std::vector<glyphs_group>::iterator{
		size_t index;
	};

	friend std::istream & operator>>(std::istream & is, line& l){
		return std::getline(is, l);
	}
	//virtual ~line(){
		//std::cerr<<"line free\n";
	//}
	template<typename T>
	line(T& refline){
		std::string{refline.begin(),refline.end()};
	}

	fill_t get_fill(){
		return 18;
	}
	std::vector<glyphs_group>::iterator x_to_glyph_itr(int x);

	bool draw_to_context(const Cairo::RefPtr<Cairo::Context>& cr,int y,const Cairo::Rectangle &rect);
};

}//namespace clv