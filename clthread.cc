#include <iostream>
#include "clthread.h"
#include "clangparse.h"


using namespace std;
//thread gt;
void* ClvCompile::PchCode(const char* CodeName,void* P,int Len){
	return nullptr;
}

bool ClvCompile::ParseCode(const char* CodeName,void* P,int Len,void* Pch){
	CompileThread = thread{[=]{
		ClpInvocation Invocation{new ClpAction{}};
		Invocation.RunCode(CodeName,static_cast<char*>(P),Len,vector<string>{"-std=c++11","-c"});
	}};
	CompileThread.detach();
	return true;
}

bool ClvCompile::SearchAST(){
	return true;
}
ClvCompile clast;
