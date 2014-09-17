#pragma once
//#include <memory>
#include <vector>
#include <string>
#include <istream>
#include <cairomm/cairomm.h>

namespace clv{

class glyphs_group :public std::pair<std::vector<Cairo::Glyph>,std::vector<size_t> >
{
	Cairo::RefPtr<Cairo::ScaledFont> font_ptr;
public:
	glyphs_group(Cairo::RefPtr<Cairo::ScaledFont> ptr):font_ptr(ptr){
	}

	void move_x_to(int x){
		for(auto &gl:first)
			gl.x=x;
	}

	void move_y_to(int y){
		for(auto &gl:first)
			gl.y=y;
	}

	void move_x(int dx){
		for(auto &gl:first)
			gl.x+=dx;
	}

	void move_y(int dy){
		for(auto &gl:first)
			gl.y+=dy;
	}

	bool draw_to_context(const Cairo::RefPtr<Cairo::Context>& cr);
	void get_extents(Cairo::FontExtents& extents){
		font_ptr->get_extents(extents);
	}
};
//#include <unistd.h>

class line :public std::string 
{
	bool synced=false;
	std::vector<glyphs_group> line_glyphs;
protected:
	void shape();

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
	line(T& refline):std::string{refline.begin(),refline.end()}{
		shape();
	}

	fill_t get_fill(){
		return 18;
	}
	std::vector<glyphs_group>::iterator x_to_glyph_itr(int x);

	bool draw_to_context(const Cairo::RefPtr<Cairo::Context>& cr,int y,const Cairo::Rectangle &rect);
};

}//namespace clv