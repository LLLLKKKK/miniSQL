
#ifndef NFA_HPP
#define NFA_HPP

#include"token.hpp"

#include<map>
#include<vector>

using std::map;
using std::vector;

class NFA
{
  private:
    vector<const char*> strings;
    vector<const char*> candidate;
    vector<const char*> tmp;
    map<const char*, Token> tokenMap;
      
    int posNow;

  public:
    NFA();
    ~NFA();

    void init();
    void add(const char* str, Token token);
    bool enter(char c);
    bool trans(char c);
    Token accept();
};

#endif
