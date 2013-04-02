#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include "filemap.h"
#include "clvfile.h"
#include "clangparse.h"

using namespace std;

int main(int argc,char* argv[])
{
	ios::sync_with_stdio(false);
	if(argc!=2)
		return 0;
	//ifstream ifs_in{argv[1]};
	//FileText text{istream_iterator<Line>{ifs_in},istream_iterator<Line>{}};
	//runToolOnCodeWithArgs(new ClpAction,static_cast<vector<string> >(text),
	//std::vector<std::string> {"-std=c++11"});
	//copy(text.begin(),text.end(),ostream_iterator<Line>{cout,"\n"});
	FileMap Map{argv[1]};
	cout<<static_cast<char*>(Map.Get());
	ClpInvocation Invocation{vector<string>{"-std=c++11","-v"},new ClpAction{}};
	Invocation.RunCode(static_cast<char*>(Map.Get()),Map.GetLength());
	char c;
	cin>>c;
}
