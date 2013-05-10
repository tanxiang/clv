#include <string>
#include <condition_variable>
#include <thread>
class ClvCompile{
	std::thread CompileThread;
	std::mutex MutReady;
	std::condition_variable CondReady;
	std::condition_variable CondSearch;
	std::string Key;
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
	bool SearchAST(const std::string& Name);
	bool SearchAST(int nLine,int nChar);
	bool SearchAST(int nOffset);
};

extern ClvCompile clast;
inline void parse_thread(const char* CodeName,void* P,int Len)
{
	clast.ParseCode(CodeName,P,Len);
}
