class CharRef{
	void *P;
protected:
public:
	int Length(){return 1;}
	int WLength(){return 1;}
	void n(){}//移动一个字符
	void n(int i){
		while(i--)n();
	}
	void p(){}//反向移动一个字符
	void p(int i){
		while(i--)p();
	}
	void w(){}//移动一个word
	void w(int i){
		while(i--)w();
	}
	void b(){}//反向移动一个word
	void b(int i){
		while(i--)b();
	}
	void reset(){}//行首^
	void toend(){}//行末$
};

template<typename SourceChar>
class MBLineRef{
	void *P;
	int Len;
protected:
public:
	struct iterator_traits {
		typedef std::bidirectional_iterator_tag iterator_category;
		typedef SourceChar		value_type;
		typedef std::ptrdiff_t	difference_type;
		typedef SourceChar*		pointer;
		typedef SourceChar&		reference;
	};
	struct iterator:public iterator_traits{
		CharRef Char;
	};
	MBLineRef(){}
	MBLineRef(void* P):P{P},Len{0}{
		CalLength();
	}

	void CalLength(){
		Len=0;
		char* Pc=static_cast<char*>(P);
		while(*(Pc++)!='\n')
			++Len;
	}
	//low level getdata
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

template<typename SourceChar>
class WCLineRef{
	void *P;
	int Len;
protected:
public:
	struct iterator_traits {
		typedef std::random_access_iterator_tag iterator_category;
		typedef SourceChar		value_type;
		typedef std::ptrdiff_t	difference_type;
		typedef SourceChar*		pointer;
		typedef SourceChar&		reference;
	};
	struct iterator:public iterator_traits{

	};
	WCLineRef(){}
	WCLineRef(void* P):P{P},Len{0}{}
	void CalLength(){}

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
	void *P;
	int Len;
	int FD;
protected:
public:
	FileMap(const char* FilePath);
	~FileMap();

	struct iterator_traits {
		typedef std::bidirectional_iterator_tag iterator_category;
		typedef LineRef		value_type;
		typedef std::ptrdiff_t	difference_type;
		typedef LineRef*		pointer;
		typedef LineRef&		reference;
	};
	struct iterator:public iterator_traits{
		LineRef Line;
		iterator operator ++ (){
			char* Pc=static_cast<char*>(Line.Get());
			Pc += Line.Length();
			Line.Set(static_cast<void*>(++Pc));
			Line.CalLength();
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

