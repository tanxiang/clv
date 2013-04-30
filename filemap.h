class CharRef{//utf-8 char ref & opts
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
	void w(){}//移动一个word
	//void w(int i){
	//	while(i--)w();
	//}
	void b(){}//反向移动一个word
	//void b(int i){
	//	while(i--)b();
	//}
	void Set(void*Pt){P=Pt;}
	void* Get(){return P;}
	char GetChar(){return *static_cast<char*>(P);}
};

template<typename SourceChar>
class MBLineRef{
	void *P;
	int Len;
	//int Bits;
protected:
	void CalLength(){
		Len=0;
		start->Set(P);
		finish->Set(P);
		while(finish->GetChar()!='\n'){
			++Len;
			++finish;
		}
		//Bits = static_cast<char*>(finish->Get()) -
		//	static_cast<char*>(start->Get());
	}
public:
	struct iterator_traits {
		typedef std::bidirectional_iterator_tag iterator_category;
		typedef SourceChar		value_type;
		typedef std::ptrdiff_t		difference_type;
		typedef SourceChar*		pointer;
		typedef SourceChar&		reference;
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
		CharRef operator *(){return Char;}
		CharRef* operator -> (){return &Char;}
	};
	MBLineRef(){}
	MBLineRef(void* P):P{P},Len{0}{
		CalLength();
	}

	//low level getdata
	void* Get(){return P;}
	void Set(void* Pt,int PLen){
		P=Pt;
		CalLength();
	}
	int Length(){
		return 1 + (static_cast<char*>(finish->Get()) -
			static_cast<char*>(start->Get()));
	}

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

template<typename SourceChar>
class WCLineRef{
	void *P;
	int Len;
protected:
	void CalLength(){}
public:
	struct iterator_traits {
		typedef std::random_access_iterator_tag iterator_category;
		typedef SourceChar		value_type;
		typedef std::ptrdiff_t		difference_type;
		typedef SourceChar*		pointer;
		typedef SourceChar&		reference;
	};
	struct iterator:public iterator_traits{

	};
	WCLineRef(){}
	WCLineRef(void* P):P{P},Len{0}{}


	void* Get(){return P;}
	void Set(void* Pt,int PLen=0){P=Pt;}
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
	void *P;
	int Len;
	int FD;
protected:
public:
	FileMap(const char* FilePath);
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
			Line.Set(static_cast<void*>(Pc),0);
			return *this;
		}
		bool operator !=(iterator it){return Line.Get() != it->Get();}
		LineRef operator *(){return Line;}
		LineRef* operator -> (){return &Line;}
		//iterator(void* P):Line{P}{}
		//iterator(){}
	};
	//low level getdata
	void* Get(){return P;}
	int Length(){return Len;}
	int Merge();
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

