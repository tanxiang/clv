#ifndef _CLVLINE_H
#define _CLVLINE_H
#include <vector>
#include <string>
#include <istream>
#include <cairomm/cairomm.h>

class line :public std::string 
{
	std::vector<Cairo::Glyph> glyphs;
	std::vector<int> glyphs_index;
protected:
	void sync_glyphs(unsigned int s=0);
public:
	typedef int fill_t;
	friend std::istream & operator>>(std::istream & is, line& l){
		return std::getline(is, l);
	}
	//virtual ~line(){
		//std::cerr<<"line free\n";
	//}
	line(){
		glyphs_index.push_back(0);
	}
	fill_t get_fill(){
		return 16;
	}
	
	bool draw_to_context(const Cairo::RefPtr<Cairo::Context>& cr,int y,const Cairo::Rectangle &rect);
};
#endif
