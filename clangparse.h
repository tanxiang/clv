#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS
#include <clang/Frontend/FrontendActions.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/Frontend/ASTConsumers.h>
#include <clang/Frontend/CompilerInstance.h>

#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <memory>
using namespace clang::tooling;
using namespace clang;

class ClpConsumer:public ASTConsumer,public RecursiveASTVisitor<ClpConsumer>{

};

class ClpAction:public ASTFrontendAction{
protected:
	virtual ASTConsumer *CreateASTConsumer(CompilerInstance &CI, llvm::StringRef InFile); 
public:
	virtual bool hasCodeCompletionSupport() const { return true; }
};

class ClpInvocation{
	std::vector<std::string> CommandLine;
	std::unique_ptr<FrontendAction> ClpAction;
protected:

public:
	ClpInvocation(std::vector<std::string> CommandLine,FrontendAction *ClpAction)
	:CommandLine{CommandLine},ClpAction{ClpAction}{
	}
	bool RunCode(std::vector<std::string> Code);
};
