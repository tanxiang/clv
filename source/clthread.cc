#include <iostream>
#include "clthread.h"
#include "clangparse.h"

using namespace std;
namespace clv{

Compile::Compile():Invocation(new clp::Invocation{new clp::Action{CondReady,CondSearch,SearchMsg}}){}

void* Compile::PchCode(const char* CodeName,void* P,int Len){
	return nullptr;
}

bool Compile::ParseCode(const char* CodeName,void* P,int Len,void* Pch){
	//cout<<"Compile"<<(int)&CondReady<<endl;
	CompileThread = thread{
		[=]{
			Invocation->RunCode(CodeName,static_cast<char*>(P),Len,vector<string>{"-std=c++1y","-c"});
		}
	};
	CompileThread.detach();
	return true;
}

bool Compile::SearchAST(const string& Name){
	//unique_lock<mutex> lock{MutReady};
	SearchMsg.nLine=32;
	SearchMsg.nChar=2;
	SearchMsg.Key=Name;
	CondSearch.notify_all();
	return true;
}
Compile clast;
}//namespace
