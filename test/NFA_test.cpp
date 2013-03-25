//#define BOOST_TEST_DYN_LINK
//#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_MAIN

#include <boost/test/included/unit_test.hpp>
#include <iostream>
#include "../nfa.hpp"
#include "../token.hpp"

using std::cout;
using namespace boost::unit_test;

//BOOST_AUTO_TEST_SUITE ( nfa )

BOOST_AUTO_TEST_CASE( refuse1 ) {
	NFA nfa;

	nfa.add("create", CREATE);
	nfa.add("table", TABLE);

	nfa.init();

	BOOST_REQUIRE( nfa.enter('c') );
	BOOST_REQUIRE( nfa.trans('r') );
	BOOST_REQUIRE( !nfa.trans('b') );
	BOOST_REQUIRE( nfa.accept() == NULLTOKEN );
}


BOOST_AUTO_TEST_CASE( accept1 ) {
	NFA nfa;

	nfa.add("create", CREATE);
	nfa.add("table", TABLE);

	nfa.init();

	BOOST_REQUIRE( nfa.enter('c') );
	BOOST_REQUIRE( nfa.trans('r') );
	BOOST_REQUIRE( nfa.trans('e') );
	BOOST_REQUIRE( nfa.trans('a') );
	BOOST_REQUIRE( nfa.trans('t') );
	BOOST_REQUIRE( nfa.trans('e') );
	BOOST_REQUIRE( nfa.accept() == CREATE );
}

//BOOST_AUTO_TEST_SUITE_END()

