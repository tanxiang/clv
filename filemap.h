template<typename LineRef>
class FileMap{
	struct iterator_traits {
		typedef std::bidirectional_iterator_tag iterator_category;
		typedef LineRef		value_type;
		typedef std::ptrdiff_t	difference_type;
		typedef LineRef*		pointer;
		typedef LineRef&		reference;
	};
	struct iterator:public iterator_traits{
		void *P;
		int Len;
	public:
		iterator operator ++ (){return *this;}
		int Length(){return Len;}
		bool operator !=(iterator it){return true;}
		void* operator *(){return P;}
	};

	void *P;
	int Len;
	int FD;

protected:
public:
	FileMap(const char* FilePath);
	~FileMap();
	//low level getdata
	void* Get(){return P;}
	int Length(){return Len;}
	int Merge();
	//edit iface
	iterator begin(){
		return NULL;
	}
	//iterator end(){
	//	return NULL;
	//}
};

