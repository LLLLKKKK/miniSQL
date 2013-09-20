
#include <cstdlib>
#include <iostream>
#include <cassert>
#include "SQLParser.hpp"
#include "SQLScanner.hpp"
#include "token.hpp"

namespace miniSQL {

SETUP_LOGGER(SQLParser);

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

std::list<PARSENODE_PTR > SQLParser::parseSQL() {
    std::list<PARSENODE_PTR > statements;
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

PARSENODE_PTR SQLParser::parseStatement() {
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
        return PARSENODE_PTR(new ParseNode(QUIT));
    }
    else if (nowReading == EXECFILE) {
        readToken();
        expect(TERMINATOR);
        return PARSENODE_PTR(new ParseNode(EXECFILE));
    }
    else {
        assert(false);
        return nullptr;
    }
}

bool SQLParser::startsQuit(const Token token) {
    return token == QUIT;
}

PARSENODE_PTR SQLParser::parseQuit() {
    PARSENODE_PTR node(new ParseNode(QUIT));
    readToken();
    return node;    
}

bool SQLParser::startsCreate(const Token token) {
    return token == CREATE;
}

PARSENODE_PTR SQLParser::parseCreate() {
    if (!startsCreate(nowReading)) {
        syntaxError(nowReading, "expect create!");
        return nullptr;
    }
    //LOG_TRACE(logger, "parse create statement.");

    PARSENODE_PTR createNode = PARSENODE_PTR(new ParseNode(CREATE));
    readToken();

    if (nowReading == TABLE) {
        createNode->children.emplace_back(new ParseNode(TABLE));
        readToken();
        
        createNode->children.push_back(parseIdentifier());
        
        expect(LEFT_BRACE);
        while (startsIdentifier(nowReading)) {
            PARSENODE_PTR id = parseIdentifier();
            //LOG_TRACE(logger, "parse id %s.", ((IdentifierNode*)id.get())->id_.c_str());
            PARSENODE_PTR type = parseType();
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

        PARSENODE_PTR indexNode = parseIdentifier();
        createNode->children.push_back(indexNode);
        expect(ON);
        PARSENODE_PTR tableNode = parseIdentifier();
        createNode->children.push_back(tableNode);
        expect(LEFT_BRACE);
        PARSENODE_PTR columnNode = parseIdentifier();
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

PARSENODE_PTR SQLParser::parseDrop() {
    if (!startsDrop(nowReading)) {
        syntaxError(nowReading, "expect drop token!");
    }
    PARSENODE_PTR dropNode = PARSENODE_PTR(new ParseNode(DROP));
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

PARSENODE_PTR SQLParser::parseSelect() {
    if (!startsSelect(nowReading)) {
        syntaxError(nowReading, "expect select token!");
        return nullptr;
    }
    PARSENODE_PTR selectNode = PARSENODE_PTR(new ParseNode(SELECT));
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

PARSENODE_PTR SQLParser::parseCondition() {
    PARSENODE_PTR condNode = PARSENODE_PTR(new ParseNode(AND));
    condNode->children.push_back(parseExpression());

    while (nowReading == AND) {
        readToken();
        condNode->children.push_back(parseExpression());
    }
    return condNode;
}

PARSENODE_PTR SQLParser::parseExpression() {
    PARSENODE_PTR columnNode = parseIdentifier();
    PARSENODE_PTR opNode = parseOperator();
    PARSENODE_PTR valueNode = parseLiteral();
    opNode->children.push_back(columnNode);
    opNode->children.push_back(valueNode);
    return opNode;
}

bool SQLParser::startsDelete(Token token) {
    return token == DELETE;
}

PARSENODE_PTR SQLParser::parseDelete() {
    if (!startsDelete(nowReading)) {
        syntaxError(nowReading, "expect delete!");
        return nullptr;
    }
    PARSENODE_PTR deleteNode = PARSENODE_PTR(new ParseNode(DELETE));
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

PARSENODE_PTR SQLParser::parseInsert() {
    if (!startsInsert(nowReading)) {
        syntaxError(nowReading, "expect insert token!");        
        return nullptr;
    }
    //LOG_TRACE(logger, "parse insert statement.");

    PARSENODE_PTR insertNode = PARSENODE_PTR(new ParseNode(INSERT));
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

PARSENODE_PTR SQLParser::parseType() {
    //LOG_TRACE(logger, "parse type %s.", tokenStr[nowReading]);
    
    if (nowReading == INTEGER) {
        readToken();
        return PARSENODE_PTR(new IntNode(-1));
    }
    else if (nowReading == FLOAT) {
        readToken();
        return PARSENODE_PTR(new FloatNode(-1));
    }
    else if (nowReading == CHAR) {
        readToken();
        PARSENODE_PTR node =  PARSENODE_PTR(new CharNode);
        expect(LEFT_BRACE);
        PARSENODE_PTR num = parseInt();
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

PARSENODE_PTR SQLParser::parseIdentifier() {
    if (nowReading != IDENTIFIER) {
        syntaxError(nowReading, "expect identifier!");
        return nullptr;
    }
    
    PARSENODE_PTR node (new IdentifierNode(std::string(scanner->getTokenBuffer())));
    readToken();
    return node;
}

PARSENODE_PTR SQLParser::parseOperator() {
    PARSENODE_PTR node = nullptr;
    if (nowReading == GREATER || nowReading == LESS
        || nowReading == GREATER_EQUAL || nowReading == LESS_EQUAL
        || nowReading == EQUAL || nowReading == NOT_EQUAL) {
        node =  PARSENODE_PTR(new ParseNode(nowReading));
        readToken();
    }
    return node;
}

PARSENODE_PTR SQLParser::parseLiteral() {
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

PARSENODE_PTR SQLParser::parseFloat() {
    PARSENODE_PTR node(new FloatNode(::atof(scanner->getTokenBuffer())));
    readToken();
    return node;
}

PARSENODE_PTR SQLParser::parseInt() {
    PARSENODE_PTR node(new IntNode(::atoi(scanner->getTokenBuffer())));
    readToken();
    return node;
}

PARSENODE_PTR SQLParser::parseChar() {
    PARSENODE_PTR node(new CharNode(std::string(scanner->getTokenBuffer())));
    readToken();
    return node;
}

}
