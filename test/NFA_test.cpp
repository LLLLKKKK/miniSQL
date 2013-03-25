//#define BOOST_TEST_DYN_LINK
//#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_MAIN

#include <boost/test/included/unit_test.hpp>
#include "../nfa.hpp"

using namespace boost::unit_test;

//BOOST_AUTO_TEST_SUITE ( nfa )

BOOST_AUTO_TEST_CASE( refuse1 ) {
	NFA nfa;

	nfa.add("aaa");
	nfa.add("bbb");

	nfa.init();

	BOOST_REQUIRE( nfa.enter('a') );
	BOOST_REQUIRE( nfa.trans('a') );
	BOOST_REQUIRE( !nfa.trans('b') );
}


BOOST_AUTO_TEST_CASE( refuse2 ) {
	NFA nfa;

	nfa.add("aaa");
	nfa.add("bbb");

	nfa.init();

	BOOST_REQUIRE( nfa.enter('a') );
	BOOST_REQUIRE( nfa.trans('a') );
	BOOST_REQUIRE( nfa.trans('a') );
	BOOST_REQUIRE( !nfa.trans('a') );
}

BOOST_AUTO_TEST_CASE( refuse3 ) {
	NFA nfa;

	nfa.add("aaa");
	nfa.add("bbb");

	nfa.init();

	BOOST_REQUIRE( nfa.enter('b') );
	BOOST_REQUIRE( nfa.trans('b') );
	BOOST_REQUIRE( nfa.trans('b') );
	BOOST_REQUIRE( !nfa.trans('b') );
}

//BOOST_AUTO_TEST_SUITE_END()

