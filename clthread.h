#include <future>
class ClvCompile{
	std::thread CompileThread;
	//std::thread SearchThread;
	//std::future<bool> CompileResult;
	std::promise<int> SearchPromise;
	std::future<int> SearchResult;

	std::promise<int> ReadyPromise;
	std::future<int> ReadyResult;

	std::promise<int> CmdPromise;
	std::future<int> CmdResult;

public:
	ClvCompile();
	void* PchCode(const char* CodeName,void* P,int Len);
	bool ParseCode(const char* CodeName,void* P,int Len,void* Pch=nullptr);
	bool ParseCodeError(){
		return true;
	}
	bool Ready(){
		return true;
	}
	bool SearchAST();
};

extern ClvCompile clast;
inline void parse_thread(const char* CodeName,void* P,int Len)
{
	clast.ParseCode(CodeName,P,Len);
}
