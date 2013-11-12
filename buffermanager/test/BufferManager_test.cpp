

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
    {
        // DbFile create
        BufferManager bufferManager;
        bufferManager.init();
        {
            auto file = bufferManager.createDbFile("bufMgrTest");
            BOOST_REQUIRE( file != nullptr );
        }
    }

    {
        // DbFile load
        BufferManager bufferManager;
        bufferManager.init();
        {
            auto file = bufferManager.loadDbFile("bufMgrTest");
            BOOST_REQUIRE( file != nullptr );
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()

