#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include "clvfile.h"

using namespace std;

int main(int argc,char* argv[])
{
	ios::sync_with_stdio(false);
	if(argc!=2)
		return 0;
	ifstream ifs_in{argv[1]};
	FileText text{istream_iterator<Line>{ifs_in},istream_iterator<Line>{}};
	copy(text.begin(),text.end(),ostream_iterator<Line>{cout,"\n"});
}
