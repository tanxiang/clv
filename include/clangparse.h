#pragma once
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


namespace clp{
class Consumer:public clang::ASTConsumer,public clang::RecursiveASTVisitor<Consumer>{
	typedef clang::RecursiveASTVisitor<Consumer> base;
	std::condition_variable& CondReady;
	std::condition_variable& CondSearch;
	//clv::MsgBox& SearchMsg;
	//std::mutex& MutReady;
	std::mutex MutSearch;
	clang::ASTContext *pContext;
	std::string FilterString;

	std::string getName(clang::Decl *D) {
		if (clang::isa<clang::NamedDecl>(D))
			return clang::cast<clang::NamedDecl>(D)->getQualifiedNameAsString();
		return "";
	}

	bool filterMatches(clang::Decl *D) {
		return getName(D).find(FilterString) != std::string::npos;
	}

public:
	Consumer(std::condition_variable& CondReady,std::condition_variable& CondSearch)
	:CondReady(CondReady),CondSearch(CondSearch){}

	virtual void HandleTranslationUnit(clang::ASTContext &Context) override;

// ----------------- Decl traversal -----------------
//
// For a Decl, we automate (in the DEF_TRAVERSE_DECL macro) traversing
// the children that come from the DeclContext associated with it.
// Therefore each Traverse* only needs to worry about children other
// than those.
//bool TraverseDeclContextHelper(DeclContext *DC);
	//bool TraverseDecl(Decl *D);
	//AST中各种声明Node访问者方法
	bool VisitDecl(clang::Decl *Declaration);

	//bool VisitNamedDecl(NamedDecl *Declaration);
	/*
	void VisitDeclContext(DeclContext *DC, bool Indent) //?
	void VisitTypedefDecl(TypedefDecl *D) //typedef ??
	void VisitTypeAliasDecl(TypeAliasDecl *D) //using ?? as ??
	void VisitEnumDecl(EnumDecl *D) //
	*/
	bool VisitFunctionDecl(clang::FunctionDecl *Declaration); 
	bool VisitFieldDecl(clang::FieldDecl *Declaration);
	/*
	void VisitLabelDecl(LabelDecl *D) // goto lable
	*/
	bool VisitVarDecl(clang::VarDecl *Declaration); //var

	/*
	void VisitFileScopeAsmDecl(FileScopeAsmDecl *D) // inline asm
	*/

	bool VisitNamespaceDecl(clang::NamespaceDecl *Declaration); //namespace ??
	bool VisitUsingDirectiveDecl(clang::UsingDirectiveDecl *Declaration); //using namespace ??
	bool VisitCXXRecordDecl(clang::CXXRecordDecl *Declaration); //class

	
	bool VisitLinkageSpecDecl(clang::LinkageSpecDecl *Declaration); //cpp link symb
	bool VisitTemplateDecl(clang::TemplateDecl *Declaration);
	
	bool VisitFunctionTemplateDecl(clang::FunctionTemplateDecl *Declaration);
	bool VisitClassTemplateDecl(clang::ClassTemplateDecl *Declaration);

	//暂无objc特性支持计划
	
	//语义node访问方法
	bool VisitStmt(clang::Stmt *Statement);
	bool VisitDeclStmt(clang::DeclStmt *Statement);
	//表达式node访问
	bool VisitMemberExpr(clang::MemberExpr *Expr);
	bool VisitCallExpr(clang::CallExpr *expr);//
	//type node访问
	bool VisitType(clang::Type *Typeinfo);
	bool VisitTypeLoc(clang::TypeLoc TL);
private:
	template <typename AstNode>
	bool IsInDecl(AstNode *Node);
};

class Action:public clang::ASTFrontendAction{
	std::condition_variable& CondReady;
	std::condition_variable& CondSearch;
	//clv::MsgBox& SearchMsg;
	//std::mutex& MutReady;
	//std::mutex& MutSearch;
protected:
	virtual clang::ASTConsumer *CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef InFile){
		//std::cout<<"cASTConsumer"<<std::endl;
		return new Consumer{CondReady,CondSearch};
		//return nullptr;
	}
	void ExecuteAction();
public:
	Action(std::condition_variable& CondReady,std::condition_variable& CondSearch)
	:CondReady(CondReady),CondSearch(CondSearch){}
	virtual bool hasCodeCompletionSupport() const {return true;}
	virtual bool hasPCHSupport() const { return false; }
	virtual bool hasASTFileSupport() const { return true; }
	virtual bool hasIRSupport() const { return false; }
};

class Invocation{
	//std::vector<std::string> CommandLine;
	std::unique_ptr<clang::FrontendAction> Action;
	//FIXME: dconstruct call a purge virtual func when unique_ptr delete the CompilerInstance??
	std::unique_ptr<clang::CompilerInstance> Compiler{new clang::CompilerInstance{}};
	//std::unique_ptr<CompilerInstance> Compiler;
protected:
	//bool RunInvocation(const char* Name,char* Code,int Length);
public:
	Invocation(clang::FrontendAction *Action):Action{Action}{}

	bool RunCode(const char* Name,char* Code,int Length,std::vector<std::string> CommandLine);
};
//void CodeToCompilerInstance(const char* Name,char* Code,int Length,CompilerInstance &Compiler);
}//namespace

