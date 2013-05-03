#include <future>

#include "filemap.h"
#include "clthread.h"
#include "clangparse.h"

using namespace std;

void parse_thread(void* P,int Len)
{
	ClpInvocation Invocation{vector<string>{"-std=c++11","-c"},new ClpAction{}};
	Invocation.RunCode(static_cast<char*>(P),Len);
}
