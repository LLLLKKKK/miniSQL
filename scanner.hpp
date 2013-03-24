
#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <fstream>
#include <string>
#include "keywords.hpp"
#include "logger.hpp"

using std::string;
using std::ifstream;

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
	string lineBuffer;
	string tokenBuffer;
	
	InputHandler *inputHandler;
	Logger* logger;
	
	Token token;
	char nowChar;
	
	void next();
	void nextNonWhitespaceChar();
	
	bool isSpaceNow() const;
	bool isAlphaNow() const;
	bool isOperatorNow() const;
	bool isQuoteNow() const;
	bool isEndOfInputNow() const;
	
	void scanValue();
	void scanIdentifier();
	void scanOperator();
	
public:
	SQLScanner(const char* filename);
	~SQLScanner();
	
	void scan();

};



#endif
