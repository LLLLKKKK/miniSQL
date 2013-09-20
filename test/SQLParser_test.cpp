

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <cstring>
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "ParseTreePrinter.hpp"
#include "../src/SQLScanner.hpp"
#include "../src/InputHandler.hpp"
#include "../src/SQLParser.hpp"

using std::cout;
using namespace boost::unit_test;
using namespace miniSQL;

BOOST_AUTO_TEST_SUITE ( parser_test_suite )

BOOST_AUTO_TEST_CASE( simpleTest ) 
{
    std::map<std::string,std::string> testFiles;
    testFiles["create.sql"] = "create.tree";
    testFiles["insert.sql"] = "create.tree";
    testFiles["drop.sql"] = "create.tree";
    testFiles["select.sql"] = "create.tree";
    testFiles["delete.sql"] = "create.tree";
    testFiles["cmd.sql"] = "create.tree";

    for (auto it = testFiles.begin(); it != testFiles.end(); it++) {
        std::string filename = "test/fixture/" + it->first;
        cout << filename << std::endl;
        std::ifstream file(filename, std::ifstream::in);
        BOOST_REQUIRE( file.good() );
        
        InputHandler inputHandler(&file);
        SQLScanner scanner(&inputHandler);
        SQLParser parser(&scanner);
        
        std::list<PARSENODE_PTR > statements = parser.parseSQL();
        
        std::stringstream pstream;
        ParseTreePrinter::print(pstream, statements);
        std::cout << pstream.str() << std::endl;
    }
}


BOOST_AUTO_TEST_CASE( speedTest ) 
{
    boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

    std::ifstream file("test/fixture/100w_test.sql", std::ifstream::in);
    BOOST_REQUIRE( file.good() );

    InputHandler inputHandler(dynamic_cast<std::istream*>(&file));
    SQLScanner scanner(&inputHandler);
    SQLParser parser(&scanner);

    std::list<PARSENODE_PTR > statements = parser.parseSQL();
        

    boost::posix_time::ptime stop_time = boost::posix_time::microsec_clock::local_time();

    std::stringstream pstream;
    ParseTreePrinter::print(pstream, statements.back());
    std::cout << pstream.str() << std::endl;
    
    boost::posix_time::time_duration dur_ms = stop_time - start_time;
    BOOST_TEST_MESSAGE( "100w SQL time cost: " << dur_ms);
}

BOOST_AUTO_TEST_SUITE_END()

