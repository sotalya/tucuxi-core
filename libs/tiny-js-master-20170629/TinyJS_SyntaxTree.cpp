#include "TinyJS_SyntaxTree.h"
#include <string.h>
#include <algorithm>
#include <assert.h>
#include <sstream>

#define ASSERT(X) assert(X)
// if this flag is enabled, the constructors of CSyntax constructs
// will ensure that the simplifying assumptions they make in their 
// emit() methods are not violated
#define CHECK_SYNTAX_TREE

// these three macros are used to avoid memory leaking in JIT-ed code. 
#define FUNCTION_VECTOR_NAME "__t_"
#define NO_LEAK_BEGIN() (std::string("(*") + FUNCTION_VECTOR_NAME + ".insert(" + FUNCTION_VECTOR_NAME + ".end(), ")
#define NO_LEAK_END() ("))")

CScriptSyntaxTree::CScriptSyntaxTree(CScriptLex* lexer)
{
    this->lexer = lexer;
    lexerOwned = false;
    root = 0;

}

CScriptSyntaxTree::CScriptSyntaxTree(const std::string& buffer)
{
    this->lexer = new CScriptLex(buffer);
    lexerOwned = true;
    root = 0;
}

CScriptSyntaxTree::~CScriptSyntaxTree()
{
    if(root)
        delete root;
    if(lexerOwned)
        delete lexer;
}

void CScriptSyntaxTree::parse()
{
    CSyntaxSequence* stmts = 0;
    while(lexer->tk)
        stmts = new CSyntaxSequence(stmts, statement());
    lexer->match(LEX_EOF);
    if(stmts && !stmts->first())
        root = stmts->second();
    else
        root = stmts;
}

void CScriptSyntaxTree::compile(std::ostream & out)
{
    if(!(CSyntaxFunction*)root)
        TRACE("Warning: the root of this syntax tree is not a function definition. Compiled code may not function correctly.\n");
    root->emit(out);
}

std::vector<CSyntaxExpression*> CScriptSyntaxTree::functionCall()
{
    std::vector<CSyntaxExpression*> args;
    lexer->match('(');
    while(lexer->tk != ')')
    {
        args.push_back(base());
        if(lexer->tk != ')') lexer->match(',');
    }
    lexer->match(')');
    return args;
}

CSyntaxExpression* CScriptSyntaxTree::factor()
{
    if(lexer->tk == '(')
    {
        lexer->match('(');
        CSyntaxExpression* a = base();
        lexer->match(')');
        return a;
    }
    if(lexer->tk == LEX_R_TRUE)
    {
        lexer->match(LEX_R_TRUE);
        return new CSyntaxFactor("1");
    }
    if(lexer->tk == LEX_R_FALSE)
    {
        lexer->match(LEX_R_FALSE);
        return new CSyntaxFactor("0");
    }
    if(lexer->tk == LEX_R_NULL)
    {
        lexer->match(LEX_R_NULL);
        return new CSyntaxFactor("null");
    }
    if(lexer->tk == LEX_R_UNDEFINED)
    {
        lexer->match(LEX_R_UNDEFINED);
        return new CSyntaxFactor("undefined");
    }
    if(lexer->tk == LEX_ID)
    {
        int nameStart = lexer->tokenStart;
        std::string tokenName = lexer->tkStr;
        lexer->match(LEX_ID);
        CSyntaxExpression* a = 0;
        while(lexer->tk == '(' || lexer->tk == '.' || lexer->tk == '[')
        {
            if(lexer->tk == '(')
            {
                std::string argString = lexer->getSubString(nameStart);
                int argStart = lexer->tokenStart;
                auto args = functionCall();
                argString += lexer->getSubString(argStart);
                a = new CSyntaxFunctionCall(a ? a : new CSyntaxID(tokenName), args, argString);
            }
            else if(lexer->tk == '.')
            {
                lexer->match('.');
                const std::string &name = lexer->tkStr;
                a = new CSyntaxBinaryOperator('.', a ? a : new CSyntaxID(tokenName), new CSyntaxID(name));
                lexer->match(LEX_ID);
            }
            else if(lexer->tk == '[')
            {
                lexer->match('[');
                CSyntaxExpression* index = base();
                lexer->match(']');
                a = new CSyntaxBinaryOperator('[', a ? a : new CSyntaxID(tokenName), index);
            }
            else ASSERT(0);
        }
        // likely the lhs of an assignment (or rhs, really)
        if(!a)
            a = new CSyntaxID(tokenName);
        return a;
    }
    if(lexer->tk == LEX_INT || lexer->tk == LEX_FLOAT)
    {
        CSyntaxFactor* a = new CSyntaxFactor(lexer->tkStr);
        lexer->match(lexer->tk);
        return a;
    }
    if(lexer->tk == LEX_STR)
    {
        CSyntaxFactor* a = new CSyntaxFactor(lexer->tkStr);
        lexer->match(LEX_STR);
        return a;
    }
    if(lexer->tk == '{')
    {
        /* JSON-style object definition */
        std::string arg = "{";
        lexer->match('{');
        // compile string
        while(lexer->tk != '}')
        {
            arg += lexer->tkStr.size() ? lexer->tkStr : std::string(1, (char)lexer->tk);
            lexer->match(lexer->tk);
        }
        arg += "}";
        lexer->match('}');
        // "__object_()" is a special native function that constructs an object from a string
        return new CSyntaxFunctionCall(new CSyntaxID(TINYJS_OBJECT_FUNCTION_NAME), std::vector<CSyntaxExpression*>(1,
            new CSyntaxFactor(arg.c_str())), TINYJS_OBJECT_FUNCTION_NAME + ("(\"" + arg + "\")"));
    }
    if(lexer->tk == '[')
    {
        /* JSON-style array definition */
        lexer->match('[');
        std::string arg = "[";
        while(lexer->tk != ']')
        {
            arg += lexer->tkStr;
            lexer->match(lexer->tk);
        }
        lexer->match(']');
        arg += ']';
        return new CSyntaxFunctionCall(new CSyntaxID(TINYJS_ARRAY_FUNCTION_NAME), std::vector<CSyntaxExpression*>(1,
            new CSyntaxFactor(arg.c_str())), TINYJS_ARRAY_FUNCTION_NAME + ("(\"" + arg + "\")"));
    }
    if(lexer->tk == LEX_R_FUNCTION)
    {
        return parseFunctionDefinition();
    }
    if(lexer->tk == LEX_R_NEW)
    {
        // new -> create a new object
        // this means that if the id referenced is a function,
        // call the function with a new object "this" in scope,
        // otherwise create a new object and set its .prototype attribute
        // to the thing specified.
        lexer->match(LEX_R_NEW);
        const std::string &className = lexer->tkStr;
        lexer->match(LEX_ID);
        std::string arg = className + "(";
        while(lexer->tk != ')')
        {
            arg += lexer->tkStr;
            lexer->match(lexer->tk);
        }
        lexer->match(')');
        arg += ')';
        return new CSyntaxFunctionCall(new CSyntaxID(TINYJS_NEW_FUNCTION_NAME), std::vector<CSyntaxExpression*>(1,
            new CSyntaxFactor(arg.c_str())), TINYJS_NEW_FUNCTION_NAME + arg);
    }
    // Nothing we can do here... just hope it's the end...
    lexer->match(LEX_EOF);
    return NULL;
}

CSyntaxExpression* CScriptSyntaxTree::unary()
{
    if(lexer->tk == '!')
    {
        lexer->match('!'); // binary not
        return new CSyntaxUnaryOperator('!', factor());
    }
    return factor();
}

CSyntaxExpression* CScriptSyntaxTree::term()
{
    CSyntaxExpression* a = unary();
    while(lexer->tk == '*' || lexer->tk == '/' || lexer->tk == '%')
    {
        int op = lexer->tk;
        lexer->match(lexer->tk);
        return new CSyntaxBinaryOperator(op, a, unary());
    }
    return a;
}

CSyntaxExpression* CScriptSyntaxTree::expression()
{
    bool negate = false;
    if(lexer->tk == '-')
    {
        lexer->match('-');
        negate = true;
    }
    CSyntaxExpression* a = term();
    if(negate)
    {
        a = new CSyntaxBinaryOperator('-', new CSyntaxFactor("0"), a);
    }

    while(lexer->tk == '+' || lexer->tk == '-' ||
        lexer->tk == LEX_PLUSPLUS || lexer->tk == LEX_MINUSMINUS)
    {
        int op = lexer->tk;
        lexer->match(lexer->tk);
        if(op == LEX_PLUSPLUS || op == LEX_MINUSMINUS)
        {
            // more desugaring
            a = new CSyntaxAssign('=', a,
                new CSyntaxBinaryOperator(op == LEX_PLUSPLUS ? '+' : '-', a, new CSyntaxFactor("1")));
        }
        else
        {
            a = new CSyntaxBinaryOperator(op, a, term());
        }
    }
    return a;
}

CSyntaxExpression* CScriptSyntaxTree::shift()
{
    CSyntaxExpression* a = expression();
    if(lexer->tk == LEX_LSHIFT || lexer->tk == LEX_RSHIFT || lexer->tk == LEX_RSHIFTUNSIGNED)
    {
        int op = lexer->tk;
        lexer->match(op);
        return new CSyntaxBinaryOperator(op, a, base());
    }
    return a;
}

CSyntaxExpression* CScriptSyntaxTree::condition()
{
    CSyntaxExpression* a = shift();
    while(lexer->tk == LEX_EQUAL || lexer->tk == LEX_NEQUAL ||
        lexer->tk == LEX_TYPEEQUAL || lexer->tk == LEX_NTYPEEQUAL ||
        lexer->tk == LEX_LEQUAL || lexer->tk == LEX_GEQUAL ||
        lexer->tk == '<' || lexer->tk == '>')
    {
        int op = lexer->tk;
        lexer->match(lexer->tk);
        a = new CSyntaxRelation(op, a, shift());
    }
    return a;
}

CSyntaxExpression* CScriptSyntaxTree::logic()
{
    CSyntaxExpression* a = condition();
    CSyntaxExpression* b;
    while(lexer->tk == '&' || lexer->tk == '|' || lexer->tk == '^' || lexer->tk == LEX_ANDAND || lexer->tk == LEX_OROR)
    {
        int op = lexer->tk;
        lexer->match(lexer->tk);
        b = condition();
        if(op == LEX_ANDAND || op == LEX_OROR)
            a = new CSyntaxCondition(op, a, b);
        else
            a = new CSyntaxBinaryOperator(op, a, b);
    }
    return a;
}

CSyntaxExpression* CScriptSyntaxTree::ternary()
{
    CSyntaxExpression* lhs = logic();
    if(lexer->tk == '?')
    {
        lexer->match('?');
        CSyntaxExpression* b1 = base();
        lexer->match(':');
        lhs = new CSyntaxTernaryOperator('?', lhs, b1, base());
    }

    return lhs;
}

CSyntaxExpression* CScriptSyntaxTree::base()
{
    CSyntaxExpression* lhs = ternary();
    if(lexer->tk == '=' || lexer->tk == LEX_PLUSEQUAL || lexer->tk == LEX_MINUSEQUAL)
    {
        int op = lexer->tk;
        lexer->match(lexer->tk);
        CSyntaxExpression* rhs = base();
        if(op == '=')
        {
            lhs = new CSyntaxAssign('=', lhs, rhs);
        }
        // hey look, syntactic desugaring
        else if(op == LEX_PLUSEQUAL)
        {
            lhs = new CSyntaxAssign('=', lhs, new CSyntaxBinaryOperator('+', lhs, rhs));
        }
        else if(op == LEX_MINUSEQUAL)
        {
            lhs = new CSyntaxAssign('=', lhs, new CSyntaxBinaryOperator('-', lhs, rhs));
        }
        else ASSERT(0);
    }
    return lhs;
}

CSyntaxStatement* CScriptSyntaxTree::block()
{
    lexer->match('{');
    CSyntaxSequence* stmts = 0;
    while(lexer->tk && lexer->tk != '}')
    {
        CSyntaxNode* stmt = statement();
        CSyntaxSequence* seq = dynamic_cast<CSyntaxSequence*>(stmt);
        if(seq)
        {
            std::vector<CSyntaxNode*> statements = seq->normalize();
            delete seq;
            for(auto& stmt2 : statements)
                stmts = new CSyntaxSequence(stmts, stmt2);
        }
        else
            stmts = new CSyntaxSequence(stmts, stmt);
    }
    lexer->match('}');
    return stmts;
}

CSyntaxNode* CScriptSyntaxTree::statement()
{
    if(lexer->tk == LEX_ID ||
        lexer->tk == LEX_INT ||
        lexer->tk == LEX_FLOAT ||
        lexer->tk == LEX_STR ||
        lexer->tk == '-')
    {
        /* Execute a simple statement that only contains basic arithmetic... */
        CSyntaxNode* out = base();
        lexer->match(';');
        return out;
    }
    else if(lexer->tk == '{')
    {
        /* A block of code */
        return block();
    }
    else if(lexer->tk == ';')
    {
        /* Empty statement - to allow things like ;;; */
        lexer->match(';');
        return statement();
    }
    else if(lexer->tk == LEX_R_VAR)
    {
        lexer->match(LEX_R_VAR);
        CSyntaxSequence* stmts = 0;
        CSyntaxNode* stmt = 0;
        while(lexer->tk != ';')
        {
            CSyntaxExpression* lhs = new CSyntaxID(lexer->tkStr);
            lexer->match(LEX_ID);
            // now do stuff defined with dots
            while(lexer->tk == '.')
            {
                lexer->match('.');
                lhs = new CSyntaxBinaryOperator('.', lhs, new CSyntaxID(lexer->tkStr));
                lexer->match(LEX_ID);
            }
            // sort out initialiser
            if(lexer->tk == '=')
            {
                lexer->match('=');
                stmt = new CSyntaxDefinition(lhs, base());
            }
            else
                stmt = new CSyntaxDefinition(lhs, NULL);
            if(lexer->tk != ';')
            {
                lexer->match(',');
                stmts = new CSyntaxSequence(stmts, stmt ? stmt : lhs);
                stmt = 0;
            }
            else if(stmts)
                stmts = new CSyntaxSequence(stmts, stmt ? stmt : lhs);
        }
        lexer->match(';');
        return stmts ? stmts : (CSyntaxNode*)stmt;
    }
    else if(lexer->tk == LEX_R_IF)
    {
        lexer->match(LEX_R_IF);
        lexer->match('(');
        CSyntaxExpression* cond = base();
        lexer->match(')');
        CSyntaxNode* body = statement();
        CSyntaxNode* else_ = 0;
        if(lexer->tk == LEX_R_ELSE)
        {
            lexer->match(LEX_R_ELSE);
            else_ = statement();
        }
        return new CSyntaxIf(cond, body, else_);
    }
    else if(lexer->tk == LEX_R_WHILE)
    {
        lexer->match(LEX_R_WHILE);
        lexer->match('(');
        CSyntaxExpression* cond = base();
        lexer->match(')');
        CSyntaxNode* body = statement();
        return new CSyntaxWhile(cond, body);
    }
    else if(lexer->tk == LEX_R_FOR)
    {
        lexer->match(LEX_R_FOR);
        lexer->match('(');
        CSyntaxNode* init = statement(); // initialisation
        CSyntaxExpression* cond = base(); // condition
        lexer->match(';');
        CSyntaxExpression* iter = base(); // iterator
        lexer->match(')');
        CSyntaxNode* body = statement();
        return new CSyntaxFor(init, cond, iter, body);
    }
    else if(lexer->tk == LEX_R_RETURN)
    {
        lexer->match(LEX_R_RETURN);
        CSyntaxExpression* value = base();
        lexer->match(';');
        return new CSyntaxReturn(value);
    }
    else if(lexer->tk == LEX_R_FUNCTION)
    {
        return parseFunctionDefinition();
    }
    else
    {
        lexer->match(LEX_EOF);
        return NULL;
    }
}

CSyntaxFunction* CScriptSyntaxTree::parseFunctionDefinition()
{
    lexer->match(LEX_R_FUNCTION);
    std::string funcName = TINYJS_TEMP_NAME;
    /* we can have functions without names */
    if(lexer->tk == LEX_ID)
    {
        funcName = lexer->tkStr;
        lexer->match(LEX_ID);
    }
    std::vector<CSyntaxID*> args = parseFunctionArguments();
    CSyntaxStatement* body = block();
    return new CSyntaxFunction(funcName == TINYJS_TEMP_NAME ? 0 : new CSyntaxID(funcName), args, body);
}

std::vector<CSyntaxID*> CScriptSyntaxTree::parseFunctionArguments()
{
    std::vector<CSyntaxID*> out;
    lexer->match('(');
    while(lexer->tk != ')')
    {
        out.push_back(new CSyntaxID(lexer->tkStr));
        lexer->match(LEX_ID);
        if(lexer->tk != ')') lexer->match(',');
    }
    lexer->match(')');
    return out;
}

CSyntaxNode::CSyntaxNode()
{
    node = 0;
}

CSyntaxNode::~CSyntaxNode()
{
    if(node)
        delete node;
}

CSyntaxSequence::CSyntaxSequence(CSyntaxNode* front, CSyntaxNode* last)
{
    node = front;
    this->last = last;
    disowned = false;
#ifdef CHECK_SYNTAX_TREE
    ASSERT(this->last && !dynamic_cast<CSyntaxSequence*>(this->last));
    ASSERT(this->node || this->last);
#endif
}

CSyntaxSequence::~CSyntaxSequence()
{
    // make sure to still delete sequences
    if(disowned)
    {
        if(node && !dynamic_cast<CSyntaxSequence*>(node))
            node = 0;
        if(last && dynamic_cast<CSyntaxSequence*>(last))
            delete last;
    }
    else if(last)
        delete last;
}

std::vector<CSyntaxNode*> CSyntaxSequence::normalize(bool disown_children)
{
    disowned = disown_children;
    std::vector<CSyntaxNode*> stmts;
    CSyntaxSequence* child;
    if(node)
    {
        if(child = dynamic_cast<CSyntaxSequence*>(node))
        {
            std::vector<CSyntaxNode*> children = child->normalize(disown_children);
            stmts.insert(stmts.end(), children.begin(), children.end());
        }
        else
            stmts.push_back(node);
    }
    if(last)
    {
        if(child = dynamic_cast<CSyntaxSequence*>(last))
        {
            std::vector<CSyntaxNode*> children = child->normalize(disown_children);
            stmts.insert(stmts.end(), children.begin(), children.end());
        }
        else
            stmts.push_back(last);
    }
    return stmts;
}

// this function assumes that the tree of sequencing is entirely left-heavy;
// ie the structure is:
//    left := sequence|statement|null
//    right := statement
// enable the compile-time constant CHECK_SYNTAX_TREE to confirm this
void CSyntaxSequence::emit(std::ostream & out, const std::string indentation)
{
    std::vector<CSyntaxSequence*> parents;
    CSyntaxNode* n = node;
    CSyntaxSequence* seq;
    while(seq = dynamic_cast<CSyntaxSequence*>(n))
    {
        parents.push_back(seq);
        n = seq->node;
    }
    while(!parents.empty())
    {
        seq = *parents.rbegin();
        parents.pop_back();
        if(seq->node && !dynamic_cast<CSyntaxSequence*>(seq->node))
        {
            seq->node->emit(out, indentation);
            if(seq->node->semicolonizable())
                out << ";";
            out << "\n";
        }
        if(seq->last)
        {
            seq->last->emit(out, indentation);
            if(seq->last->semicolonizable())
                out << ";";
            out << "\n";
        }
    }
    if(node && !dynamic_cast<CSyntaxSequence*>(node))
    {
        node->emit(out, indentation);
        if(node->semicolonizable())
            out << ";";
        out << "\n";
    }
    if(last)
    {
        last->emit(out, indentation);
        if(last->semicolonizable())
            out << ";";
        out << "\n";
    }
}

CSyntaxIf::CSyntaxIf(CSyntaxExpression* expr, CSyntaxNode* body, CSyntaxNode* else_)
{
    ASSERT(expr);
    ASSERT(body);
    node = body;
    this->else_ = else_;
    this->expr = expr;
}

CSyntaxIf::CSyntaxIf(CSyntaxExpression* expr, CSyntaxNode* body) : CSyntaxIf(expr, body, 0) { }

CSyntaxIf::~CSyntaxIf()
{
    delete expr;
    if(else_)
        delete else_;
}

void CSyntaxIf::emit(std::ostream & out, const std::string indentation)
{
    out << indentation << "if(";
    expr->emit(out);
    out << "->var->getBool()) {\n";
    node->emit(out, indentation + "    ");
    out << indentation << "} ";
    if(else_)
    {
        out << "else {\n";
        else_->emit(out, indentation + "    ");
        out << indentation << "}";
    }
}

CSyntaxWhile::CSyntaxWhile(CSyntaxExpression* expr, CSyntaxNode* body)
{
    ASSERT(body);
    ASSERT(expr);
    node = body;
    this->expr = expr;
}

CSyntaxWhile::~CSyntaxWhile()
{
    delete expr;
}

void CSyntaxWhile::emit(std::ostream & out, const std::string indentation)
{
    out << indentation << "while(";
    expr->emit(out);
    out << "->var->getBool()) {\n";
    node->emit(out, indentation + "    ");
    out << indentation << "}";
}

CSyntaxFor::CSyntaxFor(CSyntaxNode* init, CSyntaxExpression* expr, CSyntaxExpression* update, CSyntaxNode* body)
{
    ASSERT(body);
    node = body;
    this->init = init;
    this->update = update;
    cond = expr;
}

CSyntaxFor::~CSyntaxFor()
{
    if(init)
        delete init;
    if(update)
        delete update;
    if(cond)
        delete cond;
}

void CSyntaxFor::emit(std::ostream & out, const std::string indentation)
{
    out << indentation << "for(";
    if(init)
    {
        init->emit(out);
    }
    out << "; ";
    if(cond)
    {
        cond->emit(out);
        out << "->var->getBool()";
    }
    out << "; ";
    if(update)
    {
        update->emit(out);
    }
    out << ") {\n";
    node->emit(out, indentation + "    ");
    out << indentation << "}";
}

CSyntaxFactor::CSyntaxFactor(std::string val)
{
    value = val;
    node = 0;

    char f = value.front();
    if(f == '"')
        factorType = F_TYPE_STRING;
    else if(isdigit(f))
    {
        if(value.find('.') != std::string::npos)
            factorType = F_TYPE_DOUBLE;
        else
            factorType = F_TYPE_INT;
    }
    else
        factorType = F_TYPE_IDENTIFIER;
}

void CSyntaxFactor::emit(std::ostream & out, const std::string indentation)
{
    out << indentation;
    out << NO_LEAK_BEGIN();
    out << "new CScriptVarLink(new CScriptVar(";
    switch(factorType)
    {
    case F_TYPE_INT:
        out << getInt();
        break;
    case F_TYPE_DOUBLE:
        out << getDouble();
        break;
    case F_TYPE_STRING:
        out << "\"" << value.c_str() << "\"";
        break;
    case F_TYPE_IDENTIFIER:
        out << value.c_str();
        break;
    }
    out << "))";
    out << NO_LEAK_END();
}

CSyntaxID::CSyntaxID(std::string id) : CSyntaxFactor(id) { }

void CSyntaxID::emit(std::ostream & out, const std::string indentation)
{
    // this is not correct
    out << indentation << value.c_str();
}

CSyntaxFunction::CSyntaxFunction(CSyntaxID* name, std::vector<CSyntaxID*>& arguments, CSyntaxStatement* body)
{
    ASSERT(body);
    this->name = name;
    this->arguments = arguments;
    node = body;
}

CSyntaxFunction::~CSyntaxFunction()
{
    if(name)
        delete name;
    for(CSyntaxID* arg : arguments)
        if(arg)
            delete arg;
}

void CSyntaxFunction::emit(std::ostream & out, const std::string indentation)
{
    out << indentation << "extern \"C\" {\n";
    std::string realIndent = indentation + "    ";
    out << realIndent << "void ";
    getName()->emit(out);
    out << "(CScriptVar* root, void* userData) {\n";
    // to avoid memory leaks, we need a structure to hold any newly allocated CScriptVarLink*s.
    // we need to mangle the name of any locals so as to avoid name collisions
    // (however, if any variables in the function were named "__t_", this would
    // generate bad code due to a redeclaration). 
    // thus, we should endeavor to declare as little extra as possible.
    out << realIndent + "    " << "std::vector<CScriptVarLink*> " << FUNCTION_VECTOR_NAME << ";\n";
    for(auto& arg : arguments)
    {
        // setup variable declarations/assignments
        out << realIndent + "    " << "CScriptVarLink* ";
        arg->emit(out);
        out << " = new CScriptVarLink(root->getParameter(\"";
        arg->emit(out);
        out << "\"));\n";
    }
    node->emit(out, realIndent + "    ");
    // cleanup
    for(auto& arg : arguments)
    {
        out << realIndent + "    " << "delete ";
        arg->emit(out);
        out << ";\n";
    }
    out << realIndent + "    " << "for(CScriptVarLink* __to_del_: " << FUNCTION_VECTOR_NAME << ")\n";
    out << realIndent + "        " << "delete __to_del_;\n";
    out << realIndent << "}\n";
    out << indentation << "}\n";
}

CSyntaxID* CSyntaxFunction::getName()
{
    if(!name)
        generateRandomId();
    return name;
}

void CSyntaxFunction::generateRandomId()
{
    // generate a random (likely unused) identifier so that this function is not unnamed
    static const char alphanum[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string randomId = "";
    for(int i = 0; i < 20; ++i)
    {
        randomId += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    name = new CSyntaxID(randomId);
}

CSyntaxAssign::CSyntaxAssign(int op, CSyntaxExpression* lvalue, CSyntaxExpression* rvalue)
{
    ASSERT(lvalue);
    ASSERT(rvalue);
    this->op = op;
    lval = lvalue;
    node = rvalue;
}

CSyntaxAssign::~CSyntaxAssign()
{
    delete lval;
}

void CSyntaxAssign::emit(std::ostream & out, const std::string indentation)
{
    out << indentation;
    lval->emit(out);
    out << "->replaceWith(";
    node->emit(out);
    out << "->var)";
}

CSyntaxTernaryOperator::CSyntaxTernaryOperator(int op, CSyntaxExpression* cond, CSyntaxExpression* b1, CSyntaxExpression* b2)
{
    ASSERT(cond);
    ASSERT(b1);
    ASSERT(b2);
    this->op = op;
    node = cond;
    this->b1 = b1;
    this->b2 = b2;
#ifdef CHECK_SYNTAX_TREE
    ASSERT(op == '?');
#endif
}

CSyntaxTernaryOperator::~CSyntaxTernaryOperator()
{
    delete b1;
    delete b2;
}

void CSyntaxTernaryOperator::emit(std::ostream & out, const std::string indentation)
{
    // op can only be '?'
    out << indentation;
    node->emit(out);
    out << "->var->getBool() ? ";
    b1->emit(out);
    out << " : ";
    b2->emit(out);
}

CSyntaxRelation::CSyntaxRelation(int rel, CSyntaxExpression* left, CSyntaxExpression* right)
    : CSyntaxBinaryOperator(rel, left, right)
{ }

void CSyntaxRelation::emit(std::ostream & out, const std::string indentation)
{
    CSyntaxBinaryOperator::emit(out);
}

CSyntaxBinaryOperator::CSyntaxBinaryOperator(int op, CSyntaxExpression* left, CSyntaxExpression* right)
{
    ASSERT(left);
    ASSERT(right);
    this->op = op;
    node = left;
    this->right = right;
#ifdef CHECK_SYNTAX_TREE
    // right side has to be an ID
    ASSERT(op != '.' || dynamic_cast<CSyntaxID*>(right));
#endif
}

CSyntaxBinaryOperator::~CSyntaxBinaryOperator()
{
    delete right;
}

void CSyntaxBinaryOperator::emit(std::ostream & out, const std::string indentation)
{
    out << indentation;
    if(op != '[' && op != '.')
    {
        out << NO_LEAK_BEGIN();
        out << "new CScriptVarLink(";
        node->emit(out);
        out << "->var->mathsOp(";
        right->emit(out);
        out << "->var, " << op << "))";
        out << NO_LEAK_END();
    }
    else
    {
        // beautifully enough, this will return a CScriptVarLink*, which can be used as
        // /either/ an lvalue OR an rvalue depending on the calling code's fancy!
        // The more I work on this code, the more I begin to think that it was actually
        // fairly well designed.	  
        node->emit(out);
        out << "->var->findChildOrCreate(";
        if(op == '.')
        {
            out << '"';
            right->emit(out);
            out << '"';
        }
        else
        {
            right->emit(out);
            out << "->var->getString()";
        }
        out << ")";
    }
}

std::string CSyntaxBinaryOperator::lvaluePath()
{
    if(op == '.')
        return ((CSyntaxExpression*)node)->lvaluePath() + "." + right->lvaluePath();
    else if(op == '[')
    {
        generateRandomID();
        return ((CSyntaxExpression*)node)->lvaluePath() + "_array__" + randomArrayName;
    }
    ASSERT(0);
    return std::string();
}

void CSyntaxBinaryOperator::generateRandomID()
{
    if(randomArrayName.length() != 0)
        return;

    // generate a random (likely unused) identifier so that this function is not unnamed
    static const char alphanum[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string randomId = "";
    for(int i = 0; i < 5; ++i)
    {
        randomId += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    randomArrayName = randomId;
}

CSyntaxUnaryOperator::CSyntaxUnaryOperator(int op, CSyntaxExpression* expr)
{
    this->op = op;
    node = expr;
}

void CSyntaxUnaryOperator::emit(std::ostream & out, const std::string indentation)
{
    // normally, you would have to check to see if the operators we're talking about
    // are prefix or postfix operators; however, in TinyJS.cpp::111-112 it is mentioned
    // that postfix operators work as prefix operators, and prefix operators don't exist.
    // so, placing the unary operator always at the beginning happens to preserve behavior!
    // also, I don't currenly use prefix/postfix operators due to desugaring
    out << indentation;
    out << CScriptLex::getTokenStr(op, true);
    node->emit(out);
}

CSyntaxReturn::CSyntaxReturn(CSyntaxExpression* value)
{
    node = value;
}

void CSyntaxReturn::emit(std::ostream & out, const std::string indentation)
{
    out << indentation;
    out << "root->setReturnVar(";
    // todo: ensure this returns a CScriptVar or CScriptVarLink
    node->emit(out);
    out << "->var)";
}

CSyntaxCondition::CSyntaxCondition(int op, CSyntaxExpression* left, CSyntaxExpression* right)
    : CSyntaxBinaryOperator(op, left, right)
{ }

void CSyntaxCondition::emit(std::ostream & out, const std::string indentation)
{
    CSyntaxBinaryOperator::emit(out);
}

CSyntaxFunctionCall::CSyntaxFunctionCall(CSyntaxExpression* name,
    std::vector<CSyntaxExpression*> arguments, std::string originalString)
{
    node = name;
    actuals = arguments;
    origString = originalString;
}

CSyntaxFunctionCall::~CSyntaxFunctionCall()
{
    for(CSyntaxExpression* arg : actuals)
        delete arg;
}

void CSyntaxFunctionCall::emit(std::ostream & out, const std::string indentation)
{
    // we have to call back into the interpreter to make function calls.
    // the exception is recursive calls; we can set those up ourselves.
    // however, setting up a recursive call is a non-trivial amount of work
    // (clear the symbol table for function root, set up new parameters in the
    // symbol table, ensure "this" gets set up if necessary and not accidentally
    // unref'd), and since function calls to ourselves can appear in a non-tail
    // position (ie as part of an expression), it's a little bit of extra work
    // to set up a recursive call.
    // so, for now, just make an interpreter call.

    // evaluateComplex returns a CScriptVarLink (but not a pointer), so assign it to
    // our local placeholder and immediately take the reference of it so that this
    // expression returns a CScriptVarLink*.

    // CSyntaxFunction::emit() doesn't set up a local CTinyJS variable because it
    // is only used here and we want to avoid accidental redeclarations of variables.
	out << indentation << NO_LEAK_BEGIN();
	out << "new CScriptVarLink(((CTinyJS*)userData)->evaluateComplex(\"";
    // we need to escape quotes
    std::string str = origString;
    size_t pos = 0;
    while((pos = str.find_first_of('"', pos)) != std::string::npos)
    {
        str.insert(str.begin() + pos, '\\');
        pos += 2; // make sure to skip past both the \ and the " to avoid infinite loops
    }
    out << str << "\").var)";
	out << NO_LEAK_END();
}

CSyntaxDefinition::CSyntaxDefinition(CSyntaxExpression * lvalue, CSyntaxExpression * rvalue)
{
    ASSERT(lvalue);
    lval = lvalue;
    node = rvalue;
}

CSyntaxDefinition::~CSyntaxDefinition()
{
    delete lval;
}

void CSyntaxDefinition::emit(std::ostream & out, const std::string indentation)
{
    // this doesn't work with array indexing, although it IS
    // illegal to use an array index with "var"
    std::string lvalPath = lval->lvaluePath();
    out << indentation << "CScriptVarLink* ";
    out << lvalPath << " = " << "root->findChildOrCreateByPath(\"";
    std::replace(lvalPath.begin(), lvalPath.end(), '.', '_');
    out << lvalPath << "\")";
    if(node)
    {
        out << "->replaceWith(";
        node->emit(out);
        out << ")";
    }
}
