#include "nfa.hpp"

NFA::NFA()
{
	strings.reserve(30);
	candidate.reserve(30);
	tmp.reserve(30);
}

NFA::~NFA()
{
}

void NFA::init()
{
	posNow = 0;
	tmp.clear();
	candidate.clear();
}

void NFA::add(const char *str, Token token)
{
	strings.push_back(str);
	tokenMap[str] = token;
}

bool NFA::enter(char c)
{
	for (auto i = strings.begin(); i != strings.end(); i++)
	{
		if ((*i)[0] == c)
			candidate.push_back(*i);
	}
	posNow++;

	return candidate.size() != 0;
}

bool NFA::trans(char c)
{
	if (candidate.size() == 1)
	{
		if (candidate[0][posNow] == c)
		{
			posNow++;
			return true;
		}
		return false;
	}
	else
	{
		for (int i = 0;i < candidate.size(); i++)
		{
			const char* strNow = candidate[i];
			if (strNow[posNow] == c)
			{
				tmp.push_back(strNow);
			}
		}
		posNow++;
		tmp.swap(candidate);
		tmp.clear();

		return candidate.size() != 0;
	}
}

Token NFA::accept()
{
	if (candidate.size() == 1)
	{
		if (candidate[0][posNow] == 0)
		{
			return tokenMap[candidate[0]];
		}
	}
	return NULLTOKEN;
}
