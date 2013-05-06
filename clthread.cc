#include <iostream>
#include "clthread.h"
#include "clangparse.h"


using namespace std;
ClvCompile::ClvCompile():CompileThread{
	[](const char* CodeName,void* P,int Len){
		ClpInvocation Invocation{new ClpAction{}};
		return Invocation.RunCode(CodeName,static_cast<char*>(P),Len,vector<string>{"-std=c++11","-c"});
	}
}
,CompileResult{CompileThread.get_future()}{}

void* ClvCompile::PchCode(const char* CodeName,void* P,int Len){
	return nullptr;
}

bool ClvCompile::ParseCode(const char* CodeName,void* P,int Len,void* Pch){
	//CompileThread = thread{[=]{
	//	ClpInvocation Invocation{new ClpAction{}};
	//	Invocation.RunCode(CodeName,static_cast<char*>(P),Len,vector<string>{"-std=c++11","-c"});
	//}};
	//CompileThread.detach();
	CompileThread(CodeName,P,Len);
	cout<<"return!!"<<endl;
	CompileResult.get();
	return true;
}

bool ClvCompile::SearchAST(){
	if(!Ready())
		return false;
	
}
ClvCompile clast;
