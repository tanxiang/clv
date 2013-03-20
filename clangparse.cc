#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS
#include <clang/Frontend/FrontendActions.h>
#include <clang/Frontend/ASTConsumers.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>

using namespace clang::tooling;

//CommonOptionsParser OptionsParser(argc, argv);

int main(int argc,const char** argv){
	//CommonOptionsParser OptionsParser(argc, argv);
	//ClangTool Tool(OptionsParser.GetCompilations(),OptionsParser.GetSourcePathList());
	//return Tool.run(newFrontendActionFactory<clang::SyntaxOnlyAction>());
	//runToolOnCode( );
	
	//vs_args.push_back("-std=c++11");
	runToolOnCodeWithArgs(new clang::SyntaxOnlyAction,"class X {};",std::vector<std::string> {1,"-std=c++11"});
}
