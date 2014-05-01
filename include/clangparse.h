#ifndef _CLANGPARSE_H
#define _CLANGPARSE_H
#include <clang/Frontend/FrontendActions.h>
#include <clang/Frontend/CompilerInstance.h>
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
	std::string FilterString;

	std::string getName(Decl *D) {
		if (isa<NamedDecl>(D))
			return cast<NamedDecl>(D)->getQualifiedNameAsString();
		return "";
	}

	bool filterMatches(Decl *D) {
		return getName(D).find(FilterString) != std::string::npos;
	}

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
//bool TraverseDeclContextHelper(DeclContext *DC);
	//bool TraverseDecl(Decl *D);
	//AST中各种声明Node访问者方法
	bool VisitDecl(Decl *Declaration);

	//bool VisitNamedDecl(NamedDecl *Declaration);
	/*
	void VisitDeclContext(DeclContext *DC, bool Indent) //?
	void VisitTypedefDecl(TypedefDecl *D) //typedef ??
	void VisitTypeAliasDecl(TypeAliasDecl *D) //using ?? as ??
	void VisitEnumDecl(EnumDecl *D) //
	*/
	bool VisitFunctionDecl(FunctionDecl *Declaration); 
	bool VisitFieldDecl(FieldDecl *Declaration);
	/*
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
	
	//语义node访问方法
	bool VisitStmt(Stmt *Statement);
	bool VisitDeclStmt(DeclStmt *Statement);
	//表达式node访问
	bool VisitMemberExpr(MemberExpr *Expr);
	bool VisitCallExpr(CallExpr *expr);//
	//type node访问
	bool VisitType(Type *Typeinfo);
	bool VisitTypeLoc(TypeLoc TL);
private:
	template <typename AstNode>
	bool IsInDecl(AstNode *Node);
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
	void ExecuteAction();
public:
	ClpAction(std::condition_variable& CondReady,std::condition_variable& CondSearch,MsgBox& SearchMsg)
	:CondReady(CondReady),CondSearch(CondSearch),SearchMsg(SearchMsg){}
	virtual bool hasCodeCompletionSupport() const {return true;}
	virtual bool hasPCHSupport() const { return false; }
	virtual bool hasASTFileSupport() const { return true; }
	virtual bool hasIRSupport() const { return false; }
};

class ClpInvocation{
	//std::vector<std::string> CommandLine;
	std::unique_ptr<FrontendAction> Action;
	//FIXME: dconstruct call a purge virtual func when unique_ptr delete the CompilerInstance??
	CompilerInstance* Compiler;
	//std::unique_ptr<CompilerInstance> Compiler;
protected:
	//bool RunInvocation(const char* Name,char* Code,int Length);
public:
	ClpInvocation(FrontendAction *Action):Action{Action},Compiler{new CompilerInstance{}}{}

	bool RunCode(const char* Name,char* Code,int Length,std::vector<std::string> CommandLine);
};

//void CodeToCompilerInstance(const char* Name,char* Code,int Length,CompilerInstance &Compiler);
#endif
