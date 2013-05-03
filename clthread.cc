#include <future>
#include <iostream>
#include "clthread.h"
#include "clangparse.h"

using namespace std;
thread clangt;

void parse_thread(void* P,int Len)
{
	thread clangt{[&]{
		ClpInvocation Invocation{vector<string>{"-std=c++11","-c"},new ClpAction{}};
		Invocation.RunCode(static_cast<char*>(P),Len);
	}};
	//cout<<"pppppp"<<endl;
	clangt.join();
	
}
