#ifndef _CLVLINE_H
#define _CLVLINE_H
#include <vector>
#include <string>
#include <istream>
#include <cairomm/cairomm.h>

class glyphs_group :public std::vector<Cairo::Glyph>
{
public:
	bool draw_to_context(const Cairo::RefPtr<Cairo::Context>& cr);
};

class line :public std::string 
{
	bool synced;
	std::vector<glyphs_group> line_glyphs;
	std::vector<std::vector<int> > line_index;
protected:
	void sync_glyphs(const Cairo::RefPtr<Cairo::Context>& cr,int y,unsigned int s=0);
public:
	typedef int fill_t;
	struct glyphs_iter{
		std::vector<glyphs_group>::iterator iter_group;
		glyphs_group::iterator iter_glyph;
	};
	friend std::istream & operator>>(std::istream & is, line& l){
		return std::getline(is, l);
	}
	//virtual ~line(){
		//std::cerr<<"line free\n";
	//}
	line():synced(false){
		//glyphs_index.push_back(0);
	}
	fill_t get_fill(){
		return 18;
	}
	int x_to_index(int x);
	bool draw_to_context(const Cairo::RefPtr<Cairo::Context>& cr,int y,const Cairo::Rectangle &rect);
};
#endif
