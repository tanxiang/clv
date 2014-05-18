#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <iterator>
#include "filemap.h"

namespace clv{

template<typename LineRef>
FileMap<LineRef>::FileMap(const char* FilePath){
	FD = -1;
	if(FilePath){
		//file_name=FilePath;
		if ((FD = open(FilePath, O_RDWR, 0)) == -1)
			throw;
		struct stat sb;
		if(fstat(FD,&sb) == -1){
			close(FD);
			throw;
		}
		Len=sb.st_size;
		P = mmap(NULL,Len,PROT_READ|PROT_WRITE,MAP_FILE|MAP_SHARED,FD,0);
		//static_cast<char*>(P)[Len]='\n';
		start.Line.Set(P,Len);
		//auto l=Len;
		finish=start;
		//l-=finish->Length();
		while(finish->AllLength()){
			//std::cout<<finish->Length()<<':'<<l<<'-'<<std::endl;
			//l-=finish->Length();
			++finish;
		}
	}
	else{
		Len=getpagesize();
		//P = mmap(NULL,Len,PROT_READ|PROT_WRITE,MAP_ANON|MAP_SHARED,FD,0);
		if ((FD = open("/dev/zero", O_RDWR, 0)) == -1)
			throw;
		P = mmap(NULL,Len,PROT_READ|PROT_WRITE,MAP_FILE|MAP_SHARED,FD,0);
	}
	if(P == MAP_FAILED){
		close(FD);
		//perror("map alloc failed");
		throw;
	}
}


template<typename LineRef>
FileMap<LineRef>::~FileMap(){
	munmap(P,Len);
	close(FD);
}


//template FileMap<char>::FileMap(const char* FilePath);
//template FileMap<char>::~FileMap();

template<typename LineRef>
int FileMap<LineRef>::Merge(){
	return msync(P,Len,MS_SYNC);
}
/*
template<typename LineRef>
FileMap<LineRef>::iterator FileMap<LineRef>::begin(){
	return nullptr;
}

template<typename LineRef>
FileMap<LineRef>::iterator FileMap<LineRef>::end(){
	return nullptr;
}
*/
//template class FileMap<char>;
template class FileMap<MBLineRef<Utf8CharRef> >;
template class WCLineRef<wchar_t>;
}//namespace clv