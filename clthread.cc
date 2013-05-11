#include <iostream>
#include "clthread.h"
#include "clangparse.h"

using namespace std;
ClvCompile::ClvCompile(){}

void* ClvCompile::PchCode(const char* CodeName,void* P,int Len){
	return nullptr;
}

bool ClvCompile::ParseCode(const char* CodeName,void* P,int Len,void* Pch){
	//cout<<"ClvCompile"<<(int)&CondReady<<endl;
	CompileThread = thread{
		[=]{
			ClpInvocation Invocation{new ClpAction{CondReady,CondSearch,SearchMsg}};
			Invocation.RunCode(CodeName,static_cast<char*>(P),Len,vector<string>{"-std=c++11","-c"});
		}
	};
	CompileThread.detach();
	return true;
}

bool ClvCompile::SearchAST(const string& Name){
	//unique_lock<mutex> lock{MutReady};
	SearchMsg.nLine=32;
	SearchMsg.nChar=2;
	SearchMsg.Key=Name;
	CondSearch.notify_all();
	return true;
}
ClvCompile clast;
