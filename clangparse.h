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

	ASTContext *pContext;

	public:
	ClpConsumer(){}

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
	*/

	bool VisitNamespaceDecl(NamespaceDecl *D); //namespace ??
	bool VisitUsingDirectiveDecl(UsingDirectiveDecl *D); //using namespace ??
	bool VisitCXXRecordDecl(CXXRecordDecl *Declaration); //class

	
	bool VisitLinkageSpecDecl(LinkageSpecDecl *D); //cpp link symb
	bool VisitTemplateDecl(const TemplateDecl *D);
	
	bool VisitFunctionTemplateDecl(FunctionTemplateDecl *D);
	bool VisitClassTemplateDecl(ClassTemplateDecl *D);

	//暂无objc特性支持计划
	
	//表达式node访问

	bool VisitCallExpr(CallExpr *expr);//
	//语义node访问方法
	
};

class ClpAction:public ASTFrontendAction{

protected:
	virtual ASTConsumer *CreateASTConsumer(CompilerInstance &CI, llvm::StringRef InFile){
		//std::cout<<"cASTConsumer"<<std::endl;
		return new ClpConsumer{};
		//return nullptr;
	} 
public:
	ClpAction(){}
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
