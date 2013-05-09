#include <iostream>
#include "clthread.h"
#include "clangparse.h"

using namespace std;
ClvCompile::ClvCompile(){}

void* ClvCompile::PchCode(const char* CodeName,void* P,int Len){
	return nullptr;
}

bool ClvCompile::ParseCode(const char* CodeName,void* P,int Len,void* Pch){
	CompileThread = thread{
		[=](condition_variable& CondReady,condition_variable& CondSearch){
			ClpInvocation Invocation{new ClpAction{CondReady,CondSearch}};
			Invocation.RunCode(CodeName,static_cast<char*>(P),Len,vector<string>{"-std=c++11","-c"});
			},
			ref(CondReady),ref(CondSearch)
		};
	CompileThread.detach();
	return true;
}

bool ClvCompile::SearchAST(){
	//unique_lock<mutex> lock{MutReady};
	CondSearch.notify_all();
}
ClvCompile clast;
