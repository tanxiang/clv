#pragma once
#include <string>

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
	void *P;
	int Len;
protected:
	void CalLength(){}
public:
	/*
	struct iterator_traits {
		typedef std::random_access_iterator_tag iterator_category;
		typedef Char_T		value_type;
		typedef std::ptrdiff_t		difference_type;
		typedef Char_T*		pointer;
		typedef Char_T&		reference;
	};*/
	struct iterator:public  std::iterator_traits<Char_T*>{
		Char_T* p_char;
		iterator operator ++ (){
			++p_char;
			return *this;
		}
		iterator operator -- (){
			--p_char;
			return *this;
		}

		bool operator !=(iterator it){return p_char != p_char;}
		bool operator ==(iterator it){return p_char == p_char;}

		Char_T& operator *(){return *p_char;}
		Char_T* operator -> (){return p_char;}		
	};
	RawLineRef(){}
	RawLineRef(void* P):P{P},Len{0}{}


	void* Get(){return P;}
	void Set(void* Pt){P=Pt;}
	int Length(){return Len;}
	//edit iface
	iterator begin(){
		//iterator itr{P};
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
	void *FileMemMap;
	//const char *file_name;
	int Len;
	int FD;
protected:
public:
	FileMap(const char* FilePath=nullptr);
	FileMap(const std::string &FilePath){
		FileMap(FilePath.c_str());
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
			char* Pc=static_cast<char*>(Line.Get());
			Pc += Line.Length();
			Line.Set(static_cast<void*>(Pc),Line.AllLength()-Line.Length());
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
