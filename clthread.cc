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
		[=](condition_variable& CondReady,condition_variable& CondSearch,mutex& MutReady,mutex& MutSearch){
			ClpInvocation Invocation{new ClpAction{CondReady,CondSearch,MutReady,MutSearch}};
			Invocation.RunCode(CodeName,static_cast<char*>(P),Len,vector<string>{"-std=c++11","-c"});
			},
			ref(CondReady),ref(CondSearch),ref(MutReady),ref(MutSearch)
		};
	CompileThread.detach();
	return true;
}

bool ClvCompile::SearchAST(){
		
}
ClvCompile clast;
