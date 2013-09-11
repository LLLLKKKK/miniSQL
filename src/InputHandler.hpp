
#ifndef INPUTHANDLER_HPP
#define INPUTHANDLER_HPP

#include <string>
#include <istream>
#include "Logger.hpp"

class InputHandler {
public:
    InputHandler(std::istream* stream);
    char next();

    int GetLineNum() const {
        return lineNum;
    }
    int GetCharNum() const {
        return charNum;
    }  

private:
    std::string lineBuffer;
    int charNum;
    int lineNum;
    LOGGER_PTR logger;
    std::istream* stream;

    static int lineReserveSize;
};

#endif
