class CharRef{

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
	};

	MBLineRef(void* P):P{P},Len{0}{
		CalLength();
	}

	//low level getdata
	void* Get(){return P;}
	void Set(void* Pt){P=Pt;}
	int Length(){return Len;}
	void CalLength(){
		Len=0;
		char* Pc=static_cast<char*>(P);
		while(*(Pc++)!='\n')
			++Len;
	}
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
	WCLineRef(void* P):P{P},Len{0}{}
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
		iterator(void* P):Line{P}{}
	};
	//edit iface
	iterator begin(){
		iterator itr{P};
		return itr;
	}
	iterator end();//{
	//	return NULL;
	//}

	//low level getdata
	void* Get(){return P;}
	int Length(){return Len;}
	int Merge();
};

