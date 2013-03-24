#include <cctype>
#include "logger.hpp"
#include "scanner.hpp"

SQLScanner::SQLScanner(const char* filename)
{
	inputHandler = new InputHandler(filename);
	logger = Logger::getLogger("SQL Scanner");
}

SQLScanner::~SQLScanner()
{
	delete inputHandler;
}

void SQLScanner::nextNonWhitespaceChar()
{
	nowChar = inputHandler->next();
	while(isSpaceNow())
		nowChar = inputHandler->next();
}

void SQLScanner::scan()
{
	nextNonWhitespaceChar();

	if (isAlphaNow())
	{
		scanIdentifier();
	}
	else if (isQuoteNow())
	{
		scanValue();
	}
	else if (isOperatorNow())
	{
		scanOperator();
	}
	else if (isEndOfInputNow())
	{
		token = NULLTOKEN;
	}
	else
	{
		logger->error("unknown token at line %d char %d", 
			inputHandler->GetLineNum(), inputHandler->GetCharNum());
		token = ERROR;
	}
}

void SQLScanner::next()
{
	scan();
}

void SQLScanner::scanValue()
{
	next();
	while(!isQuoteNow())
	{
		tokenBuffer.push_back(nowChar);
		next();
	}
	next();
}

void SQLScanner::scanIdentifier()
{
	while(isAlphaNow())
	{
		tokenBuffer.push_back(nowChar);	
		next();
	}
}

void SQLScanner::scanOperator()
{
	if (nowChar == '>')
	{
		next();
		if (nowChar == '=')
		{
			next();
			token = GREATER_EQUAL;
		}
		else
		{
			token = GREATER;
		}
	}
	else if (nowChar == '<')
	{
		next();
		if (nowChar == '=')
		{
			next();
			token = LESS_EQUAL;
		}
		else if (nowChar == '>')
		{
			next();
			token = NOT_EQUAL;
		}
		else
		{
			token = LESS;
		}
	}
	else if (nowChar == '=')
	{
		next();
		if (nowChar == '=')
		{
			next();
			token = EQUAL;
		}
		else
		{
			token = ERROR;
			logger->error("unexpected token at line %d char %d", 
				inputHandler->GetLineNum(), inputHandler->GetCharNum());
		}
	}
	else
	{

	}
}

bool SQLScanner::isSpaceNow() const
{
	return isalpha(nowChar);
}

bool SQLScanner::isAlphaNow() const
{
	return isalpha(nowChar);
}

bool SQLScanner::isQuoteNow() const
{
	return nowChar == '`';
}

bool SQLScanner::isOperatorNow() const
{
	return nowChar == '=' || nowChar == '<' || nowChar == '>';
}

bool SQLScanner::isEndOfInputNow() const
{
	return nowChar == -1;
}

InputHandler::InputHandler(const char* filename)
{
	inputFile.open(filename, std::ifstream::in);
	
	charNum = 0;
	lineNum = 0;
}

char InputHandler::next()
{
	if (charNum == lineBuffer.size())
	{
		std::getline(inputFile, lineBuffer);
		lineNum++;
		charNum = 0;
	}
	
	if (inputFile.eof())
	{
		return -1;
	}
	else
	{
		return lineBuffer[charNum++];
	}
}

int InputHandler::GetLineNum() const 
{
	return lineNum;
}

int InputHandler::GetCharNum() const
{
	return charNum;
}

