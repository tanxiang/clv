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
	file_text text{istream_iterator<line>{ifs_in},istream_iterator<line>{}};
	copy(text.begin(),text.end(),ostream_iterator<line>{cout,"\n"});
}
