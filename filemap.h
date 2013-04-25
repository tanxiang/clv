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
		iterator operator ++ (){return *this;}
		bool operator !=(iterator it){return true;}
		LineRef operator *(){return Line;}
	};
	//edit iface
	iterator begin();//{
	//	return NULL;
	//}
	iterator end();//{
	//	return NULL;
	//}

	//low level getdata
	void* Get(){return P;}
	int Length(){return Len;}
	int Merge();

};

