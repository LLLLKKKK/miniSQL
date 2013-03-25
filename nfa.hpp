
#ifndef NFA_HPP
#define NFA_HPP

#include<vector>
#include<map>
#include"token.hpp"

using std::vector;
using std::map;

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
