template<typename Code>
class FileMap{
	class iterator{
		iterator operator ++ (){return nullptr;}
		int Length(){return 3;}
		bool operator !=(iterator it){return true;}
		Code* operator *(){return nullptr;}
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
};
