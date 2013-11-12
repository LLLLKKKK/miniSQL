
#include <cstdlib>
#include <iostream>
#include <cassert>
#include "SQLParser.hpp"
#include "SQLScanner.hpp"
#include "Token.hpp"

namespace miniSQL {


SQLParser::SQLParser(SQLScanner* scanner) {
    this->error_ = false;
    this->scanner = scanner;
    assert(scanner != nullptr);
}

void SQLParser::readToken() {
    prevReading = nowReading;
    nowReading = scanner->nextToken();
}

void SQLParser::expect(const Token token) {
    if (nowReading != token) {
        syntaxError(token, "expected token");
    }
    readToken();
}

void SQLParser::syntaxError(const Token token, const char* description) {
    MINISQL_LOG_ERROR("At line %d:%d, %s %s", scanner->getLineNum(), scanner->getCharNum(), 
                tokenStr[token], description); 

    error_ = true;
}

std::list<ParseNodePtr > SQLParser::parseSQL() {
    std::list<ParseNodePtr > statements;
    readToken();
    while (startsStatement(nowReading)) {
        statements.push_back(parseStatement());
    }
    expect(NULLTOKEN);
    return statements;
}

bool SQLParser::startsStatement(const Token token) {
    return startsCreate(token) || startsSelect(token) 
        || startsDelete(token) || startsInsert(token)
        || startsDrop(token) || token == QUIT || token == EXECFILE;
}

ParseNodePtr SQLParser::parseStatement() {
    if (!startsStatement(nowReading)) {
        syntaxError(nowReading, "expect start token!");
        return nullptr;
    }
    if (startsCreate(nowReading)) {
        return parseCreate();
    }
    else if (startsSelect(nowReading)) {
        return parseSelect();
    }
    else if (startsDelete(nowReading)) {
        return parseDelete();
    }
    else if (startsInsert(nowReading)) {
        return parseInsert();
    }
    else if (startsDrop(nowReading)) {
        return parseDrop();
    }
    else if (nowReading == QUIT) {
        readToken();
        expect(TERMINATOR);
        return ParseNodePtr(new ParseNode(QUIT));
    }
    else if (nowReading == EXECFILE) {
        readToken();
        expect(TERMINATOR);
        return ParseNodePtr(new ParseNode(EXECFILE));
    }
    else {
        assert(false);
        return nullptr;
    }
}

bool SQLParser::startsQuit(const Token token) {
    return token == QUIT;
}

ParseNodePtr SQLParser::parseQuit() {
    ParseNodePtr node(new ParseNode(QUIT));
    readToken();
    return node;    
}

bool SQLParser::startsCreate(const Token token) {
    return token == CREATE;
}

ParseNodePtr SQLParser::parseCreate() {
    if (!startsCreate(nowReading)) {
        syntaxError(nowReading, "expect create!");
        return nullptr;
    }
    //LOG_TRACE(logger, "parse create statement.");

    ParseNodePtr createNode = ParseNodePtr(new ParseNode(CREATE));
    readToken();

    if (nowReading == TABLE) {
        createNode->children.emplace_back(new ParseNode(TABLE));
        readToken();
        
        createNode->children.push_back(parseIdentifier());
        
        expect(LEFT_BRACE);
        while (startsIdentifier(nowReading)) {
            ParseNodePtr id = parseIdentifier();
            //LOG_TRACE(logger, "parse id %s.", ((IdentifierNode*)id.get())->id_.c_str());
            ParseNodePtr type = parseType();
            if (nowReading == UNIQUE) {
                readToken();
                id->children.emplace_back(new ParseNode(UNIQUE));
            }
            expect(SLICE);
            id->children.push_back(type);
            createNode->children.push_back(id);
        }

        expect(PRIMARY);
        expect(KEY);
        expect(LEFT_BRACE);
        createNode->children.push_back(parseIdentifier());
        expect(RIGHT_BRACE);

        expect(RIGHT_BRACE);
    }
    else if (nowReading == INDEX) {
        createNode->children.emplace_back(new ParseNode(INDEX));
        readToken();

        ParseNodePtr indexNode = parseIdentifier();
        createNode->children.push_back(indexNode);
        expect(ON);
        ParseNodePtr tableNode = parseIdentifier();
        createNode->children.push_back(tableNode);
        expect(LEFT_BRACE);
        ParseNodePtr columnNode = parseIdentifier();
        createNode->children.push_back(columnNode);
        expect(RIGHT_BRACE);
    }
    else {
        syntaxError(nowReading, "expect table or index!");
        return nullptr;
    }

    expect(TERMINATOR);
    return createNode;
}

bool SQLParser::startsDrop(const Token token) {
    return token == DROP;
}

ParseNodePtr SQLParser::parseDrop() {
    if (!startsDrop(nowReading)) {
        syntaxError(nowReading, "expect drop token!");
    }
    ParseNodePtr dropNode = ParseNodePtr(new ParseNode(DROP));
    readToken();
    if (nowReading == TABLE) {
        readToken();
        dropNode->children.emplace_back(new ParseNode(TABLE));
        dropNode->children.push_back(parseIdentifier());
    }
    else if (nowReading == INDEX) {
        readToken();
        dropNode->children.emplace_back(new ParseNode(INDEX));
        dropNode->children.push_back(parseIdentifier());
    }
    else {
        syntaxError(nowReading, "expect table or index!");
        return nullptr;
    }
    expect(TERMINATOR);
    return dropNode;
}

bool SQLParser::startsSelect(const Token token) {
    return token == SELECT;
}

ParseNodePtr SQLParser::parseSelect() {
    if (!startsSelect(nowReading)) {
        syntaxError(nowReading, "expect select token!");
        return nullptr;
    }
    ParseNodePtr selectNode = ParseNodePtr(new ParseNode(SELECT));
    readToken();
    expect(STAR);
    expect(FROM);
    selectNode->children.push_back(parseIdentifier());
    if (nowReading == WHERE) {
        readToken();
        selectNode->children.push_back(parseCondition());
    }
    expect(TERMINATOR);
    return selectNode;
}

ParseNodePtr SQLParser::parseCondition() {
    ParseNodePtr condNode = ParseNodePtr(new ParseNode(AND));
    condNode->children.push_back(parseExpression());

    while (nowReading == AND) {
        readToken();
        condNode->children.push_back(parseExpression());
    }
    return condNode;
}

ParseNodePtr SQLParser::parseExpression() {
    ParseNodePtr columnNode = parseIdentifier();
    ParseNodePtr opNode = parseOperator();
    ParseNodePtr valueNode = parseLiteral();
    opNode->children.push_back(columnNode);
    opNode->children.push_back(valueNode);
    return opNode;
}

bool SQLParser::startsDelete(Token token) {
    return token == DELETE;
}

ParseNodePtr SQLParser::parseDelete() {
    if (!startsDelete(nowReading)) {
        syntaxError(nowReading, "expect delete!");
        return nullptr;
    }
    ParseNodePtr deleteNode = ParseNodePtr(new ParseNode(DELETE));
    readToken();
    expect(FROM);
    deleteNode->children.push_back(parseIdentifier());
    if (nowReading == WHERE) {
        readToken();
        deleteNode->children.push_back(parseCondition());
    }
    expect(TERMINATOR);
    return deleteNode;
}

bool SQLParser::startsInsert(Token token) {
    return token == INSERT;
}

ParseNodePtr SQLParser::parseInsert() {
    if (!startsInsert(nowReading)) {
        syntaxError(nowReading, "expect insert token!");        
        return nullptr;
    }
    //LOG_TRACE(logger, "parse insert statement.");

    ParseNodePtr insertNode = ParseNodePtr(new ParseNode(INSERT));
    readToken();
    expect(INTO);
    insertNode->children.push_back(parseIdentifier());
    expect(VALUES);

    expect(LEFT_BRACE);
    insertNode->children.push_back(parseLiteral());
    while (nowReading == SLICE) {
        readToken();
        insertNode->children.push_back(parseLiteral());
    }
    expect(RIGHT_BRACE);

    expect(TERMINATOR);
    return insertNode;
}

ParseNodePtr SQLParser::parseType() {
    //LOG_TRACE(logger, "parse type %s.", tokenStr[nowReading]);
    
    if (nowReading == INTEGER) {
        readToken();
        return ParseNodePtr(new IntNode(-1));
    }
    else if (nowReading == FLOAT) {
        readToken();
        return ParseNodePtr(new FloatNode(-1));
    }
    else if (nowReading == CHAR) {
        readToken();
        ParseNodePtr node =  ParseNodePtr(new CharNode);
        expect(LEFT_BRACE);
        ParseNodePtr num = parseInt();
        expect(RIGHT_BRACE);
        node->children.push_back(num);
        return node;
    }
    else {
        syntaxError(nowReading, "expect type!");
        return nullptr;
    }
}

bool SQLParser::startsIdentifier(const Token token) {
    return token == IDENTIFIER;
}

ParseNodePtr SQLParser::parseIdentifier() {
    if (nowReading != IDENTIFIER) {
        syntaxError(nowReading, "expect identifier!");
        return nullptr;
    }
    
    ParseNodePtr node (new IdentifierNode(std::string(scanner->getTokenBuffer())));
    readToken();
    return node;
}

ParseNodePtr SQLParser::parseOperator() {
    ParseNodePtr node = nullptr;
    if (nowReading == GREATER || nowReading == LESS
        || nowReading == GREATER_EQUAL || nowReading == LESS_EQUAL
        || nowReading == EQUAL || nowReading == NOT_EQUAL) {
        node =  ParseNodePtr(new ParseNode(nowReading));
        readToken();
    }
    return node;
}

ParseNodePtr SQLParser::parseLiteral() {
    if (nowReading == INTEGER) {
        return parseInt();
    }
    else if (nowReading == CHAR) {
        return parseChar();
    }
    else if (nowReading == FLOAT) {
        return parseFloat();
    }
    else {
        syntaxError(nowReading, "expect literal!");
        return nullptr;
    }
}

ParseNodePtr SQLParser::parseFloat() {
    ParseNodePtr node(new FloatNode(::atof(scanner->getTokenBuffer())));
    readToken();
    return node;
}

ParseNodePtr SQLParser::parseInt() {
    ParseNodePtr node(new IntNode(::atoi(scanner->getTokenBuffer())));
    readToken();
    return node;
}

ParseNodePtr SQLParser::parseChar() {
    ParseNodePtr node(new CharNode(std::string(scanner->getTokenBuffer())));
    readToken();
    return node;
}

}
