
#include <iostream>
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../src/scanner.hpp"

using std::cout;
using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE ( scanner_test_suite )

BOOST_AUTO_TEST_CASE( complexTest ) 
{
  SQLScanner scanner("test/fixture/complex_test.sql");

  Token token = scanner.nextToken();

  while (token != NULLTOKEN)
  {
    // cout << tokenStr[token] << ' ' << scanner.getTokenBuffer() << '\n';
    BOOST_REQUIRE_NE( token, ERROR );
    token = scanner.nextToken();
  }
}


BOOST_AUTO_TEST_CASE( speedTest ) 
{
  boost::posix_time::ptime start_time =
      boost::posix_time::microsec_clock::local_time();

  SQLScanner scanner("test/fixture/100w_test.sql");
  int count = 0;
  Token token = scanner.nextToken();

  while (token != NULLTOKEN)
  {
    if (token == TERMINATOR) count++;
    token = scanner.nextToken();
  }

  boost::posix_time::ptime stop_time =
      boost::posix_time::microsec_clock::local_time();

  boost::posix_time::time_duration dur_ms = stop_time - start_time;
  BOOST_TEST_MESSAGE( "100w SQL time cost: " << dur_ms << " ms");
  BOOST_REQUIRE_EQUAL( count, 1000000 );
}

BOOST_AUTO_TEST_SUITE_END()

