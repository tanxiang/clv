#pragma once
#include "unorder_tree.h"
#include <vector>
namespace clv{

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

}//namespace clv
