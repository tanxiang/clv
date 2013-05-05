#include <future>
#include <iostream>
#include "clthread.h"
#include "clangparse.h"

using namespace std;
thread gt;


void parse_thread(void* P,int Len)
{
	//if(gt.joinable())
	//	cout<<"canjoin";
	//thread clangt{cl,P,Len};
	thread clangt{[=]{
		ClpInvocation Invocation{vector<string>{"-std=c++11","-c"},new ClpAction{}};
		Invocation.RunCode(static_cast<char*>(P),Len);
	}};
	gt=move(clangt);
	//if(gt.joinable())
	//	cout<<"canjoin2\n";
	gt.detach();

	//clangt.detach();
	//cout << "Hello, Thread!\n";
	//this_thread::sleep_for(std::chrono::seconds(6));
	//clangt.detach();
}
