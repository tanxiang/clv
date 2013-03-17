#include <iostream>
#include <fstream>
#include <iterator>
#include "clvfile.h"

using namespace std;

int main(int argc,char* argv[])
{
	istream_iterator<string> file_itr(cin);
	file_text text(file_itr,istream_iterator<string>());
};
