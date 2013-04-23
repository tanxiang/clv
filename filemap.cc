#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "filemap.h"

#include <iostream>

FileMap::FileMap(const char* FilePath){
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
		Length=sb.st_size;
		P = mmap(NULL,Length,PROT_READ|PROT_WRITE,MAP_SHARED,FD,0);
	}
	else{
		Length=1024;
		P = mmap(NULL,Length,PROT_READ|PROT_WRITE,MAP_SHARED,FD,0);
	}
	if(P == MAP_FAILED){
		close(FD);
		throw;
	}
}

FileMap::~FileMap(){
	munmap(P,Length);
	close(FD);
}

int FileMap::Merge(){
	return msync(P,Length,MS_SYNC);
}

