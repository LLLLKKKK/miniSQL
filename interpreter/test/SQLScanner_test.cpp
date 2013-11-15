
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "SQLScanner.h"
#include "InputHandler.h"

using std::cout;
using namespace boost::unit_test;
using namespace miniSQL;

BOOST_AUTO_TEST_SUITE ( scanner_test_suite )

BOOST_AUTO_TEST_CASE( simpleTest ) 
{
    std::stringstream stream;
    stream << "abc 123 where" << std::endl;
    stream << "on and >= <= = > <" << std::endl;
    stream << "create drop insert select delete quit" << std::endl;
    stream << "into primary key index values table on where from" << std::endl;
    stream << "( ) , *" << std:: endl;
    stream << "1.23 -2.3 1 3 " << std::endl;
    stream << "; df" << std::endl;
//    std::cout << stream.str();

    InputHandler inputHandler(&stream);
    SQLScanner scanner(&inputHandler);
    
    Token token;
    token = scanner.nextToken();
    while (token != NULLTOKEN) {
        cout << tokenStr[token] << ' ' << scanner.getTokenBuffer() << '\n';
        BOOST_REQUIRE_NE( token, ERROR );
        token = scanner.nextToken();
    }

    // token = scanner.nextToken();
    // BOOST_REQUIRE_EQUAL( token, IDENTIFIER );
    // BOOST_REQUIRE_EQUAL( scanner.getTokenBuffer(), "abc" );
    // token = scanner.nextToken();
    // BOOST_REQUIRE_EQUAL( token, INTEGER );
    // BOOST_REQUIRE_EQUAL( scanner.getTokenBuffer(), "123" );
    // token = scanner.nextToken();
    // BOOST_REQUIRE_EQUAL( token, WHERE );
    // token = scanner.nextToken();
    // BOOST_REQUIRE_EQUAL( token, ON );
    // token = scanner.nextToken();
    // BOOST_REQUIRE_EQUAL( token, AND );
    // token = scanner.nextToken();
    // BOOST_REQUIRE_EQUAL( token, GREATER_EQUAL );
    // token = scanner.nextToken();
    // BOOST_REQUIRE_EQUAL( token, LESS_EQUAL );
    // token = scanner.nextToken();
    // BOOST_REQUIRE_EQUAL( token, EQUAL );
    // token = scanner.nextToken();
    // BOOST_REQUIRE_EQUAL( token, GREATER );
    // token = scanner.nextToken();
    // BOOST_REQUIRE_EQUAL( token, LESS );
    // token = scanner.nextToken();
    // BOOST_REQUIRE_EQUAL( token, CREATE );
    // token = scanner.nextToken();
    // BOOST_REQUIRE_EQUAL( token, DROP );
    // token = scanner.nextToken();
    // BOOST_REQUIRE_EQUAL( token, INSERT );
    // token = scanner.nextToken();
    // BOOST_REQUIRE_EQUAL( token, SELECT );
    // token = scanner.nextToken();
    // BOOST_REQUIRE_EQUAL( token, DELETE );
    // token = scanner.nextToken();
    // BOOST_REQUIRE_EQUAL( token, QUIT );

}

BOOST_AUTO_TEST_CASE( simple2Test ) 
{
    std::stringstream stream;
    stream << "create table student (sno char(8), sname char(16) unique," 
        " sage int, sgender char (1), primary key ( sno ) );" << std::endl;

    InputHandler inputHandler(&stream);
    SQLScanner scanner(&inputHandler);
    
    Token token = scanner.nextToken();
    
    while (token != NULLTOKEN) {
        //cout << tokenStr[token] << ' ' << scanner.getTokenBuffer() << '\n';
        BOOST_REQUIRE_NE( token, ERROR );
        token = scanner.nextToken();
    }
}


BOOST_AUTO_TEST_CASE( complexTest ) 
{
    std::ifstream file("fixture/complex_test.sql", std::ifstream::in);
    BOOST_REQUIRE( file.good() );
    InputHandler inputHandler(dynamic_cast<std::istream*>(&file));
    SQLScanner scanner(&inputHandler);

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
    boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

    std::ifstream file("fixture/100w_test.sql", std::ifstream::in);
    BOOST_REQUIRE( file.good() );

    InputHandler inputHandler(dynamic_cast<std::istream*>(&file));
    SQLScanner scanner(&inputHandler);

    int count = 0;
    Token token = scanner.nextToken();
    
    while (token != NULLTOKEN)
    {
        if (token == TERMINATOR) count++;
        token = scanner.nextToken();
    }
    
    boost::posix_time::ptime stop_time = boost::posix_time::microsec_clock::local_time();
    
    boost::posix_time::time_duration dur_ms = stop_time - start_time;
    BOOST_TEST_MESSAGE( "100w SQL time cost: " << dur_ms);
    BOOST_REQUIRE_EQUAL( count, 1000000 );
}

BOOST_AUTO_TEST_SUITE_END()

