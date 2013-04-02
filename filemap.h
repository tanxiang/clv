class FileMap{
	void *P;
	int Length;
	int FD;

protected:
public:
	FileMap(char* FilePath);
	~FileMap();
	void* Get(){return P;}
	int GetLength(){return Length;}
	int Merge();
};
