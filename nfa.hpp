
#ifndef NFA_HPP
#define NFA_HPP

#include<vector>

using std::vector;

class NFA
{
	private:
		vector<const char*> strings;
		vector<const char*> candidate;
		vector<const char*> tmp;

		int posNow;

	public:
		NFA();
		~NFA();

		void init();
		void add(const char* str);
		bool enter(char c);
		bool trans(char c);
};

#endif
