

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <cstring>
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "BufferManager.hpp"


using std::cout;
using namespace boost::unit_test;
using namespace miniSQL;

BOOST_AUTO_TEST_SUITE ( buffermanager_test_suite )

BOOST_AUTO_TEST_CASE( DbFileOperations ) 
{
    auto filename = "bufMgrTest";
    {
        // DbFile create
        BufferManager bufferManager;
        bufferManager.init();
        {
            auto file = bufferManager.createDbFile(filename);
            BOOST_REQUIRE( file != nullptr );
        }
    }

    {
        // DbFile load
        BufferManager bufferManager;
        bufferManager.init();
        {
            auto file = bufferManager.loadDbFile(filename);
            BOOST_REQUIRE( file != nullptr );
            BOOST_REQUIRE( bufferManager.createPage(filename) == 1 );
            BOOST_REQUIRE( bufferManager.createPage(filename) == 2 );
        }
        BOOST_REQUIRE( bufferManager.deletePage(filename, 1) );
        //BOOST_REQUIRE( bufferManager.getPage(filename, 1) == nullptr );
        BOOST_REQUIRE( bufferManager.createPage(filename) == 1);
        BOOST_REQUIRE( bufferManager.createPage(filename) == 3);
    }

}

BOOST_AUTO_TEST_SUITE_END()

