#include "InputHandler.hpp"

int InputHandler::lineReserveSize = 200;

InputHandler::InputHandler(std::istream* stream) {
    lineBuffer.reserve(lineReserveSize); 

    this->stream = stream;   
    if (!stream->good()) {
        LOG_ERROR(logger, "Bad stream %s open failed!");
        charNum = -1;
        lineNum = -1;
    }
    else {
        std::getline(*stream, lineBuffer);
  
        charNum = 0;
        lineNum = 0;
    }
}

char InputHandler::next() {
    if (charNum < (int) lineBuffer.size()) {
        return lineBuffer[charNum++];
    }
    else {
        if (stream->eof()) {
            return -1;
        }
        std::getline(*stream, lineBuffer);
        lineNum++;
        charNum = 0;
        return ' ';
    }

    return lineBuffer[charNum++];
}
