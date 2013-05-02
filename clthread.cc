#include <thread>

#include "filemap.h"
#include "clthread.h"
#include "clangparse.h"

using namespace std;

int clmain(int argc,char* argv[])
{
	ios::sync_with_stdio(false);
	if(argc!=2)
		return 0;
	FileMap<MBLineRef<CharRef> > Map{argv[1]};

	ClpInvocation Invocation{vector<string>{"-std=c++11","-c"},new ClpAction{}};
	Invocation.RunCode(static_cast<char*>(Map.Get()),Map.Length());
}
