
//#define BOOST_TEST_DYN_LINK
//#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_MAIN

#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include "../scanner.hpp"

using std::cout;
using namespace boost::unit_test;

//BOOST_AUTO_TEST_SUITE ( nfa )

BOOST_AUTO_TEST_CASE( complexTest ) 
{
	SQLScanner scanner("test.sql");

	Token token = scanner.nextToken();

	while (token != NULLTOKEN)
	{
		//cout << tokenStr[token] << ' ' << scanner.getTokenBuffer() << '\n';
		token = scanner.nextToken();
	}
}


BOOST_AUTO_TEST_CASE( speedTest ) 
{
	SQLScanner scanner("100wtest.sql");
	int count = 0;
	Token token = scanner.nextToken();

	while (token != NULLTOKEN)
	{
		if (token == TERMINATOR) count++;
		token = scanner.nextToken();
	}
	BOOST_REQUIRE_EQUAL( count, 1000000 );
}



//BOOST_AUTO_TEST_SUITE_END()

