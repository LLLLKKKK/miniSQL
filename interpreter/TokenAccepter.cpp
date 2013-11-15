#include "TokenAccepter.h"
#include <string>

namespace miniSQL {

int TokenAccepter::reserveSpace = 30;

TokenAccepter::TokenAccepter() {
    strings.reserve(reserveSpace);
    candidate.reserve(reserveSpace);
    tmp.reserve(reserveSpace);
}

void TokenAccepter::init() {
    posNow = 0;
    tmp.clear();
    candidate.clear();
}

void TokenAccepter::add(const char *str, const Token token) {
    strings.push_back(str);
    tokenMap[str] = token;
}

bool TokenAccepter::enter(const char c) {
    for (auto i = strings.begin(); i != strings.end(); i++) {
        if ((*i)[0] == c)
            candidate.push_back(*i);
    }
    posNow++;

    return candidate.size() != 0;
}

bool TokenAccepter::trans(const char c)
{
    if (candidate.size() == 1) {
        if (candidate[0][posNow] == c) {
            posNow++;
            return true;
        }
        return false;
    }
    else {
        for (size_t i = 0;i < candidate.size(); i++) {
            const char* strNow = candidate[i];
            if (strNow[posNow] == c) {
                tmp.push_back(strNow);
            }
        }
        posNow++;
        tmp.swap(candidate);
        tmp.clear();

        return candidate.size() != 0;
    }
}

Token TokenAccepter::accept() {
    for (auto it = candidate.begin(); it != candidate.end(); it++) {
        if ((*it)[posNow] == 0) {
            return tokenMap[*it];
        }
    }
    return NULLTOKEN;
}

}
