class FileMap{
	void *P;
	int Length;
	int FD;

protected:
public:
	FileMap(const char* FilePath);
	~FileMap();
	void* Get(){return P;}
	int GetLength(){return Length;}
	int Merge();
};
