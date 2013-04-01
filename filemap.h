class FileMap{
	void *P;
	int Length;
	int FD;

protected:
public:
	FileMap(char* FilePath);
	~FileMap();	
};
