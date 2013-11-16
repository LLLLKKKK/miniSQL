
#include <iostream>
#include <sstream>
#include <string>
#include "SQLWorker.h"

int main() {
    miniSQL::SQLWorker worker;
    if (!worker.init()) {
        std::cerr << "Init SQLWorker failed\n";
        return 1;
    }

    while (true) {
        std::cout<< "LK miniSQL> ";
        std::string line;
        std::getline(std::cin, line);
        std::stringstream stream(line);
        if (!worker.start(&stream)) {
            break;
        }
    } 
    
    return 0;
}
