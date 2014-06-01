#pragma once
#include <string>
#include "dbg.h"

namespace clv{
class Utf8CharRef{//utf-8 char ref & opts
	void *P;
	enum CodeType{
		EXTN=0,		//10xxxxxx
		ASCII=1,	//0xxxxxxx
		EXT1=2,		//110xxxxx
		EXT2=3,		//1110xxxx
		EXT3=4,		//11110xxx
		EXT4=5		//111110xx
	};
protected:
	CodeType Code(char c){
		if(!(c & 0x80)) return ASCII;
		if((c & 0xc0) == 0x80) return EXTN;
		if((c & 0xe0) == 0xc0) return EXT1;
		if((c & 0xf0) == 0xe0) return EXT2;
		if((c & 0xf8) == 0xf0) return EXT3;
		return EXT4;
	}
public:
	int Length(){return Code(*static_cast<char*>(P));}
	int WordLength(){return 1;}
	int n(){//移动一个字符
		char* Pc = static_cast<char*>(P);
		int Len = Code(*Pc);
		Pc += Len;
		P = static_cast<void*>(Pc);
		return Len;
	}
	int n(int i){
		int Len = 0;
		while(i--)Len += n();
		return Len;
	}
	int p(){//反向移动一个字符
		char* Pc = static_cast<char*>(P);
		int Len = 0;
		while(!Len){
			--Pc;
			Len = Code(*Pc);
		}
		P = static_cast<void*>(Pc);
		return Len;
	}
	int p(int i){
		int Len = 0;
		while(i--)Len += p();
		return Len;
	}
	void w(){//移动一个word
		;
	}
	//void w(int i){
	//	while(i--)w();
	//}
	void b(){//反向移动一个word
		;
	}
	//void b(int i){
	//	while(i--)b();
	//}
	void Set(void*Pt){P=Pt;}
	void* Get(){return P;}
	char GetChar(){return *static_cast<char*>(P);}
};

template<typename CharRef>
class MBLineRef{
	void *P;
	int NumChar;
	int BitsToEnd;
protected:
	void CalLength(int Plen){
		NumChar=0;
		start->Set(P);
		finish->Set(P);
		while(finish->GetChar()!='\n'){
			//std::cout<<"char"<<std::endl;
			++NumChar;
			++finish;
		}
	}
public:
	struct iterator_traits {
		typedef std::bidirectional_iterator_tag iterator_category;
		typedef CharRef		value_type;
		typedef std::ptrdiff_t		difference_type;
		typedef CharRef*		pointer;
		typedef CharRef&		reference;
	};
	struct iterator:public iterator_traits{
		CharRef Char;
		iterator operator ++ (){
			Char.n();
			return *this;
		}
		iterator operator -- (){
			Char.p();
			return *this;
		}
		bool operator !=(iterator it){return Char.Get() != it->Get();}
		bool operator ==(iterator it){return Char.Get() == it->Get();}

		CharRef& operator *(){return Char;}
		CharRef* operator -> (){return &Char;}
	};
	MBLineRef():NumChar{0}{}
	MBLineRef(void* P,int Plen):P{P},BitsToEnd{Plen},NumChar{0}{
		if(Plen)
			CalLength(Plen);
	}

	//low level getdata
	void* Get(){return P;}
	void Set(void* Pt,int Plen){
		P=Pt;
		BitsToEnd=Plen;
		if(Plen)
			CalLength(Plen);
	}
	void Set(iterator itr,int Plen){
		Set(itr.operator->(),Plen);
	}
	int AllLength(){
		//FIXME:bug!! CalLength init start finish when to call CalLength??
		//CalLength();
		//std::cout<<"CalLength();"<<start->Get()<<'-'<<finish->Get()<<std::endl;
		return BitsToEnd;
	}

	int Length(){
		//FIXME:bug!! CalLength init start finish when to call CalLength??
		//CalLength();
		//std::cout<<"CalLength();"<<start->Get()<<'-'<<finish->Get()<<std::endl;
		return 1 + (static_cast<char*>(finish->Get()) -
			static_cast<char*>(start->Get()));
	}

	//edit iface
	iterator begin(){
		//CalLength();
		return start;
	}
	iterator end(){
		return finish;
	}
private:
	iterator start;
	iterator finish;
};

template<typename Char_T>
class RawLineRef{
	Char_T *Head;
	size_t LenToEnd;
protected:
	size_t CalLength(){
		start=Head;
		size_t LineLen;
		for(LineLen=0;LineLen<LenToEnd;LineLen++){
			if(Head[LineLen]=='\n'){//FIXME need wchar_t \r\n??
				finish=Head+LineLen+1;
				return LineLen;
			}
		}
		finish=Head+LenToEnd;
		return LineLen;
	}
public:

	typedef Char_T* iterator;
	//RawLineRef(){}
	//RawLineRef(Char_T* P):Head{P},LenToEnd{0}{}


	Char_T* Get(){return Head;}
	void Set(Char_T* Pt,size_t Len){
		Head=Pt;LenToEnd=Len;
		 CalLength();
	}

	size_t Length(){return  finish-start;}
	size_t AllLength(){return LenToEnd;}
	//edit iface
	iterator begin(){
		return start;
	}
	iterator end(){
		return finish;
	}
private:
	iterator start;
	iterator finish;
};

template<typename LineRef>
class FileMap{
	char *FileMemMap;
	//const char *file_name;
	int Len;
	int FD;
protected:
public:
	FileMap(const char* FilePath=nullptr);
	FileMap(const std::string &FilePath):FileMap(FilePath.c_str()){

	}

	~FileMap();

	struct iterator_traits {
		typedef std::bidirectional_iterator_tag iterator_category;
		typedef LineRef			value_type;
		typedef std::ptrdiff_t		difference_type;
		typedef LineRef*		pointer;
		typedef LineRef&		reference;
	};
	struct iterator:public iterator_traits{
		LineRef Line;
		iterator operator ++ (){
			//debug<<"line:";
			for(auto line_char:Line)
				debug<<line_char;
			//debug<<__PRETTY_FUNCTION__<<'\n';
			auto NextHead=Line.end();
			Line.Set(NextHead,Line.AllLength()-Line.Length());
			return *this;
		}
		bool operator !=(iterator it){return Line.Get() != it->Get();}
		LineRef& operator *(){return Line;}
		LineRef* operator -> (){return &Line;}
	};
	void* Get(){return FileMemMap;}
	int Length(){return Len;}
	int Merge();
	//edit iface
	iterator begin(){
		return start;
	}
	iterator end(){
		return finish;
	}
private:
	iterator start;
	iterator finish;
};

}//namespace clv
