#include "clthread.h"
#include "clangparse.h"
#include <clang/Frontend/FrontendDiagnostic.h>
#include <clang/Sema/CodeCompleteConsumer.h>
//#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Driver/Compilation.h>
#include <llvm/Support/Host.h>
#include <iostream>

using namespace std;
using namespace clang::tooling;

class ClvCodeCompleteConsumer : public CodeCompleteConsumer {};

/*
#define DISPATCH(NAME, CLASS, VAR) \
  return getDerived().Traverse##NAME(static_cast<CLASS*>(VAR))
bool ClpConsumer::TraverseDecl(Decl *D) {
cout<<"1search:"<<SearchMsg.Key<<endl;
  if (!D)
    return true;

  // As a syntax visitor, by default we want to ignore declarations for
  // implicit declarations (ones not typed explicitly by the user).
  if (!getDerived().shouldVisitImplicitCode() && D->isImplicit())
    return true;
cout<<"2search:"<<SearchMsg.Key<<endl;
  switch (D->getKind()) {
#define ABSTRACT_DECL(DECL)
#define DECL(CLASS, BASE) \
  case Decl::CLASS: DISPATCH(CLASS##Decl, CLASS##Decl, D);
#include "clang/AST/DeclNodes.inc"
 }
cout<<"3search:"<<SearchMsg.Key<<endl;
  return true;
}
*/
void ClpConsumer::HandleTranslationUnit(ASTContext &Context){
	//sleep & wait sreach opt
	//cout<<"ClpConsumer"<<(int)&CondReady<<endl;
	pContext = &Context;
	unique_lock<mutex> lock{MutSearch};
	CondSearch.wait(lock);
	while(SearchMsg.Key!="$"){//search cond
		//Context.getTranslationUnitDecl()->dump(llvm::outs());
		TraverseDecl(Context.getTranslationUnitDecl());//search ast
		CondSearch.wait(lock);
	}
}
/*
bool ClpConsumer::VisitNamedDecl(NamedDecl *Declaration) {
	cout<<"Name:"<<Declaration->getNameAsString()<<'\n';
	//Declaration->getQualifiedNameAsString();
	//cout << Declaration->getName() << '\n';
	return true;
}
*/
bool ClpConsumer::VisitFunctionDecl(FunctionDecl *Declaration){
	cout<<__PRETTY_FUNCTION__<<endl;
	//if (Declaration->isInlineSpecified())  llvm::outs() << "inline ";
	//if (Declaration->isVirtualAsWritten()) llvm::outs() << "virtual ";
	//if (Declaration->isModulePrivate())    llvm::outs() << "__module_private__ ";
	//switch (Declaration->getStorageClassAsWritten()) {
	//case SC_None: break;
	//case SC_Extern: llvm::outs() << "extern "; break;
	//case SC_Static: llvm::outs() << "static "; break;
	//case SC_PrivateExtern: llvm::outs() << "__private_extern__ "; break;
	//case SC_Auto: case SC_Register: case SC_OpenCLWorkGroupLocal:
	//	llvm_unreachable("invalid for functions");
	//}
	cout << Declaration->getNameInfo().getAsString() << "()\t"
		<< Declaration->getType().getAsString() <<'\n';
	if(IsInDecl(Declaration)){
		auto Location = pContext->getFullLoc(Declaration->getLocStart());
		if (Location.isValid())
			cout << "declaration at FileID=" << Location.getFileID().getHashValue()
				<< "\tLine=" << Location.getSpellingLineNumber() 
				<< "\tColumn=" << Location.getSpellingColumnNumber() << '\n';
	}
	return true;
}

bool ClpConsumer::VisitFieldDecl(FieldDecl *Declaration){
	cout<<__PRETTY_FUNCTION__<<endl;
	cout << Declaration->getNameAsString() << '\t'
		<< Declaration->getType().getAsString() <<'\n';
	return true;
}

bool ClpConsumer::VisitVarDecl(VarDecl *Declaration){
	cout<<__PRETTY_FUNCTION__<<endl;
	llvm::outs() << Declaration->getName() << '\t'
		<< Declaration->getType().getAsString() <<'\n';
	Declaration->getType()->isPointerType();
	Declaration->getType()->isObjectType();
	auto Location = pContext->getFullLoc(Declaration->getLocStart());
	if (Location.isValid())
		cout << "declaration at FileID=" << Location.getFileID().getHashValue()
			<< "\tLine=" << Location.getSpellingLineNumber() 
			<< "\tColumn=" << Location.getSpellingColumnNumber() << '\n';
	IsInDecl(Declaration);
	return true;
}

bool ClpConsumer::VisitNamespaceDecl(NamespaceDecl *Declaration){
	cout<<__PRETTY_FUNCTION__<<endl;
	llvm::outs() << Declaration->getName() << '\t';
	//	<< Declaration->getType().getAsString() <<'\n';
	if(IsInDecl(Declaration)){
		auto Location = pContext->getFullLoc(Declaration->getLocStart());
		if (Location.isValid())
			cout << "declaration at FileID=" << Location.getFileID().getHashValue()
				<< "\tLine=" << Location.getSpellingLineNumber() 
				<< "\tColumn=" << Location.getSpellingColumnNumber() << '\n';
	}
	return true;
}

bool ClpConsumer::VisitUsingDirectiveDecl(UsingDirectiveDecl *Declaration){ //using 
	cout<<__PRETTY_FUNCTION__<<endl;
	//llvm::outs() << Declaration->getName() << "\t";
	if(IsInDecl(Declaration)){
		auto Location = pContext->getFullLoc(Declaration->getLocStart());
		if (Location.isValid())
			cout << "declaration at FileID=" << Location.getFileID().getHashValue()
				<< "\tLine=" << Location.getSpellingLineNumber() 
				<< "\tColumn=" << Location.getSpellingColumnNumber() << '\n';
	}
	return true;
}

bool ClpConsumer::VisitCXXRecordDecl(CXXRecordDecl *Declaration){
	cout<<__PRETTY_FUNCTION__<<endl;
	llvm::outs() << Declaration->getKindName();
	if (Declaration->getIdentifier())
		llvm::outs() << '\t' << *Declaration;
	llvm::outs() << '\n';
	auto Location = pContext->getFullLoc(Declaration->getLocStart());
	if (Location.isValid())
		llvm::outs() << "declaration at FileID=" << Location.getFileID().getHashValue()
			<< "\tLine=" << Location.getSpellingLineNumber() 
			<< "\tColumn=" << Location.getSpellingColumnNumber() << '\n';

	if (Declaration->isCompleteDefinition() && Declaration->getNumBases()){
		for(auto It = Declaration->bases_begin();
			It !=Declaration->bases_end();++It){ //继承关系
			llvm::outs() << '\t';
			llvm::outs() << (It->isVirtual() ?  "V:" : "N:");
			switch(It->getAccessSpecifier()) {
			case AS_public:    llvm::outs() << "public:"; break;
			case AS_protected: llvm::outs() << "protected:"; break;
			case AS_private:   llvm::outs() << "private:"; break;
			case AS_none: break;
			}
			llvm::outs() << It->getType().getAsString();
			llvm::outs() << '\n';
		}
	}
	IsInDecl(Declaration);
	return true;
}

bool ClpConsumer::VisitLinkageSpecDecl(LinkageSpecDecl *Declaration){
	cout<<__PRETTY_FUNCTION__<<endl;
	//llvm::outs() << Declaration->getName() << "\t"
	//	<< Declaration->getType().getAsString() <<'\n';
	if(IsInDecl(Declaration)){
		auto Location = pContext->getFullLoc(Declaration->getLocStart());
		if (Location.isValid())
			cout << "declaration at FileID=" << Location.getFileID().getHashValue()
				<< "\tLine=" << Location.getSpellingLineNumber() 
				<< "\tColumn=" << Location.getSpellingColumnNumber() << '\n';
	}
	return true;
}

bool ClpConsumer::VisitTemplateDecl(TemplateDecl *Declaration){
	cout<<__PRETTY_FUNCTION__<<endl;
	IsInDecl(Declaration);
	return true;
}

bool ClpConsumer::VisitFunctionTemplateDecl(FunctionTemplateDecl *Declaration){
	cout<<__PRETTY_FUNCTION__<<endl;
	cout << Declaration->getTemplatedDecl()->getNameInfo().getAsString() << "()\t"
		<< Declaration->getTemplatedDecl()->getType().getAsString() << '\n';
	if(IsInDecl(Declaration)){
		auto Location = pContext->getFullLoc(Declaration->getLocStart());
		if (Location.isValid())
			cout << "declaration at FileID=" << Location.getFileID().getHashValue()
				<< "\tLine=" << Location.getSpellingLineNumber() 
				<< "\tColumn=" << Location.getSpellingColumnNumber() << '\n';
	}
	return true;
}

bool ClpConsumer::VisitClassTemplateDecl(ClassTemplateDecl *Declaration){
	cout<<__PRETTY_FUNCTION__<<endl;
	cout << Declaration->getTemplatedDecl()->getKindName();
	if(IsInDecl(Declaration)){
		auto Location = pContext->getFullLoc(Declaration->getLocStart());
		if (Location.isValid())
			cout << "declaration at FileID=" << Location.getFileID().getHashValue()
				<< "\tLine=" << Location.getSpellingLineNumber() 
				<< "\tColumn=" << Location.getSpellingColumnNumber() << '\n';
	}
	return true;
}

bool ClpConsumer::VisitCallExpr(CallExpr *expr){
	cout<<__PRETTY_FUNCTION__<<endl;
	//IsInDecl(expr);
	return true;
}

template <typename AstNode>
bool ClpConsumer::IsInDecl(AstNode *Node){
	auto LocationStart = pContext->getFullLoc(Node->getLocStart());
	auto LocationEnd = pContext->getFullLoc(Node->getLocEnd());
	if(LocationStart.isValid()&&LocationEnd.isValid()){
		if(LocationStart.getFileID().getHashValue()==1 && 
			LocationEnd.getFileID().getHashValue()==1){
			if(SearchMsg.nLine<LocationStart.getSpellingLineNumber() ||
				SearchMsg.nLine>LocationEnd.getSpellingLineNumber())
				return false;
			if(SearchMsg.nLine==LocationStart.getSpellingLineNumber() &&
				SearchMsg.nChar<LocationStart.getSpellingColumnNumber())
				return false;
			if(SearchMsg.nLine==LocationEnd.getSpellingLineNumber() &&
				SearchMsg.nChar>LocationEnd.getSpellingColumnNumber())
				return false;
			cout<<"RAGEin\n";
			return true;
		}
	}
	return false;
}

bool ClpConsumer::VisitDecl(Decl *Declaration){
	cout<<__PRETTY_FUNCTION__<<Declaration->getDeclKindName()<<':'<<getName(Declaration)<<endl;
	return true;
}

bool ClpConsumer::VisitStmt(Stmt *Statement){
	cout<<__PRETTY_FUNCTION__<<Statement->getStmtClassName()<<endl;
	return true;
}
bool ClpConsumer::VisitDeclStmt(DeclStmt *Statement){

	return true;
}

bool ClpConsumer::VisitMemberExpr(MemberExpr *Expression){
	cout<<__PRETTY_FUNCTION__<<Expression->getMemberDecl()->getNameAsString()<<endl;
	
	return true;
}

bool ClpConsumer::VisitType(Type *Typeinfo){
	cout<<__PRETTY_FUNCTION__<<Typeinfo->getTypeClassName()<<endl;
	//string s;
	//getAsStringInternal(Typeinfo,s);
	return true;
}

bool ClpConsumer::VisitTypeLoc(TypeLoc TL){
	cout<<__PRETTY_FUNCTION__<<endl;
	return true;
}

bool ClpInvocation::RunCode(const char* Name,char* Code,int Length,std::vector<std::string> CommandLine){
	vector<string> Commands;
	vector<const char*> Argv;
	Commands.push_back("clang-tool");
	Commands.push_back("-fsyntax-only");
	Commands.insert(Commands.end(),CommandLine.begin(),CommandLine.end());
	Commands.push_back(Name);
	for (int I = 0, E = Commands.size(); I != E; ++I)
		Argv.push_back(Commands[I].c_str());

	IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts = new DiagnosticOptions();
	//TextDiagnosticPrinter DiagnosticPrinter(llvm::errs(), &*DiagOpts);
	IntrusiveRefCntPtr<DiagnosticsEngine> Diagnostics = new DiagnosticsEngine{
		IntrusiveRefCntPtr<clang::DiagnosticIDs>{new DiagnosticIDs{}},
		&*DiagOpts,nullptr,false};

	driver::Driver Driver{"clang-tool",llvm::sys::getDefaultTargetTriple(),"a.out",false,*Diagnostics};
	Driver.setCheckInputsExist(false);

	driver::Compilation Compilation{*Driver.BuildCompilation(llvm::makeArrayRef(Argv))};

	const auto &Jobs = Compilation.getJobs();
	//need check job 
	const auto *Cmd = cast<clang::driver::Command>(*Jobs.begin());
	//need check cmd
	auto CC1Args = Cmd->getArguments();
	//need check cc1args

	IntrusiveRefCntPtr<CompilerInvocation> Invocation{new CompilerInvocation{}};
	clang::CompilerInvocation::CreateFromArgs(
		*Invocation, CC1Args.data() + 1, CC1Args.data() + CC1Args.size(),
		*Diagnostics);
	Invocation->getFrontendOpts().DisableFree = false;
	//if("-v"){
	//	Compilation.PrintJob(llvm::errs(), Compilation.getJobs(), "\n", true);
	//	llvm::errs() << "\n";
	//}

	return RunInvocation(Name,Code,Length,*Invocation,CC1Args);
}

bool ClpInvocation::RunInvocation(const char* Name,char* Code,int Length,CompilerInvocation &Invocation,driver::ArgStringList &CC1Args){
	Compiler.setInvocation(&Invocation);

	Compiler.createDiagnostics(CC1Args.size(),const_cast<char**>(CC1Args.data()));
	if (!Compiler.hasDiagnostics())
		return false;
	
	Compiler.createFileManager();
	Compiler.createSourceManager(Compiler.getFileManager());
	CodeToCompilerInstance(Name,Code,Length,Compiler);
	const bool Success = Compiler.ExecuteAction(*Action);
	Compiler.resetAndLeakFileManager();
	return Success;
}

void CodeToCompilerInstance(const char* Name,char* Code,int Length,CompilerInstance &Compiler){
	const llvm::MemoryBuffer *Input = llvm::MemoryBuffer::getMemBuffer(Code);
	const FileEntry *FromFile = Compiler.getFileManager().getVirtualFile(Name,Length, 0);
	Compiler.getSourceManager().overrideFileContents(FromFile,Input);
}

