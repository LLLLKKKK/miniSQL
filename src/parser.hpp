
#ifndef _SQLPARSER_H_
#define _SQLPARSER_H_

#include "token.hpp"

class SQLScanner;

struct ParseNode {
    Token token;
    ParseNode(Token token) {
        this->token = token;
    }
};

struct CharNode : public ParseNode{
    char c;
};

struct IntNode : public ParseNode {
    int i;
};

struct FloatNode : public ParseNode {
    float f;
};


class SQLParser {
public:
    SQLParser(SQLScanner* scanner);
    
public:
    static parse(SQLScanner* scanner);
    ParseNode* parse();

private:
    std::list<ParseNode*> parseSQL();

    bool startsStatement();
    ParseNode* parseStatement();

    ParseNode* parseLiteral();

private:
    void readToken();
    void expect();

private:
    Token nowReading;
    Token prevReading;

    bool error;
};

#endif
