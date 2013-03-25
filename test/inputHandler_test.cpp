
//#define BOOST_TEST_DYN_LINK
//#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_MAIN

#include <boost/test/included/unit_test.hpp>
#include <iostream>
#include "../scanner.hpp"

using std::cout;
using namespace boost::unit_test;


BOOST_AUTO_TEST_CASE( OutputAllChars ) 
{
	InputHandler input("test.sql");
	char c = input.next();
	while (c != -1)
	{
		cout << c;
		c = input.next();
	}
}


