#include "InputHandler.hpp"

namespace miniSQL {

SETUP_LOGGER(InputHandler);

int InputHandler::lineReserveSize = 200;

InputHandler::InputHandler(std::istream* stream) {
    lineBuffer.reserve(lineReserveSize); 

    this->stream = stream;   
    if (!stream->good()) {
        MINISQL_LOG_ERROR("Bad stream %s open failed!");
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

}
