#pragma once

#include <unordered_map>
#include <unordered_set>
#include <string>

#include "hindleymilner.hpp"

namespace HindleyMilner {

class Env {
public:
    std::unordered_map<std::string, HindleyMilner::Poly> types;
    std::unordered_set<int> bounded_types;
    bool is_bound(Mono*);
};

class TypeChecker {
    // TODO function check 
    // One of the check function responsabilities is to
    // check all return values inside a function declaration
    // return the same type
    Env env;
    void deduce(TypedAST*);
    void deduce(TypedASTNumberLiteral*);
    void deduce(TypedASTStringLiteral*);
    void deduce(TypedASTBooleanLiteral*);
    void deduce(TypedASTDeclaration*);
    void deduce(TypedASTIdentifier*);
    void deduce(TypedASTFunctionLiteral*);
    void deduce(TypedASTCallExpression*);
    void gather_free_variables(TypedAST*, std::vector<int>&);
    void gather_free_variables(Mono*, std::vector<int>&);
};

}