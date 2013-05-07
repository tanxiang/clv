//#define __STDC_LIMIT_MACROS
//#define __STDC_CONSTANT_MACROS

#include <clang/Frontend/FrontendActions.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Driver/Driver.h>
#include <memory>
#include <future>

using namespace clang;

class ClpConsumer:public ASTConsumer,public RecursiveASTVisitor<ClpConsumer>{
	typedef RecursiveASTVisitor<ClpConsumer> base;
	std::promise<int>& SearchPromise;
	ASTContext *pContext;
	public:
	ClpConsumer(std::promise<int>& Promise):SearchPromise(Promise){}

	virtual void HandleTranslationUnit(ASTContext &Context) override;

	/*virtual bool HandleTopLevelDecl(DeclGroupRef D) override{
		for(DeclGroupRef::iterator It : D)
			TraverseDecl(*It);
	}*/
	//AST中各种声明Node访问者方法
	/*
	void VisitDeclContext(DeclContext *DC, bool Indent) //?
	void VisitTypedefDecl(TypedefDecl *D) //typedef ??
	void VisitTypeAliasDecl(TypeAliasDecl *D) //using ?? as ??
	void VisitEnumDecl(EnumDecl *D) //
	*/
	bool VisitFunctionDecl(FunctionDecl *Declaration); 
	/*
	void VisitFieldDecl(FieldDecl *D) // mutable?? 
	void VisitLabelDecl(LabelDecl *D) // goto lable
	*/
	bool VisitVarDecl(VarDecl *Declaration); //var
	/*
	void VisitFileScopeAsmDecl(FileScopeAsmDecl *D) // inline asm
	bool VisitNamespaceDecl(NamespaceDecl *D) //namespace ??
	void VisitUsingDirectiveDecl(UsingDirectiveDecl *D) //using namespace ??
	*/
	bool VisitCXXRecordDecl(CXXRecordDecl *Declaration);
	/*
	void VisitLinkageSpecDecl(LinkageSpecDecl *D) //cpp link symb
	void VisitTemplateDecl(const TemplateDecl *D);
	void VisitFunctionTemplateDecl(FunctionTemplateDecl *D);
	void VisitClassTemplateDecl(ClassTemplateDecl *D);

	暂无objc特性支持计划
	*/
	//表达式node访问
	bool VisitCallExpr(CallExpr *expr){return true;}
	//语义node访问方法
	
};

class ClpAction:public ASTFrontendAction{
	std::promise<int>& SearchPromise;
protected:
	virtual ASTConsumer *CreateASTConsumer(CompilerInstance &CI, llvm::StringRef InFile){
		//std::cout<<"cASTConsumer"<<std::endl;
		return new ClpConsumer{SearchPromise};
		//return nullptr;
	} 
public:
	ClpAction(std::promise<int>& Promise):SearchPromise(Promise){}
	virtual bool hasCodeCompletionSupport() const {return true;}
};

class ClpInvocation{
	//std::vector<std::string> CommandLine;
	std::unique_ptr<FrontendAction> Action;
protected:
	bool RunInvocation(const char* Name,char* Code,int Length,CompilerInvocation &Invocation,driver::ArgStringList &CC1Args);
public:
	ClpInvocation(FrontendAction *Action):Action{Action}{
	}
	bool RunCode(const char* Name,char* Code,int Length,std::vector<std::string> CommandLine);
};

void CodeToCompilerInstance(const char* Name,char* Code,int Length,CompilerInstance &Compiler);
