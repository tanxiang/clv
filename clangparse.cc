#include <clang/Frontend/FrontendActions.h>
#include <clang/Frontend/ASTConsumers.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>

using namespace clang::tooling;

CommonOptionsParser OptionsParser(argc, argv);

