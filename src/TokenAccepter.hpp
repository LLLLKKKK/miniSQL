
#ifndef TOKENACCEPTER_HPP
#define TOKENACCEPTER_HPP

#include <map>
#include <vector>

#include "token.hpp"

using std::map;
using std::vector;

class TokenAccepter
{
public:
    TokenAccepter();

    void init();
    void add(const char* str, const Token token);
    bool enter(const char c);
    bool trans(const char c);
    Token accept();

private:
    vector<const char*> strings;
    vector<const char*> candidate;
    vector<const char*> tmp;
    map<const char*, Token> tokenMap;

    int posNow;
    static int reserveSpace;
};

#endif
