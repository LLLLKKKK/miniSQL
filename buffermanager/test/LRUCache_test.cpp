
#include <cstring>
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "Page.hpp"
#include "LRUCache.hpp"

using std::cout;
using namespace boost::unit_test;
using namespace miniSQL;

struct PageGetSizeCallback {
    uint64_t operator() (PagePtr page) {
        return PAGE_SIZE;
    }
};

BOOST_AUTO_TEST_SUITE ( buffermanager_test_suite )

BOOST_AUTO_TEST_CASE( simpleTest ) 
{
    LRUCache<PageID, PagePtr, PageGetSizeCallback> cache(BUFFER_SIZE);
    PagePtr page;
    BOOST_REQUIRE( cache.put(1, page) );
    BOOST_REQUIRE( cache.get(1, page) );
}

BOOST_AUTO_TEST_SUITE_END()

