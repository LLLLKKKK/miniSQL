
#ifndef PARSETREEPRINTER_HPP
#define PARSETREEPRINTER_HPP

#include "../src/SQLParser.hpp"

using namespace miniSQL;

class ParseTreePrinter {
public:
    static void print(std::stringstream& stream, const PARSENODE_PTR node);
    static void print(std::stringstream& stream, const std::list<PARSENODE_PTR >& nodes);
    static void indentPrint(std::stringstream& stream, const PARSENODE_PTR node, int indent);

public:
    static int INDENT_INCREMENT;

private:
    static void printNode(std::stringstream& stream, const PARSENODE_PTR node);
};

#endif
