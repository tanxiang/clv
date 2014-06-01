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
		if ((FD = open(FilePath, O_RDWR|O_CREAT)) < 0){
			perror("open");
			throw;
		}
		struct stat sb;
		if(fstat(FD,&sb) == -1){
			perror("fstat");
			close(FD);
			throw;
		}
		Len=sb.st_size;
		FileMemMap = static_cast<char*>(mmap(NULL,Len,PROT_READ|PROT_WRITE,MAP_FILE|MAP_SHARED,FD,0));
		start.Line.Set(FileMemMap,Len);
		finish.Line.Set(FileMemMap+Len,0);
	}
	else{
		Len = getpagesize();
		//FileMemMap = mmap(NULL,Len,PROT_READ|PROT_WRITE,MAP_ANON|MAP_SHARED,FD,0);
		if ((FD = open("/dev/zero", O_RDWR, 0)) <0){
			perror("open");
		}
		FileMemMap = static_cast<char*>(mmap(NULL,Len,PROT_READ|PROT_WRITE,MAP_FILE|MAP_SHARED,FD,0));
		start.Line.Set(FileMemMap,Len);
		finish=start;
	}
	close(FD);
}


template<typename LineRef>
FileMap<LineRef>::~FileMap(){
	munmap(FileMemMap,Len);
	close(FD);
}


//template FileMap<char>::FileMap(const char* FilePath);
//template FileMap<char>::~FileMap();

template<typename LineRef>
int FileMap<LineRef>::Merge(){
	return msync(FileMemMap,Len,MS_SYNC);
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
//template class FileMap<MBLineRef<Utf8CharRef> >;
template class FileMap<RawLineRef<char> >;
}//namespace clv