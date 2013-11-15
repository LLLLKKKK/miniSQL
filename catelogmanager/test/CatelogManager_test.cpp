
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "CatelogManager.h"

using std::cout;
using namespace boost::unit_test;
using namespace miniSQL;

BOOST_AUTO_TEST_SUITE ( catelogmanager_test_suite )

BOOST_AUTO_TEST_CASE( catelogReadAndWrite ) 
{
    {
        TableInfo tableInfo;
        tableInfo.indexMap["indexA"] = "columnA";
        tableInfo.indexMap["indexB"] = "columnB";
        tableInfo.name = "tableA";
        tableInfo.recordInfo.size = 20;
        FieldInfo fieldInfo;
        fieldInfo.offset = 0;
        fieldInfo.type.baseType = FloatType;
        fieldInfo.type.length = 0;
        tableInfo.recordInfo.fieldInfoMap["columnA"] = fieldInfo;
        fieldInfo.offset = 8;
        fieldInfo.type.baseType = IntType;
        fieldInfo.type.length = 0;
        tableInfo.recordInfo.fieldInfoMap["columnB"] = fieldInfo;
        fieldInfo.offset = 12;
        fieldInfo.type.baseType = CharType;
        fieldInfo.type.length = 10;
        tableInfo.recordInfo.fieldInfoMap["columnC"] = fieldInfo;

        CatelogManager catelogManager;
        catelogManager.readCatelog();
        catelogManager.addCatelog(tableInfo);
    }

    {
        TableInfo tableInfo;
        CatelogManager catelogManager;
        BOOST_CHECK(catelogManager.readCatelog());
        catelogManager.getCatelog("tableA", tableInfo);

        BOOST_CHECK_EQUAL(tableInfo.indexMap["indexA"], "columnA");
        BOOST_CHECK_EQUAL(tableInfo.indexMap["indexB"], "columnB");
        BOOST_CHECK_EQUAL(tableInfo.name, "tableA");
        BOOST_CHECK_EQUAL(tableInfo.recordInfo.size, 20);

        FieldInfo fieldInfo = tableInfo.recordInfo.fieldInfoMap["columnA"];
        BOOST_CHECK_EQUAL(0, fieldInfo.offset);
        BOOST_CHECK_EQUAL(FloatType, fieldInfo.type.baseType);
        BOOST_CHECK_EQUAL(0, fieldInfo.type.length);
        
        fieldInfo = tableInfo.recordInfo.fieldInfoMap["columnB"];
        BOOST_CHECK_EQUAL(8, fieldInfo.offset);
        BOOST_CHECK_EQUAL(IntType, fieldInfo.type.baseType);
        BOOST_CHECK_EQUAL(0, fieldInfo.type.length);

        fieldInfo = tableInfo.recordInfo.fieldInfoMap["columnC"];
        BOOST_CHECK_EQUAL(12, fieldInfo.offset);
        BOOST_CHECK_EQUAL(CharType, fieldInfo.type.baseType);
        BOOST_CHECK_EQUAL(10, fieldInfo.type.length);
    }
}

BOOST_AUTO_TEST_SUITE_END()

