#include "ParseTreePrinter.hpp"
#include <sstream>
#include <iostream>
using std::endl;

int ParseTreePrinter::INDENT_INCREMENT = 4;

void ParseTreePrinter::print(std::stringstream& stream, 
                             const std::list<PARSENODE_PTR >& nodes) {
    for (auto it = nodes.begin(); it != nodes.end(); it++) {
        indentPrint(stream, *it, 0);
    }
}

void ParseTreePrinter::print(std::stringstream& stream, const PARSENODE_PTR node) {
    indentPrint(stream, node, 0);
}

void ParseTreePrinter::indentPrint(std::stringstream& stream, const PARSENODE_PTR node, 
                                   int indent) {
    for (int i = 0; i < indent; i++) {
        stream << ' ';
    }
    printNode(stream, node);
    if (node) {
        for (auto it = node->children.begin(); it != node->children.end(); it++) {
            indentPrint(stream, *it, indent + INDENT_INCREMENT);
        }
    }
}

void ParseTreePrinter::printNode(std::stringstream& stream, const PARSENODE_PTR node) {
    if (!node) {
        stream << "NULL" << endl;
        std::cout << "MULL" << endl;
        return ;
    }
    
    if (node->token_ == IDENTIFIER) {
        IdentifierNode* id_node = (IdentifierNode*)node.get();
        stream << tokenStr[IDENTIFIER] << ' ' << id_node->id_ << ' '; 
    }
    else if (node->token_ == INTEGER) {
        stream << tokenStr[INTEGER] << ' ';
        IntNode* i_node = (IntNode*)node.get();
        if (i_node == nullptr)
            stream << "type ";
        else
            stream << i_node->i_ << ' ';
    }
    else if (node->token_ == CHAR) {
        stream << tokenStr[CHAR] << ' ';
        CharNode* c_node = (CharNode*)node.get();
        if (c_node == nullptr)
            stream << "type ";
        else
            stream << c_node->c_ << ' ';
    }
    else if (node->token_ == FLOAT) {
        stream << tokenStr[FLOAT] << ' ';
        FloatNode* float_node = (FloatNode*)node.get();
        if (float_node != nullptr)
            stream << "type ";
        else
            stream << float_node->f_ << ' ';
    }
    else {
        stream << tokenStr[node->token_] << ' ';
    }
    stream << std::endl;
}
