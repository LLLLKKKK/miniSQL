#include <boost/test/unit_test.hpp>
#include <iostream>
#include "../src/nfa.hpp"
#include "../src/token.hpp"

using std::cout;
using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE ( nfa_test_suite )

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

BOOST_AUTO_TEST_SUITE_END()

