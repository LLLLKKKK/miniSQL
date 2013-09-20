#include <cctype>
#include <cassert>

#include "SQLScanner.hpp"
#include "TokenAccepter.hpp"
#include "InputHandler.hpp"

namespace miniSQL {

SETUP_LOGGER(SQLScanner);

SQLScanner::SQLScanner(InputHandler* inputHandler) {
    input = inputHandler;
    tokenAccepter = new TokenAccepter;
    nowChar = input->next();
    
    for (int i = FIRST_TOKEN; i <= LAST_TOKEN; i++) {
        tokenAccepter->add(tokenStr[i], (Token)i);
    }
    
    tokenBuffer.reserve(30);
}

SQLScanner::~SQLScanner() {
    delete tokenAccepter;
}

int SQLScanner::getLineNum() const {
    return input->GetLineNum();
}
int SQLScanner::getCharNum() const {
    return input->GetCharNum();
}

void SQLScanner::nextNonWhitespaceChar() {
    while(isSpaceNow())
        nowChar = input->next();
}

void SQLScanner::nextChar() {
    nowChar = input->next();
}

void SQLScanner::scan() {
    nextNonWhitespaceChar();

    tokenBuffer.clear();

    if (isAlphaNow()) {
        scanIdentifier();
    }
    else if (isQuoteNow()) {
        scanChar();
    }
    else if (isNegativeOrDigitsNow()) {
        scanNumber();
    }
    else if (isOperatorNow()) {
        scanOperator();
    }
    else if (nowChar == '(') {
        token = LEFT_BRACE;
        nowChar = input->next();
    }
    else if (nowChar == ')') {
        token = RIGHT_BRACE;
        nowChar = input->next();
    }
    else if (nowChar == '*') {
        token = STAR;
        nowChar = input->next();
    }
    else if (nowChar == ',') {
        token = SLICE;
        nowChar = input->next();
    }
    else if (isEndOfInputNow()) {
        token = NULLTOKEN;
    }
    else if (isTerminator()) {
        token = TERMINATOR;
        nowChar = input->next();
    }
    else {
        MINISQL_LOG_ERROR("unknown token %c at line %d char %d", 
                    nowChar, input->GetLineNum(), input->GetCharNum());
        token = ERROR;
        nowChar = input->next();
    }
}

const char* SQLScanner::getTokenBuffer() const {
    return tokenBuffer.c_str();
}

Token SQLScanner::nextToken() {
    scan();
    return token;
}

void SQLScanner::scanNumber() {
    tokenBuffer.push_back(nowChar);
    appendSubsequentDigits();
    if (nowChar == '.') {
        tokenBuffer.push_back(nowChar);
        appendSubsequentDigits();
        token = FLOAT;
    }
    else {
        token = INTEGER;
    }
}

void SQLScanner::scanChar() {
    nextChar();
    while(!isQuoteNow()) {
        tokenBuffer.push_back(nowChar);
        nextChar();
    }
    nextChar();
    token = CHAR;
}

void SQLScanner::scanIdentifier() {
    tokenAccepter->init();

    if (tokenAccepter->enter(nowChar)) {
        tokenBuffer.push_back(nowChar);
        nextChar();
    
        while(isAlphaNow() && tokenAccepter->trans(nowChar)) {
            tokenBuffer.push_back(nowChar);
            nextChar();
        }

        if (isAlphaNow()) {
            tokenBuffer.push_back(nowChar);
            appendSubsequentAlpha();
            token = IDENTIFIER;
            //LOG_TRACE(logger, "%s failed trans tokenAccepter", tokenBuffer.c_str());
        }
        else {
            token = tokenAccepter->accept();
            //LOG_TRACE(logger, "%s accept tokenAccepter %s", tokenStr[token], tokenBuffer.c_str());
            if (token == NULLTOKEN)
                token = IDENTIFIER;
        }
    }
    else {
        tokenBuffer.push_back(nowChar);
        appendSubsequentAlpha();
        token = IDENTIFIER;
        //LOG_TRACE(logger, "%s didn't enter tokenAccepter", tokenBuffer.c_str()); 
    }
}

void SQLScanner::scanOperator() {
    if (nowChar == '>') {
        nextChar();
        if (nowChar == '=') {
            nextChar();
            token = GREATER_EQUAL;
        }
        else {
            token = GREATER;
        }
    }
    else if (nowChar == '<') {
        nextChar();
        if (nowChar == '=') {
            nextChar();
            token = LESS_EQUAL;
        }
        else if (nowChar == '>') {
            nextChar();
            token = NOT_EQUAL;
        }
        else {
            token = LESS;
        }
    }
    else if (nowChar == '=') {
        nextChar();
        token = EQUAL;
    }
    else {
        assert(false);  
    }
}

void SQLScanner::appendSubsequentAlpha() {
    nextChar();
    while (isalpha(nowChar)) {
        tokenBuffer.push_back(nowChar);
        nextChar();
    }
}

void SQLScanner::appendSubsequentDigits() {
    nextChar();
    while (isdigit(nowChar)) {
        tokenBuffer.push_back(nowChar);
        nextChar();
    }
}

bool SQLScanner::isSpaceNow() const {
    return isspace(nowChar);
}

bool SQLScanner::isTerminator() const {
    return nowChar == ';';
}

bool SQLScanner::isAlphaNow() const {
    return isalpha(nowChar);
}

bool SQLScanner::isQuoteNow() const {
    return nowChar == '\'';
}

bool SQLScanner::isOperatorNow() const {
    return nowChar == '=' || nowChar == '<' || nowChar == '>';
}

bool SQLScanner::isNegativeOrDigitsNow() const {
    return isdigit(nowChar) || nowChar == '-';
}

bool SQLScanner::isEndOfInputNow() const {
    return nowChar == -1;
}


}
