#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <iterator>
#include "filemap.h"

template<typename LineRef>
FileMap<LineRef>::FileMap(const char* FilePath){
	FD = -1;
	if(FilePath){
		FD=open(FilePath,O_RDWR);
		if(FD == -1){
			throw;
		}
		struct stat sb;
		if(fstat(FD,&sb) == -1){
			close(FD);
			throw;
		}
		Len=sb.st_size;
		P = mmap(NULL,Len,PROT_READ|PROT_WRITE,MAP_SHARED,FD,0);
	}
	else{
		Len=1024;
		P = mmap(NULL,Len,PROT_READ|PROT_WRITE,MAP_SHARED,FD,0);
	}
	if(P == MAP_FAILED){
		close(FD);
		throw;
	}

	//static_cast<char*>(P)[Len]='\n';
	start.Line.Set(P);
	auto l=Len;
	finish=start;
	//l-=finish->Length();

	while(l){
		//std::cout<<finish->Length()<<':'<<l<<'-'<<std::endl;
		l-=finish->Length();
		++finish;
	}
}


template<typename LineRef>
FileMap<LineRef>::~FileMap(){
	munmap(P,Len+1);
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
template class FileMap<MBLineRef<CharRef> >;
template class WCLineRef<wchar_t>;
