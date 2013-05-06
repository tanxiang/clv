#include <future>
class ClvCompile{
	std::packaged_task<bool(const char* CodeName,void* P,int Len)> CompileThread;
	//std::thread SearchThread;
	//std::futrue SearchFutrue;
	std::promise<int> SearchPromise;
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
