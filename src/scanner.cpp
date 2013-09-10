#include <cctype>
#include <cassert>
#include "logger.hpp"
#include "scanner.hpp"
#include "nfa.hpp"

SQLScanner::SQLScanner(const char* filename) {
    input = new InputHandler(filename);
    nfa = new NFA;
    nowChar = input->next();
    
    logger = Logger::getLogger("SQL Scanner");
    
    for (int i = FIRST_TOKEN; i <= LAST_TOKEN; i++) {
        nfa->add(tokenStr[i], (Token)i);
    }
    
    tokenBuffer.reserve(30);
}

SQLScanner::~SQLScanner() {
    delete nfa;
    delete input;
}

void SQLScanner::nextNonWhitespaceChar() {
    while(isSpaceNow())
        nowChar = input->next();
}

void SQLScanner::nextChar() {
    nowChar = input->next();
}

void SQLScanner::scan()
{
  nextNonWhitespaceChar();

  tokenBuffer.clear();

  if (isAlphaNow())
  {
    scanIdentifier();
  }
  else if (isQuoteNow())
  {
    scanChar();
  }
  else if (isNegativeOrDigitsNow())
  {
    scanNumber();
  }
  else if (isOperatorNow())
  {
    scanOperator();
  }
  else if (nowChar == '(')
  {
    token = LEFT_BRACE;
    nowChar = input->next();
  }
  else if (nowChar == ')')
  {
    token = RIGHT_BRACE;
    nowChar = input->next();
  }
  else if (nowChar == '*')
  {
    token = STAR;
    nowChar = input->next();
  }
  else if (nowChar == ',')
  {
    token = SLICE;
    nowChar = input->next();
  }
  else if (isEndOfInputNow())
  {
    token = NULLTOKEN;
  }
  else if (isTerminator())
  {
    token = TERMINATOR;
    nowChar = input->next();
  }
  else
  {
      LOG_ERROR(logger, "unknown token %c at line %d char %d", 
                nowChar, input->GetLineNum(), input->GetCharNum());
      token = ERROR;
      nowChar = input->next();
  }

}

const char* SQLScanner::getTokenBuffer() const
{
  return tokenBuffer.c_str();
}

Token SQLScanner::nextToken()
{
  scan();
  return token;
}

void SQLScanner::scanNumber()
{
  tokenBuffer.push_back(nowChar);
  appendSubsequentDigits();
  if (nowChar == '.')
  {
    tokenBuffer.push_back(nowChar);
    appendSubsequentDigits();
    token = FLOAT;
  }
  else
  {
    token = INTEGER;
  }
}

void SQLScanner::scanChar()
{
  nextChar();
  while(!isQuoteNow())
  {
    tokenBuffer.push_back(nowChar);
    nextChar();
  }
  nextChar();
  token = CHAR;
}

void SQLScanner::scanIdentifier()
{
  nfa->init();

  if (nfa->enter(nowChar))
  {
    tokenBuffer.push_back(nowChar);
    nextChar();
    
    while(isAlphaNow() && nfa->trans(nowChar))
    {
      tokenBuffer.push_back(nowChar);
      nextChar();
    }

    if (isAlphaNow())
    {
      tokenBuffer.push_back(nowChar);
      appendSubsequentAlpha();
      token = IDENTIFIER;

    }
    else
    {
      token = nfa->accept();
      if (token == NULLTOKEN)
        token = IDENTIFIER;
    }
  }
  else
  {
    tokenBuffer.push_back(nowChar);
    appendSubsequentAlpha();
    token = IDENTIFIER;
  }
}

void SQLScanner::scanOperator()
{
  if (nowChar == '>')
  {
    nextChar();
    if (nowChar == '=')
    {
      nextChar();
      token = GREATER_EQUAL;
    }
    else
    {
      token = GREATER;
    }
  }
  else if (nowChar == '<')
  {
    nextChar();
    if (nowChar == '=')
    {
      nextChar();
      token = LESS_EQUAL;
    }
    else if (nowChar == '>')
    {
      nextChar();
      token = NOT_EQUAL;
    }
    else
    {
      token = LESS;
    }
  }
  else if (nowChar == '=')
  {
    nextChar();
    if (nowChar == '=')
    {
      nextChar();
      token = EQUAL;
    }
    else
    {
        token = ERROR;
        LOG_ERROR(logger, "unexpected token at line %d char %d", 
                  input->GetLineNum(), input->GetCharNum());
    }
  }
  else
  {
      assert(false);  
  }
}

void SQLScanner::appendSubsequentAlpha()
{
  nextChar();
  while (isalpha(nowChar))
  { 
    tokenBuffer.push_back(nowChar);
    nextChar();
  }
}

void SQLScanner::appendSubsequentDigits()
{
  nextChar();
  while (isdigit(nowChar))
  {
    tokenBuffer.push_back(nowChar);
    nextChar();
  }
}
bool SQLScanner::isSpaceNow() const
{
  return isspace(nowChar);
}

bool SQLScanner::isTerminator() const
{
  return nowChar == ';';
}

bool SQLScanner::isAlphaNow() const
{
  return isalpha(nowChar);
}

bool SQLScanner::isQuoteNow() const
{
  return nowChar == '\'';
}

bool SQLScanner::isOperatorNow() const
{
  return nowChar == '=' || nowChar == '<' || nowChar == '>';
}

bool SQLScanner::isNegativeOrDigitsNow() const
{
  return isdigit(nowChar) || nowChar == '-';
}

bool SQLScanner::isEndOfInputNow() const
{
  return nowChar == -1;
}


InputHandler::InputHandler(const char* filename)
{
  lineBuffer.reserve(1000);

  inputFile.open(filename, std::ifstream::in);
  if (inputFile.good()) {
    
  }

  std::getline(inputFile, lineBuffer);
  
  charNum = 0;
  lineNum = 0;
}

char InputHandler::next()
{
    if (charNum == (int)lineBuffer.size())
  {
    std::getline(inputFile, lineBuffer);
    lineNum++;
    charNum = 0;
  }

  if (inputFile.eof())
  {
    return -1;
  }

  return lineBuffer[charNum++];
}

int InputHandler::GetLineNum() const 
{
  return lineNum;
}

int InputHandler::GetCharNum() const
{
  return charNum;
}

