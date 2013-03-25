#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS
#include <clang/Frontend/FrontendActions.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/Frontend/ASTConsumers.h>
#include <clang/Frontend/CompilerInstance.h>

#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>

#include <iostream>
using namespace clang::tooling;
using namespace clang;
//CommonOptionsParser OptionsParser(argc, argv);

class ClpConsumer:public ASTConsumer,public RecursiveASTVisitor<ClpConsumer>{

};

class ClpAction:public ASTFrontendAction{
protected:
	virtual ASTConsumer *CreateASTConsumer(CompilerInstance &CI, llvm::StringRef InFile); 
public:
	virtual bool hasCodeCompletionSupport() const { return true; }
};

ASTConsumer* ClpAction::CreateASTConsumer(CompilerInstance &CI, llvm::StringRef InFile){
	//std::cout<<"cASTConsumer"<<std::endl;
	return new ClpConsumer;
	//return CreateASTViewer();
}

int main(int argc,const char** argv){
	//CommonOptionsParser OptionsParser(argc, argv);
	//ClangTool Tool(OptionsParser.GetCompilations(),OptionsParser.GetSourcePathList());
	//return Tool.run(newFrontendActionFactory<clang::SyntaxOnlyAction>());
	
	runToolOnCodeWithArgs(new ClpAction,
	" class X {public:\n int in;};X x1{};\nint main(){ x1.in==0;}",
	std::vector<std::string> {"-std=c++11"});
}
