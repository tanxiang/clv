#include "clthread.h"
#include "clangparse.h"
#include <clang/Frontend/FrontendDiagnostic.h>
#include <clang/Lex/Preprocessor.h>
//#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Driver/Compilation.h>
#include <llvm/Support/Host.h>

#include <clang/Parse/ParseAST.h>
//#include "clang/Parse/ParseDiagnostic.h"
#include <clang/Sema/Sema.h>
#include <clang/Parse/Parser.h>
#include <llvm/Support/CrashRecoveryContext.h>
#include <memory>
#include <iostream>

using namespace std;
using namespace clang;
using namespace clang::tooling;

namespace clp{

void CodeCompleteConsumer::ProcessCodeCompleteResults(Sema &S,
					CodeCompletionContext Context,
					CodeCompletionResult *Results,
					unsigned NumResults) {
	cout<<__PRETTY_FUNCTION__<<endl;
	for (unsigned I = 0; I != NumResults; ++I) {
		cout << "COMPLETION: ";
		switch (Results[I].Kind){
		case CodeCompletionResult::RK_Declaration:
			cout <<"D:"<< Results[I].Declaration->getNameAsString()<<'\n';
			break;
		case CodeCompletionResult::RK_Keyword:
			cout <<"K:"<< Results[I].Keyword << '\n';
			break;
		case CodeCompletionResult::RK_Macro:
			cout <<"M:"<< Results[I].Macro->getNameStart();
			break;
		case CodeCompletionResult::RK_Pattern:
			cout <<"P:"<<Results[I].Pattern->getAsString();
			//for(auto R:*Results[I].Pattern)
			//	;
			cout<<endl;
			break;
		};
	}
	return;
}


void Consumer::HandleTranslationUnit(ASTContext &Context){
	//sleep & wait sreach opt
	//cout<<"Consumer"<<(int)&cond_ready<<endl;
	pContext = &Context;
	unique_lock<mutex> lock{MutSearch};
	cond_search.wait(lock);
	/*
	while(SearchMsg.Key!="$"){//search cond
		//Context.getTranslationUnitDecl()->dump(llvm::outs());
		TraverseDecl(Context.getTranslationUnitDecl());//search ast
		cond_search.wait(lock);
	}
	*/
}
/*
bool Consumer::VisitNamedDecl(NamedDecl *Declaration) {
	cout<<"Name:"<<Declaration->getNameAsString()<<'\n';
	//Declaration->getQualifiedNameAsString();
	//cout << Declaration->getName() << '\n';
	return true;
}
*/
bool Consumer::VisitFunctionDecl(FunctionDecl *Declaration){
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

bool Consumer::VisitFieldDecl(FieldDecl *Declaration){
	cout<<__PRETTY_FUNCTION__<<endl;
	cout << Declaration->getNameAsString() << '\t'
		<< Declaration->getType().getAsString() <<'\n';
	return true;
}

bool Consumer::VisitVarDecl(VarDecl *Declaration){
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

bool Consumer::VisitNamespaceDecl(NamespaceDecl *Declaration){
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

bool Consumer::VisitUsingDirectiveDecl(UsingDirectiveDecl *Declaration){ //using 
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

bool Consumer::VisitCXXRecordDecl(CXXRecordDecl *Declaration){
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

bool Consumer::VisitLinkageSpecDecl(LinkageSpecDecl *Declaration){
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

bool Consumer::VisitTemplateDecl(TemplateDecl *Declaration){
	//cout<<__PRETTY_FUNCTION__<<endl;
	//IsInDecl(Declaration);
	return true;
}

bool Consumer::VisitFunctionTemplateDecl(FunctionTemplateDecl *Declaration){
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

bool Consumer::VisitClassTemplateDecl(ClassTemplateDecl *Declaration){
	cout<<__PRETTY_FUNCTION__<<endl;
	//cout << Declaration->getTemplatedDecl()->getKindName();
	if(IsInDecl(Declaration)){
		auto Location = pContext->getFullLoc(Declaration->getLocStart());
		if (Location.isValid())
			cout << "declaration at FileID=" << Location.getFileID().getHashValue()
				<< "\tLine=" << Location.getSpellingLineNumber() 
				<< "\tColumn=" << Location.getSpellingColumnNumber() << '\n';
	}
	return true;
}

bool Consumer::VisitCallExpr(CallExpr *expr){
	//cout<<__PRETTY_FUNCTION__<<endl;
	//IsInDecl(expr);
	return true;
}

template <typename AstNode>
bool Consumer::IsInDecl(AstNode *Node){
	auto LocationStart = pContext->getFullLoc(Node->getLocStart());
	auto LocationEnd = pContext->getFullLoc(Node->getLocEnd());
	if(LocationStart.isValid()&&LocationEnd.isValid()){
		if(LocationStart.getFileID().getHashValue()==1 && 
			LocationEnd.getFileID().getHashValue()==1){
			/*
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
			*/
			return true;
		}
	}
	return false;
}

bool Consumer::VisitDecl(Decl *Declaration){
	cout<<__PRETTY_FUNCTION__<<Declaration->getDeclKindName()<<':'<<getName(Declaration)<<endl;
	return true;
}

bool Consumer::VisitStmt(Stmt *Statement){
	cout<<__PRETTY_FUNCTION__<<Statement->getStmtClassName()<<endl;
	return true;
}
bool Consumer::VisitDeclStmt(DeclStmt *Statement){

	return true;
}

bool Consumer::VisitMemberExpr(MemberExpr *Expression){
	//cout<<__PRETTY_FUNCTION__<<Expression->getMemberDecl()->getNameAsString()<<endl;
	
	return true;
}

bool Consumer::VisitType(Type *Typeinfo){
	cout<<__PRETTY_FUNCTION__<<Typeinfo->getTypeClassName()<<endl;
	//string s;
	//getAsStringInternal(Typeinfo,s);
	return true;
}

bool Consumer::VisitTypeLoc(TypeLoc TL){
	cout<<__PRETTY_FUNCTION__<<endl;
	return true;
}

static bool EnableCodeCompletion(CompilerInvocation &Invocation,
		CompilerInstance &Compiler,
		const char* Filename,
		unsigned Line,
		unsigned Column) {
	//cout<<"start"<<__PRETTY_FUNCTION__<<endl;
	// Tell the source manager to chop off the given file at a specific
	// line and column.
	auto &FrontendOpts = Invocation.getFrontendOpts();
	auto &CodeCompleteOpts = FrontendOpts.CodeCompleteOpts;
	//FIXME need set Preprocessor Completion support
	//auto &PreprocessorOpts = Invocation.getPreprocessorOpts();
	//const FileEntry *Entry = Compiler.getPreprocessor().getFileManager().getFile(Filename);
	//if (!Entry) {
	//	cout<<"error"<<__PRETTY_FUNCTION__<<endl;
	//	return true;
	//}
	// Truncate the named file at the given line/column.
	//Compiler.getPreprocessor().SetCodeCompletionPoint(Entry, Line, Column);

	FrontendOpts.CodeCompletionAt.FileName = Filename;
	FrontendOpts.CodeCompletionAt.Line = Line;
	FrontendOpts.CodeCompletionAt.Column = Column;

	CodeCompleteConsumer *ClvCompleteConsumer = new clp::CodeCompleteConsumer{CodeCompleteOpts};

	Compiler.setCodeCompletionConsumer(ClvCompleteConsumer);

	//cout<<"done"<<__PRETTY_FUNCTION__<<endl;
	return false;
}

void CodeToCompilerInstance(const char* Name,char* Code,int Length,CompilerInstance &Compiler){
	auto *Input = llvm::MemoryBuffer::getMemBuffer(Code);
	auto *FromFile = Compiler.getFileManager().getVirtualFile(Name,Length, 0);
	Compiler.getSourceManager().overrideFileContents(FromFile,Input);
}

bool Invocation::RunCode(const char* Name,char* Code,int Length,std::vector<std::string> CommandLine){
	vector<string> Commands;
	vector<const char*> Argv;
	Commands.push_back("clang-tool");
	Commands.push_back("-fsyntax-only");
	Commands.insert(Commands.end(),CommandLine.begin(),CommandLine.end());
	Commands.push_back(Name);
	for (int I = 0, E = Commands.size(); I != E; ++I)
		Argv.push_back(Commands[I].c_str());
	//Recover resources if we crash before exiting this method.
	//llvm::CrashRecoveryContextCleanupRegistrar<CompilerInstance> CICleanup(Compiler->get());
	IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts = new DiagnosticOptions();
	//TextDiagnosticPrinter DiagnosticPrinter(llvm::errs(), &*DiagOpts);
	IntrusiveRefCntPtr<DiagnosticsEngine> Diagnostics = new DiagnosticsEngine{
		IntrusiveRefCntPtr<clang::DiagnosticIDs>{new DiagnosticIDs{}},
		&*DiagOpts,nullptr,false};
	//FIXME  clang::ProcessWarningOptions & use Diagnostics

	driver::Driver Driver{"clang-tool","a.out",*Diagnostics};
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
	auto LangOpts = Invocation->getLangOpts();
	Compiler->setInvocation(Invocation.get());
	
	Compiler->createDiagnostics();
	if (!Compiler->hasDiagnostics())
		return false;

	Compiler->createFileManager();
	Compiler->createSourceManager(Compiler->getFileManager());
	//cout<<"createPreprocessor"<<__PRETTY_FUNCTION__<<endl;
	//Compiler->createPreprocessor();
	//cout<<"createPreprocessor end "<<__PRETTY_FUNCTION__<<endl;
	CodeToCompilerInstance(Name,Code,Length,*Compiler);
	EnableCodeCompletion(*Invocation,*Compiler,Name,32,5);

	const bool Success = Compiler->ExecuteAction(*Action);
	Compiler->resetAndLeakFileManager();

	return Success;
}


void MyParseAST(Sema &S, bool PrintStats, bool SkipFunctionBodies) {
cout<<__FUNCTION__<<" start"<<endl;
  // Collect global stats on Decls/Stmts (until we have a module streamer).
  if (PrintStats) {
    Decl::EnableStatistics();
    Stmt::EnableStatistics();
  }

  // Also turn on collection of stats inside of the Sema object.
  bool OldCollectStats = PrintStats;
  std::swap(OldCollectStats, S.CollectStats);

  ASTConsumer *Consumer = &S.getASTConsumer();

  std::unique_ptr<Parser> ParseOP(new Parser(S.getPreprocessor(), S,
                                       SkipFunctionBodies));
  Parser &P = *ParseOP.get();

  //PrettyStackTraceParserEntry CrashInfo(P);
  // Recover resources if we crash before exiting this method.
  llvm::CrashRecoveryContextCleanupRegistrar<Parser>
    CleanupParser(ParseOP.get());

  S.getPreprocessor().EnterMainSourceFile();
  P.Initialize();

  // C11 6.9p1 says translation units must have at least one top-level
  // declaration. C++ doesn't have this restriction. We also don't want to
  // complain if we have a precompiled header, although technically if the PCH
  // is empty we should still emit the (pedantic) diagnostic.
  Parser::DeclGroupPtrTy ADecl;
  ExternalASTSource *External = S.getASTContext().getExternalSource();
cout<<__FUNCTION__<<" StartTranslationUnit"<<endl;
  if (External)
    External->StartTranslationUnit(Consumer);

    do {
      // If we got a null return and something *was* parsed, ignore it.  This
      // is due to a top-level semicolon, an action override, or a parse error
      // skipping something.
      if (ADecl && !Consumer->HandleTopLevelDecl(ADecl.get()))
	return;
      if (S.getPreprocessor().isCodeCompletionReached()) 
        cout<<"CodeCompletionReached"<<endl;
      if(P.getCurToken().is(tok::code_completion))
	cout<<"is code_completion"<<endl;
      //if (S.getPreprocessor().IsFileLexer())
	//cout<<"is FileLexer"<<endl;
      //if (S.getPreprocessor().isIncrementalProcessingEnabled())
	//cout<<"ConsumeToken();"<<endl;
      cout<<__FUNCTION__<<" 1ParseTopLevelDecl:"<<P.getCurToken().getKind()<<endl;
    } while (!P.ParseTopLevelDecl(ADecl));

  // Process any TopLevelDecls generated by #pragma weak.
  for (auto It : S.WeakTopLevelDecls())
    Consumer->HandleTopLevelDecl(DeclGroupRef(It));

cout<<__FUNCTION__<<"HandleTranslationUnit"<<endl;
  Consumer->HandleTranslationUnit(S.getASTContext());
cout<<__FUNCTION__<<"HandleTranslationUnit"<<" end"<<endl;
  std::swap(OldCollectStats, S.CollectStats);
  if (PrintStats) {
    llvm::errs() << "\nSTATISTICS:\n";
    P.getActions().PrintStats();
    S.getASTContext().PrintStats();
    Decl::PrintStats();
    Stmt::PrintStats();
    Consumer->PrintStats();
  }
}

void Action::ExecuteAction() {
	cout<<__PRETTY_FUNCTION__<<" start"<<endl;
	CompilerInstance &CI = getCompilerInstance();

	// FIXME: Move the truncation aspect of this into Sema, we delayed this till
	// here so the source manager would be initialized.
	if (hasCodeCompletionSupport() &&
		!CI.getFrontendOpts().CodeCompletionAt.FileName.empty() &&
		!CI.hasCodeCompletionConsumer()){
		CI.createCodeCompletionConsumer();
		cout<<"CI.createCodeCompletionConsumer"<<endl;
	}
	else{
		auto const &Loc = CI.getFrontendOpts().CodeCompletionAt;
		auto &PP = CI.getPreprocessor();
		PP.SetCodeCompletionPoint( PP.getFileManager().getFile(Loc.FileName), Loc.Line, Loc.Column);
	}

	// Use a code completion consumer?
	if (CI.hasCodeCompletionConsumer()){
		auto CompletionConsumer = &CI.getCodeCompletionConsumer();
		if (!CI.hasSema()){
			CI.createSema(getTranslationUnitKind(), CompletionConsumer);
			cout<<"CI.createSema"<<endl;
		}
	}
	//Decl::PrintStats();
	//Stmt::PrintStats();
	MyParseAST(CI.getSema(), CI.getFrontendOpts().ShowStats,CI.getFrontendOpts().SkipFunctionBodies);
	//ParseAST(CI.getSema(), true,CI.getFrontendOpts().SkipFunctionBodies);
	cout<<__PRETTY_FUNCTION__<<" done"<<endl;
}

}//namespace
