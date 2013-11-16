
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
        tableInfo.indexToColumnMap["indexA"] = "columnA";
        tableInfo.indexToColumnMap["indexB"] = "columnB";
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

        tableInfo.recordInfo.fields.push_back("columnB");
        tableInfo.recordInfo.fields.push_back("columnC");
        tableInfo.recordInfo.fields.push_back("columnA");

        CatelogManager catelogManager;
        BOOST_CHECK(catelogManager.readTables());
        catelogManager.addTable(tableInfo);
    }

    {
        TableInfo tableInfo;
        CatelogManager catelogManager;
        BOOST_CHECK(catelogManager.readTables());
        catelogManager.getTable("tableA", tableInfo);

        BOOST_CHECK_EQUAL(tableInfo.indexToColumnMap["indexA"], "columnA");
        BOOST_CHECK_EQUAL(tableInfo.indexToColumnMap["indexB"], "columnB");
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

