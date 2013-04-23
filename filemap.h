class FileMap{
	void *P;
	int Len;
	int FD;

protected:
public:
	FileMap(const char* FilePath);
	~FileMap();
	void* Get(){return P;}
	int Length(){return Len;}
	int Merge();
};
