#include <boost/test/unit_test.hpp>
#include <iostream>
#include "TokenAccepter.hpp"
#include "Token.hpp"

using std::cout;
using namespace boost::unit_test;
using namespace miniSQL;

BOOST_AUTO_TEST_SUITE ( TokenAccepter_test_suite )

BOOST_AUTO_TEST_CASE( refuse1 ) {
    TokenAccepter tokenAccepter;

    tokenAccepter.add("create", CREATE);
    tokenAccepter.add("table", TABLE);

    tokenAccepter.init();

    BOOST_REQUIRE( tokenAccepter.enter('c') );
    BOOST_REQUIRE( tokenAccepter.trans('r') );
    BOOST_REQUIRE( !tokenAccepter.trans('b') );
    BOOST_REQUIRE( tokenAccepter.accept() == NULLTOKEN );
}


BOOST_AUTO_TEST_CASE( accept1 ) {
    TokenAccepter tokenAccepter;

    tokenAccepter.add("create", CREATE);
    tokenAccepter.add("table", TABLE);

    tokenAccepter.init();

    BOOST_REQUIRE( tokenAccepter.enter('c') );
    BOOST_REQUIRE( tokenAccepter.trans('r') );
    BOOST_REQUIRE( tokenAccepter.trans('e') );
    BOOST_REQUIRE( tokenAccepter.trans('a') );
    BOOST_REQUIRE( tokenAccepter.trans('t') );
    BOOST_REQUIRE( tokenAccepter.trans('e') );
    BOOST_REQUIRE( tokenAccepter.accept() == CREATE );
}

BOOST_AUTO_TEST_SUITE_END()

