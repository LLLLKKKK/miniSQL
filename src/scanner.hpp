
#ifndef SCANNER_HPP
#define SCANNER_HPP

#include "token.hpp"
#include <string>
#include <fstream>

using std::string;
using std::ifstream;

class Logger;
class NFA;

class InputHandler 
{

private:
	string lineBuffer;
	int charNum;
	int lineNum;
	ifstream inputFile;
	
public:

	InputHandler(const char* filename);
	int GetLineNum() const;
	int GetCharNum() const;
	
	char next();
};


class SQLScanner {

private:
	string tokenBuffer;
	
	InputHandler *input;
	NFA* nfa;
	Logger* logger;
	
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
	
public:
	SQLScanner(const char* filename);
	~SQLScanner();
	
	const char* getTokenBuffer() const;
	Token nextToken();
	void scan();

};



#endif
