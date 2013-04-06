#include "clangparse.h"
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendDiagnostic.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Tooling/CommonOptionsParser.h>
//#include <clang/Frontend/ASTConsumers.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Driver/Compilation.h>
#include <llvm/Support/Host.h>
#include <iostream>

using namespace std;
using namespace clang::tooling;

ASTConsumer* ClpAction::CreateASTConsumer(CompilerInstance &CI, llvm::StringRef InFile){
	//std::cout<<"cASTConsumer"<<std::endl;
	return new ClpConsumer;
	//return CreateASTViewer();
}

void ClpConsumer::HandleTranslationUnit(ASTContext &Context){
	//Context.getTranslationUnitDecl()->dump(llvm::outs());
	TraverseDecl(Context.getTranslationUnitDecl());
}

bool ClpConsumer::VisitCXXRecordDecl(CXXRecordDecl *Declaration){
	//cout<<endl;
	//Declaration->dump(llvm::outs());
	return true;
}

bool ClpInvocation::RunCode(char* Code,int Length){
	vector<string> Commands;
	vector<const char*> Argv;
	Commands.push_back("clang-tool");
	Commands.push_back("-fsyntax-only");
	Commands.insert(Commands.end(),CommandLine.begin(),CommandLine.end());
	Commands.push_back("input.cc");
	for (int I = 0, E = Commands.size(); I != E; ++I)
		Argv.push_back(Commands[I].c_str());

	IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts = new DiagnosticOptions();
	TextDiagnosticPrinter DiagnosticPrinter(llvm::errs(), &*DiagOpts);
	IntrusiveRefCntPtr<DiagnosticsEngine> Diagnostics = new DiagnosticsEngine{
		IntrusiveRefCntPtr<clang::DiagnosticIDs>{new DiagnosticIDs{}},
		&*DiagOpts,&DiagnosticPrinter,false};

	driver::Driver Driver{"clang-tool",llvm::sys::getDefaultTargetTriple(),"a.out",false,*Diagnostics};
	Driver.setCheckInputsExist(false);
	driver::Compilation Compilation{*Driver.BuildCompilation(llvm::makeArrayRef(Argv))};

	const clang::driver::JobList &Jobs = Compilation.getJobs();
	//need check job 
	const clang::driver::Command *Cmd = cast<clang::driver::Command>(*Jobs.begin());
	//need check cmd
	clang::driver::ArgStringList CC1Args = Cmd->getArguments();
	//need check cc1args

	IntrusiveRefCntPtr<CompilerInvocation> Invocation=new CompilerInvocation{};
	clang::CompilerInvocation::CreateFromArgs(
		*Invocation, CC1Args.data() + 1, CC1Args.data() + CC1Args.size(),
		*Diagnostics);
	Invocation->getFrontendOpts().DisableFree = false;
	Compilation.PrintJob(llvm::errs(), Compilation.getJobs(), "\n", true);
	llvm::errs() << "\n";

	return RunInvocation(Code,Length,*Invocation,CC1Args);
}

bool ClpInvocation::RunInvocation(char* Code,int Length,CompilerInvocation &Invocation,driver::ArgStringList &CC1Args){
	clang::CompilerInstance Compiler;
	Compiler.setInvocation(&Invocation);

	Compiler.createDiagnostics(CC1Args.size(),const_cast<char**>(CC1Args.data()));
	if (!Compiler.hasDiagnostics())
		return false;
	
	Compiler.createFileManager();
	Compiler.createSourceManager(Compiler.getFileManager());
	CodeToCompilerInstance(Code,Length,Compiler);
	const bool Success = Compiler.ExecuteAction(*Action);
	Compiler.resetAndLeakFileManager();
	return Success;
}

void CodeToCompilerInstance(char* Code,int Length,CompilerInstance &Compiler){
	const llvm::MemoryBuffer *Input = llvm::MemoryBuffer::getMemBuffer(Code);
	const FileEntry *FromFile = Compiler.getFileManager().getVirtualFile("input.cc",Length, 0);
	Compiler.getSourceManager().overrideFileContents(FromFile,Input);
}

