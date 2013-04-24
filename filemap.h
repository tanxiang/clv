template<typename Code>
class FileMap{
	class iterator{
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
};

