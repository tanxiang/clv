#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS
#include <clang/Frontend/FrontendActions.h>
#include <clang/Frontend/CompilerInvocation.h>

#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Driver/Driver.h>
#include <memory>

using namespace clang;

class ClpConsumer:public ASTConsumer,public RecursiveASTVisitor<ClpConsumer>{
	typedef RecursiveASTVisitor<ClpConsumer> base;
	public:
	virtual void HandleTranslationUnit(ASTContext &Context);

	void HandleTopLevelSingleDecl(Decl *Declaration){};
	//hand
	bool VisitCXXRecordDecl(CXXRecordDecl *Declaration);
};

class ClpAction:public ASTFrontendAction{
protected:
	virtual ASTConsumer *CreateASTConsumer(CompilerInstance &CI, llvm::StringRef InFile); 
public:
	virtual bool hasCodeCompletionSupport() const {return true;}
};

class ClpInvocation{
	std::vector<std::string> CommandLine;
	std::unique_ptr<FrontendAction> Action;
protected:
	bool RunInvocation(char* Code,int Length,CompilerInvocation &Invocation,driver::ArgStringList &CC1Args);
public:
	ClpInvocation(std::vector<std::string> CommandLine,FrontendAction *Action)
	:CommandLine{CommandLine},Action{Action}{
	}
	bool RunCode(char* Code,int Length);
};

void CodeToCompilerInstance(char* Code,int Length,CompilerInstance &Compiler);
