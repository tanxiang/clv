#ifndef _CLVCONTEXT_H
#define _CLVCONTEXT_H
#include "unorder_tree.h"
template<typename T>
class context :public unorder_tree<T>{

public:
	template<typename InputIterator,typename = std::_RequireInputIter<InputIterator>>
	context(InputIterator first,InputIterator last):unorder_tree<T>(first,last){}
	//context();
protected:
};
#endif //_CLVCONTEXT_H