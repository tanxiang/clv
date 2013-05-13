//#define __STDC_LIMIT_MACROS
//#define __STDC_CONSTANT_MACROS

#include <clang/Frontend/FrontendActions.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Driver/Driver.h>
#include <string>
#include <memory>
#include <condition_variable>
#include <thread>

using namespace clang;

class ClpConsumer:public ASTConsumer,public RecursiveASTVisitor<ClpConsumer>{
	typedef RecursiveASTVisitor<ClpConsumer> base;
	std::condition_variable& CondReady;
	std::condition_variable& CondSearch;
	MsgBox& SearchMsg;
	//std::mutex& MutReady;
	std::mutex MutSearch;
	ASTContext *pContext;

	public:
	ClpConsumer(std::condition_variable& CondReady,std::condition_variable& CondSearch,MsgBox& SearchMsg)
	:CondReady(CondReady),CondSearch(CondSearch),SearchMsg(SearchMsg){}

	virtual void HandleTranslationUnit(ASTContext &Context) override;

// ----------------- Decl traversal -----------------
//
// For a Decl, we automate (in the DEF_TRAVERSE_DECL macro) traversing
// the children that come from the DeclContext associated with it.
// Therefore each Traverse* only needs to worry about children other
// than those.
bool TraverseDeclContextHelper(DeclContext *DC);
	//bool TraverseDecl(Decl *D);
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

	bool VisitNamespaceDecl(NamespaceDecl *Declaration); //namespace ??
	bool VisitUsingDirectiveDecl(UsingDirectiveDecl *Declaration); //using namespace ??
	bool VisitCXXRecordDecl(CXXRecordDecl *Declaration); //class

	
	bool VisitLinkageSpecDecl(LinkageSpecDecl *Declaration); //cpp link symb
	bool VisitTemplateDecl(TemplateDecl *Declaration);
	
	bool VisitFunctionTemplateDecl(FunctionTemplateDecl *Declaration);
	bool VisitClassTemplateDecl(ClassTemplateDecl *Declaration);

	//暂无objc特性支持计划
	
	//表达式node访问

	bool VisitCallExpr(CallExpr *expr);//
	//语义node访问方法
	bool IsInDecl(Decl *Declaration);
};

class ClpAction:public ASTFrontendAction{
	std::condition_variable& CondReady;
	std::condition_variable& CondSearch;
	MsgBox& SearchMsg;
	//std::mutex& MutReady;
	//std::mutex& MutSearch;
protected:
	virtual ASTConsumer *CreateASTConsumer(CompilerInstance &CI, llvm::StringRef InFile){
		//std::cout<<"cASTConsumer"<<std::endl;
		return new ClpConsumer{CondReady,CondSearch,SearchMsg};
		//return nullptr;
	} 
public:
	ClpAction(std::condition_variable& CondReady,std::condition_variable& CondSearch,MsgBox& SearchMsg)
	:CondReady(CondReady),CondSearch(CondSearch),SearchMsg(SearchMsg){}
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
