
#ifndef PARSETREEPRINTER_HPP
#define PARSETREEPRINTER_HPP

#include "SQLParser.h"

using namespace miniSQL;

class ParseTreePrinter {
public:
    static void print(std::stringstream& stream, const ParseNodePtr node);
    static void print(std::stringstream& stream, const std::list<ParseNodePtr >& nodes);
    static void indentPrint(std::stringstream& stream, const ParseNodePtr node, int indent);

public:
    static int INDENT_INCREMENT;

private:
    static void printNode(std::stringstream& stream, const ParseNodePtr node);
};

#endif
