
#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <string>

#include "token.hpp"
#include "Logger.hpp"

using std::string;

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
    LOGGER_PTR logger;
  
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
};

#endif
