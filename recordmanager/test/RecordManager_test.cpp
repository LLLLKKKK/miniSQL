

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <cstring>
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "BufferManager.h"


using std::cout;
using namespace boost::unit_test;
using namespace miniSQL;

BOOST_AUTO_TEST_SUITE ( recordmanager_test_suite )

BOOST_AUTO_TEST_CASE( DbFileOperations ) 
{

    BOOST_REQUIRE( bufferManager.createPage(filename)->id == 3);
    
}

BOOST_AUTO_TEST_SUITE_END()

