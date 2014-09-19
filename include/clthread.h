#pragma once
#include <vector>
#include <string>
#include <condition_variable>
#include <thread>
#include <memory>

namespace clp{
class Invocation;
}

namespace clv{

struct MsgBox{
	std::mutex Mut;
	std::string Key;
	size_t nLine,nChar;
	std::vector<std::string> Context;	
};

class Compile{
	std::thread CompileThread;
	std::mutex MutReady;
	std::condition_variable CondReady;
	std::condition_variable CondSearch;
	MsgBox SearchMsg;
	std::unique_ptr<clp::Invocation> Invocation;
public:
	Compile();
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

extern Compile clast;
inline void parse_thread(const char* CodeName,void* P,int Len)
{
	clast.ParseCode(CodeName,P,Len);
}
}//namespace
