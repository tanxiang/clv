#ifndef _CLVLINE_H
#define _CLVLINE_H
#include <memory>
#include <string>
class line :public std::string 
{
	int coord_y;
	int tags;
	public:
		typedef int fill_t;
		friend std::istream & operator>>(std::istream & is, line& l){
			return std::getline(is, l);
		}
		virtual ~line(){
			//std::cerr<<"line free\n";
		}
		fill_t get_fill(){
			return 12;
		}
};
#endif
