#include "TinyJS.h"
#include <string.h>
#include <vector>
#include <cstdlib>
#include <assert.h>

#pragma once

class CSyntaxNode
{
public:
    CSyntaxNode();
    virtual ~CSyntaxNode();

    virtual void emit(std::ostream& out, const std::string indentation = "") = 0;
    // returns true if this node is the kind that should have a semicolon after it.
    // since returns and declarations are statement-types but require semicolons,
    // and the latter of which can appear in for statements, it cannot emit a semicolon
    // itself. plus this is a little more "OO".
    virtual bool semicolonizable() = 0;

protected:
    CSyntaxNode* node;
};

// these two classes serve no purpose except to divide the two
// sides of the syntax tree. 
class CSyntaxStatement : public CSyntaxNode
{
public:
    virtual bool semicolonizable() { return false; }
};
class CSyntaxExpression : public CSyntaxNode
{
public:
    virtual bool semicolonizable() { return true; }
    virtual std::string lvaluePath() { assert(0); return std::string(); }
};

class CSyntaxSequence : public CSyntaxStatement
{
public:
    CSyntaxSequence(CSyntaxNode* front, CSyntaxNode* last);
    ~CSyntaxSequence();

    /// converts the sequence (and any subsequences) into a serial vector of statements.
    /// Using this function will also "disown" all the statements in this sequence if
    /// disown_children is true, ie the leaves of the tree represented by this sequence 
    /// will no longer be deleted when this sequence is deleted.
    std::vector<CSyntaxNode*> normalize(bool disown_children = true);

    CSyntaxNode* first() { return node; }
    CSyntaxNode* second() { return last; }

    virtual void emit(std::ostream& out, const std::string indentation = "");

private:
    CSyntaxNode* last;
    bool disowned;
};

class CSyntaxIf : public CSyntaxStatement
{
public:
    CSyntaxIf(CSyntaxExpression* expr, CSyntaxNode* body, CSyntaxNode* else_);
    CSyntaxIf(CSyntaxExpression* expr, CSyntaxNode* body);
    ~CSyntaxIf();

    virtual void emit(std::ostream& out, const std::string indentation = "");

private:
    CSyntaxExpression* expr;
    CSyntaxNode* else_;
};

class CSyntaxWhile : public CSyntaxStatement
{
public:
    CSyntaxWhile(CSyntaxExpression* expr, CSyntaxNode* body);
    ~CSyntaxWhile();

    virtual void emit(std::ostream& out, const std::string indentation = "");

private:
    CSyntaxExpression* expr;
};

class CSyntaxFor : public CSyntaxStatement
{
public:
    CSyntaxFor(CSyntaxNode* init, CSyntaxExpression* cond, CSyntaxExpression* update, CSyntaxNode* body);
    ~CSyntaxFor();

    virtual void emit(std::ostream& out, const std::string indentation = "");

private:
    CSyntaxNode* init;
    CSyntaxExpression* cond;
    CSyntaxExpression* update;
};

class CSyntaxFactor : public CSyntaxExpression
{
public:
    enum FACTOR_TYPES
    {
        F_TYPE_INT = 1,
        F_TYPE_DOUBLE = 2,
        F_TYPE_STRING = 4,
        F_TYPE_IDENTIFIER = 8
    };
    CSyntaxFactor(std::string val);

    bool isValueType() { return factorType & (F_TYPE_INT | F_TYPE_DOUBLE | F_TYPE_STRING); }
    std::string getRawValue() { return value; }
    double getDouble() { if(factorType != F_TYPE_DOUBLE) return getInt(); return std::strtod(value.c_str(), 0); }
    int getInt() { if(factorType != F_TYPE_INT) return 0; return std::strtol(value.c_str(), 0, 0); }

    virtual void emit(std::ostream& out, const std::string indentation = "");

protected:
    std::string value;
    int factorType;
};

class CSyntaxID : public CSyntaxFactor
{
public:
    CSyntaxID(std::string id);

    virtual void emit(std::ostream& out, const std::string indentation = "");
    std::string getName() { return value; }
    std::string lvaluePath() { return getName(); }
};

class CSyntaxFunction : public CSyntaxExpression
{
public:
    CSyntaxFunction(CSyntaxID* name, std::vector<CSyntaxID*>& arguments, CSyntaxStatement* body);
    ~CSyntaxFunction();

    virtual void emit(std::ostream& out, const std::string indentation = "");
    CSyntaxID* getName();

private:
    CSyntaxID* name;
    std::vector<CSyntaxID*> arguments;

    void generateRandomId();
};

class CSyntaxFunctionCall : public CSyntaxExpression
{
public:
    CSyntaxFunctionCall(CSyntaxExpression* name, std::vector<CSyntaxExpression*> arguments, std::string originalArguments);
    ~CSyntaxFunctionCall();

    virtual void emit(std::ostream& out, const std::string indentation = "");

protected:
    std::vector<CSyntaxExpression*> actuals;
    std::string origString;
};

class CSyntaxReturn : public CSyntaxStatement
{
public:
    CSyntaxReturn(CSyntaxExpression* value);
    virtual void emit(std::ostream& out, const std::string indentation = "");
    virtual bool semicolonizable() { return true; }
};

class CSyntaxAssign : public CSyntaxExpression
{
public:
    // this has to be an expression on the lhs to deal with things like
    //     var some.path.to.thing = something;
    // or things like 
    //     a["myfavoriteindex"] = something;
    // the thing on the lhs is an l-value, but it is an expression that evaluates
    // to one. 
    CSyntaxAssign(int op, CSyntaxExpression* lvalue, CSyntaxExpression* rvalue);
    ~CSyntaxAssign();

    virtual void emit(std::ostream& out, const std::string indentation = "");

private:
    int op;
    CSyntaxExpression* lval;
};

class CSyntaxDefinition : public CSyntaxStatement
{
public:
    CSyntaxDefinition(CSyntaxExpression* lvalue, CSyntaxExpression* rvalue);
    ~CSyntaxDefinition();

    virtual void emit(std::ostream& out, const std::string indentation = "");
    virtual bool semicolonizable() { return true; }

private:
    CSyntaxExpression* lval;
};

class CSyntaxTernaryOperator : public CSyntaxExpression
{
public:
    CSyntaxTernaryOperator(int op, CSyntaxExpression* cond, CSyntaxExpression* b1, CSyntaxExpression* b2);
    ~CSyntaxTernaryOperator();

    virtual void emit(std::ostream& out, const std::string indentation = "");

private:
    int op;
    CSyntaxExpression* b1;
    CSyntaxExpression* b2;
};

class CSyntaxBinaryOperator : public CSyntaxExpression
{
public:
    CSyntaxBinaryOperator(int op, CSyntaxExpression* left, CSyntaxExpression* right);
    ~CSyntaxBinaryOperator();

    bool canBeLval() { return op == '.' || op == '['; }

    virtual void emit(std::ostream& out, const std::string indentation = "");
    virtual std::string lvaluePath();

private:
    int op;
    CSyntaxExpression* right;
    std::string randomArrayName = "";

    void generateRandomID();
};

class CSyntaxCondition : public CSyntaxBinaryOperator
{
public:
    CSyntaxCondition(int op, CSyntaxExpression* left, CSyntaxExpression* right);

    virtual void emit(std::ostream& out, const std::string indentation = "");
};

class CSyntaxRelation : public CSyntaxBinaryOperator
{
public:
    CSyntaxRelation(int rel, CSyntaxExpression* left, CSyntaxExpression* right);

    virtual void emit(std::ostream& out, const std::string indentation = "");
};

class CSyntaxUnaryOperator : public CSyntaxExpression
{
public:
    CSyntaxUnaryOperator(int op, CSyntaxExpression* expr);

    virtual void emit(std::ostream& out, const std::string indentation = "");

private:
    int op;
};

class CScriptSyntaxTree
{
public:
    CScriptSyntaxTree(CScriptLex* lexer);
    CScriptSyntaxTree(const std::string& buffer);
    ~CScriptSyntaxTree();

    void parse();
    void compile(std::ostream& out);

protected:
    CScriptLex* lexer;
    bool lexerOwned;
    CSyntaxNode* root;

    // taken from TinyJS.h
    // parsing - in order of precedence
    std::vector<CSyntaxExpression*> functionCall();
    CSyntaxExpression* factor();
    CSyntaxExpression* unary();
    CSyntaxExpression* term();
    CSyntaxExpression* expression();
    CSyntaxExpression* shift();
    CSyntaxExpression* condition();
    CSyntaxExpression* logic();
    CSyntaxExpression* ternary();
    CSyntaxExpression* base();
    // can return null for blocks like "{ }" or possibly "{ ;* }"
    CSyntaxStatement* block();
    CSyntaxNode* statement();
    // parsing utility functions
    CSyntaxFunction* parseFunctionDefinition();
    std::vector<CSyntaxID*> parseFunctionArguments();
};