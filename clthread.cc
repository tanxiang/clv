#include <iostream>
#include "clthread.h"
#include "clangparse.h"


using namespace std;
//thread gt;


void parse_thread(const char* CodeName,void* P,int Len)
{
	//if(gt.joinable())
	//	cout<<"canjoin";
	//thread clangt{cl,P,Len};
	cout<<CodeName<<endl;
	thread clangt{[=]{
		ClpInvocation Invocation{new ClpAction{}};
		Invocation.RunCode(CodeName,static_cast<char*>(P),Len,vector<string>{"-std=c++11","-c"});
	}};
	//gt=move(clangt);
	//if(gt.joinable())
	//	cout<<"canjoin2\n";
	//gt.detach();

	clangt.detach();
	//cout << "Hello, Thread!\n";
	//this_thread::sleep_for(std::chrono::seconds(6));
	//clangt.detach();
}
