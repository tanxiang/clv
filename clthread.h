#include <future>
class Compile{
	std::thread CompileThread;
	std::thread SearchThread;
public:
	void* PchCode(const char* CodeName,void* P,int Len);
	bool ParseCode(const char* CodeName,void* P,int Len,void* Pch=nullptr);
	bool ParseCodeError();
	bool Ready();
	bool SearchAST();
};

void parse_thread(const char* CodeName,void* P,int Len);
