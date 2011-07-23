/* File: ast_decl.cc
 * -----------------
 * Implementation of Decl node classes.
 */
#include "ast_decl.h"
#include "ast_type.h"
#include "ast_stmt.h"

Decl::Decl(Identifier *n) : Node(*n->GetLocation()) {
    Assert(n != NULL);
    (id=n)->SetParent(this);
}

bool Decl::operator==(const Decl &rhs) {
    return *id == *rhs.id;
}

void Decl::Check(List<Scope*> *scopeList) {
    /* TODO: Once all subclasses support this function it should be made a pure
     * virtual function.
     */
    return;
}

VarDecl::VarDecl(Identifier *n, Type *t) : Decl(n) {
    Assert(n != NULL && t != NULL);
    (type=t)->SetParent(this);
}

ClassDecl::ClassDecl(Identifier *n, NamedType *ex, List<NamedType*> *imp, List<Decl*> *m) : Decl(n) {
    // extends can be NULL, impl & mem may be empty lists but cannot be NULL
    Assert(n != NULL && imp != NULL && m != NULL);
    extends = ex;
    if (extends) extends->SetParent(this);
    (implements=imp)->SetParentAll(this);
    (members=m)->SetParentAll(this);
}


InterfaceDecl::InterfaceDecl(Identifier *n, List<Decl*> *m) : Decl(n) {
    Assert(n != NULL && m != NULL);
    (members=m)->SetParentAll(this);
}

FnDecl::FnDecl(Identifier *n, Type *r, List<VarDecl*> *d) : Decl(n) {
    Assert(n != NULL && r!= NULL && d != NULL);
    (returnType=r)->SetParent(this);
    (formals=d)->SetParentAll(this);
    body = NULL;
}

void FnDecl::SetFunctionBody(Stmt *b) {
    (body=b)->SetParent(this);
}

void FnDecl::Check(List<Scope*> *scopeList) {
    Scope *top = scopeList->Nth(scopeList->NumElements()-1);

    if (top->AddUniqDecl(this) != 0)
        return;

    Scope *formalsScope = new Scope;
    if (CheckFormals(formalsScope) != 0)
        return;
    scopeList->Append(formalsScope);

    body->Check(scopeList);

    scopeList->RemoveAt(scopeList->NumElements()-1);
}

int FnDecl::CheckFormals(Scope *formalsScope) {
    for (int i = 0, n = formals->NumElements(); i < n; ++i)
        if (formalsScope->AddUniqDecl(formals->Nth(i)) != 0)
            return 1;

    return 0;
}
