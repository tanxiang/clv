#include "clangparse.h"
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendDiagnostic.h>
//#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Driver/Compilation.h>
#include <llvm/Support/Host.h>
#include <iostream>

using namespace std;
using namespace clang::tooling;

void ClpConsumer::HandleTranslationUnit(ASTContext &Context){
	//Context.getTranslationUnitDecl()->dump(llvm::outs());
	//sleep & wait sreach opt
	pContext = &Context;
	TraverseDecl(Context.getTranslationUnitDecl());
}

bool ClpConsumer::VisitFunctionDecl(FunctionDecl *Declaration){
	cout<<__PRETTY_FUNCTION__<<endl;
	if (Declaration->isInlineSpecified())  llvm::outs() << "inline ";
	if (Declaration->isVirtualAsWritten()) llvm::outs() << "virtual ";
	if (Declaration->isModulePrivate())    llvm::outs() << "__module_private__ ";
	switch (Declaration->getStorageClassAsWritten()) {
	case SC_None: break;
	case SC_Extern: llvm::outs() << "extern "; break;
	case SC_Static: llvm::outs() << "static "; break;
	case SC_PrivateExtern: llvm::outs() << "__private_extern__ "; break;
	case SC_Auto: case SC_Register: case SC_OpenCLWorkGroupLocal:
		llvm_unreachable("invalid for functions");
	}
	llvm::outs() << Declaration->getNameInfo().getAsString() << "()\t"
		<<Declaration->getType().getAsString() <<'\n';
	auto Location = pContext->getFullLoc(Declaration->getLocStart());
	if (Location.isValid())
		llvm::outs() << "declaration at FileID=" << Location.getFileID().getHashValue()
			<< "\tLine=" << Location.getSpellingLineNumber() 
			<< "\tColumn=" << Location.getSpellingColumnNumber() << '\n';
	return true;
}

bool ClpConsumer::VisitVarDecl(VarDecl *Declaration){
	cout<<__PRETTY_FUNCTION__<<endl;
	llvm::outs() << Declaration->getName() << "\t"
		<< Declaration->getType().getAsString() <<'\n';
	return true;
}

bool ClpConsumer::VisitNamespaceDecl(NamespaceDecl *D){
	cout<<__PRETTY_FUNCTION__<<endl;
	return true;
}

bool ClpConsumer::VisitUsingDirectiveDecl(UsingDirectiveDecl *D){ //using 
	cout<<__PRETTY_FUNCTION__<<endl;
	return true;
}

bool ClpConsumer::VisitCXXRecordDecl(CXXRecordDecl *Declaration){
	cout<<__PRETTY_FUNCTION__<<endl;
	llvm::outs()<< Declaration->getKindName();
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
	return true;
}

bool ClpConsumer::VisitFunctionTemplateDecl(FunctionTemplateDecl *D){
	cout<<__PRETTY_FUNCTION__<<endl;
	return true;
}

bool ClpConsumer::VisitClassTemplateDecl(ClassTemplateDecl *D){
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

	IntrusiveRefCntPtr<CompilerInvocation> Invocation=new CompilerInvocation{};
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
	clang::CompilerInstance Compiler;
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

