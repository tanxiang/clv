#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include "clvfile.h"
#include "clangparse.h"

using namespace std;
/*
class FileTex:public llvm::Twine{
public:
	template<typename Iterator>
	FileTex(Iterator first,Iterator last){
		while(first!=last){
			LHSKind=StdStringKind;
			LHS.stdString=*first;
			if(++first==last)
				RHSKind=EmptyKind;
			else{
				RHSKind=TwineKind;
				LHS.twine=new FileTex(first,last);
			}
		}
	}
};
*/

int main(int argc,char* argv[])
{
	ios::sync_with_stdio(false);
	if(argc!=2)
		return 0;
	ifstream ifs_in{argv[1]};
	FileText text{istream_iterator<Line>{ifs_in},istream_iterator<Line>{}};
	//runToolOnCodeWithArgs(new ClpAction,static_cast<vector<string> >(text),
	//std::vector<std::string> {"-std=c++11"});
	//copy(text.begin(),text.end(),ostream_iterator<Line>{cout,"\n"});
}
