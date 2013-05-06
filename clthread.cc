#include <iostream>
#include "clthread.h"
#include "clangparse.h"


using namespace std;
ClvCompile::ClvCompile():SearchResult{SearchPromise.get_future()}{}

void* ClvCompile::PchCode(const char* CodeName,void* P,int Len){
	return nullptr;
}

bool ClvCompile::ParseCode(const char* CodeName,void* P,int Len,void* Pch){
	CompileThread = thread{
		[=](promise<int>& SearchPromise){
			ClpInvocation Invocation{new ClpAction{}};
			Invocation.RunCode(CodeName,static_cast<char*>(P),Len,vector<string>{"-std=c++11","-c"});
			},
			ref(SearchPromise)
		};
	CompileThread.detach();
	return true;
}

bool ClvCompile::SearchAST(){
	if(!Ready())
		return false;
	
}
ClvCompile clast;
