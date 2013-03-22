#!/bin/sh
clang++ --std=c++11 clangparse.cc -L/usr/lib/llvm \
-lclangTooling -lclangFrontend -lclangDriver -lclangParse \
-lclangSema -lclangAST -lclangSerialization -lclangLex -lclangBasic \
-lclangEdit -lclang -lclangAnalysis \
`llvm-config --libs` -ldl -lpthread

