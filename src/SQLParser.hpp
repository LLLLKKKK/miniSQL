
#ifndef _SQLPARSER_H_
#define _SQLPARSER_H_

#include <memory>
#include <list>
#include <string>
#include "common.h"
#include "token.hpp"
#include "Logger.hpp"

class SQLScanner;

#define PARSENODE_PTR std::shared_ptr<ParseNode>

struct ParseNode {
    Token token_;
    std::list<PARSENODE_PTR > children;
    ParseNode(Token token) : token_(token) {}
};


struct CharNode : public ParseNode {
    std::string c_;
    CharNode(const std::string& c) : ParseNode(CHAR), c_(c) { }
    CharNode() : ParseNode(CHAR) { }
};

struct IntNode : public ParseNode {
    int i_;
    IntNode(int i) : ParseNode(INTEGER), i_(i) { }
};

struct FloatNode : public ParseNode {
    float f_;
    FloatNode(float f) : ParseNode(FLOAT), f_(f) { }
};

struct IdentifierNode : public ParseNode {
    std::string id_;
    IdentifierNode(const std::string& id) : ParseNode(IDENTIFIER), id_(id) { }
};

class SQLParser {
public:
    SQLParser(SQLScanner* scanner);
    
    DISALLOW_COPY_AND_ASSGIN(SQLParser);
    
public:
    std::list<PARSENODE_PTR> parseSQL();

    bool isError() const {
        return error_;
    }

private:

    bool startsStatement(const Token token);
    PARSENODE_PTR parseStatement();

    bool startsQuit(const Token token);
    PARSENODE_PTR parseQuit();

    bool startsCreate(const Token token);
    PARSENODE_PTR parseCreate();
    bool startsDrop(const Token token);
    PARSENODE_PTR parseDrop();
    bool startsSelect(const Token token);
    PARSENODE_PTR parseSelect();
    bool startsDelete(const Token token);
    PARSENODE_PTR parseDelete();
    bool startsInsert(const Token token);
    PARSENODE_PTR parseInsert();

    PARSENODE_PTR parseExpression();
    PARSENODE_PTR parseCondition();    
    PARSENODE_PTR parseType();

    PARSENODE_PTR parseOperator();
    PARSENODE_PTR parseLiteral();

    bool startsIdentifier(const Token token);
    PARSENODE_PTR parseIdentifier();
    PARSENODE_PTR parseFloat();
    PARSENODE_PTR parseInt();
    PARSENODE_PTR parseChar();

private:
    void readToken();
    void expect(const Token token);
    void syntaxError(const Token token, const char* description);

private:
    Token nowReading;
    Token prevReading;

    SQLScanner* scanner;

    bool error_;

private:
    DECALRE_LOG();
};

#endif
