
#ifndef _SQLPARSER_H_
#define _SQLPARSER_H_

#include <memory>
#include <list>
#include <string>
#include "common.h"
#include "token.hpp"
#include "Logger.hpp"

namespace miniSQL {

class SQLScanner;
class Logger;
class ParseNode;

typedef std::shared_ptr<ParseNode> ParseNodePtr;

struct ParseNode {
    Token token_;
    std::list<ParseNodePtr> children;
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
    
    DISALLOW_COPY_AND_ASSIGN(SQLParser);
    
public:
    std::list<ParseNodePtr> parseSQL();

    bool isError() const {
        return error_;
    }

private:

    bool startsStatement(const Token token);
    ParseNodePtr parseStatement();

    bool startsQuit(const Token token);
    ParseNodePtr parseQuit();

    bool startsCreate(const Token token);
    ParseNodePtr parseCreate();
    bool startsDrop(const Token token);
    ParseNodePtr parseDrop();
    bool startsSelect(const Token token);
    ParseNodePtr parseSelect();
    bool startsDelete(const Token token);
    ParseNodePtr parseDelete();
    bool startsInsert(const Token token);
    ParseNodePtr parseInsert();

    ParseNodePtr parseExpression();
    ParseNodePtr parseCondition();    
    ParseNodePtr parseType();

    ParseNodePtr parseOperator();
    ParseNodePtr parseLiteral();

    bool startsIdentifier(const Token token);
    ParseNodePtr parseIdentifier();
    ParseNodePtr parseFloat();
    ParseNodePtr parseInt();
    ParseNodePtr parseChar();

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
    DECLARE_LOGGER(c);
};

}

#endif
