#ifndef _CLVCONTEXT_H
#define _CLVCONTEXT_H
#include "unorder_tree.h"
#include <vector>
template<typename T>
class context :public unorder_tree<T>{
	struct cursor:public unorder_tree<T>::iterator{
		typename T::glyph_iter it_glyph;
	};
public:

	std::vector<cursor> cursors;

	template<typename InputIterator,typename = std::_RequireInputIter<InputIterator>>
	context(InputIterator first,InputIterator last):unorder_tree<T>(first,last){}
	//context();

protected:
};
#endif //_CLVCONTEXT_H
