
#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <string>

#include "Token.h"
#include "logger/Logger.h"

using std::string;

namespace miniSQL {

class TokenAccepter;
class InputHandler;

class SQLScanner {
public:
    SQLScanner(InputHandler* inputHandler);
    ~SQLScanner();
  
    const char* getTokenBuffer() const;
    Token nextToken();
    void scan();
    int getLineNum() const;
    int getCharNum() const;

private:
    string tokenBuffer;
    InputHandler *input;
    TokenAccepter* tokenAccepter;
  
    Token token;
    char nowChar;  

    void nextNonWhitespaceChar();
    void nextChar();
  
    bool isSpaceNow() const;
    bool isAlphaNow() const;
    bool isOperatorNow() const;
    bool isQuoteNow() const;
    bool isEndOfInputNow() const;
    bool isTerminator() const;
    bool isNegativeOrDigitsNow() const;

    void appendSubsequentDigits();
    void appendSubsequentAlpha();

    void scanChar();
    void scanNumber();
    void scanIdentifier();
    void scanOperator();

private:
    DECLARE_LOGGER(c);
};

}

#endif
