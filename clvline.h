#ifndef _CLVLINE_H
#define _CLVLINE_H
#include <vector>
#include <string>
//#include <memory>
#include <gtkmm.h>
class line :public std::string 
{
	std::vector<Cairo::Glyph> glyphs;
public:
	typedef int fill_t;
	friend std::istream & operator>>(std::istream & is, line& l){
		return std::getline(is, l);
	}
	//virtual ~line(){
		//std::cerr<<"line free\n";
	//}
	fill_t get_fill(){
		return 12;
	}
};
#endif
