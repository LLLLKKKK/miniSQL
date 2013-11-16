

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <cstring>
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "RecordManager.h"


using std::cout;
using namespace boost::unit_test;
using namespace miniSQL;

RecordInfo fakeRecordInfo() {
    RecordInfo recordInfo;
    recordInfo.size = 30;

    FieldInfo fieldInfo;
    fieldInfo.offset = 0;
    fieldInfo.type.baseType = FloatType;
    fieldInfo.type.length = 0;
    recordInfo.fieldInfoMap["columnA"] = fieldInfo;
    fieldInfo.offset = 8;
    fieldInfo.type.baseType = IntType;
    fieldInfo.type.length = 0;
    recordInfo.fieldInfoMap["columnB"] = fieldInfo;
    fieldInfo.offset = 12;
    fieldInfo.type.baseType = CharType;
    fieldInfo.type.length = 10;
    recordInfo.fieldInfoMap["columnC"] = fieldInfo;
        
    recordInfo.fields.push_back("columnB");
    recordInfo.fields.push_back("columnC");
    recordInfo.fields.push_back("columnA");

    return recordInfo;
}

BOOST_AUTO_TEST_SUITE ( recordmanager_test_suite )

BOOST_AUTO_TEST_CASE( record_simple_test ) 
{
    auto bufferMgr = std::make_shared<BufferManager>();
    bufferMgr->init();
    auto recordInfo = fakeRecordInfo();
    auto recordMgr = std::unique_ptr<RecordManager>(
            new RecordManager(bufferMgr, "record_test", recordInfo));

    BOOST_CHECK( recordMgr->init(true) );

    auto pos = recordMgr->getRecordBeginPos();
    Record record1(recordInfo);
    record1.putField(0, 123456);
    record1.putField(1, std::string("1234567890"));
    record1.putField(2, 1.31f);
    BOOST_CHECK( recordMgr->insertRecord(record1) );
    
    recordMgr->moveToNextPos(pos, RECORD_EMPTY);
    record1.putField(0, 456789);
    record1.putField(1, std::string("3125124131"));
    record1.putField(2, 5.231f);
    BOOST_CHECK( recordMgr->insertRecord(record1) );
    
    auto beginPos = recordMgr->getRecordBeginPos();
    Record record2(recordInfo);
    BOOST_CHECK( recordMgr->getRecord(beginPos, record2) );

    {
        int i;
        record2.getField(0, i);
        BOOST_CHECK_EQUAL( 123456, i );
        float f;
        record2.getField(2, f);
        BOOST_CHECK_EQUAL( 1.31f, f );
        std::string c;
        record2.getField(1, c);
        BOOST_CHECK_EQUAL( std::string("1234567890"), c );
    }

    recordMgr->moveToNextPos(beginPos, RECORD_VERSION);
    BOOST_CHECK( recordMgr->getRecord(beginPos, record2) );

    {
        int i;
        record2.getField(0, i);
        BOOST_CHECK_EQUAL( 456789, i );
        float f;
        record2.getField(2, f);
        BOOST_CHECK_EQUAL( 5.231f, f );
        std::string c;
        record2.getField(1, c);
        BOOST_CHECK_EQUAL( std::string("3125124131"), c );
    }
}

BOOST_AUTO_TEST_CASE( record_multipage_test ) 
{
    auto bufferMgr = std::make_shared<BufferManager>();
    bufferMgr->init();
    auto recordInfo = fakeRecordInfo();
    auto recordMgr = std::unique_ptr<RecordManager>(
            new RecordManager(bufferMgr, "record_test", recordInfo));
    
    BOOST_CHECK( recordMgr->init(true) );
    
    int times = 500;
    Record record1(recordInfo);
    for (int i=0;i<times;i++) {
        record1.putField(0, 123456);
        record1.putField(1, std::string("1234567890"));
        record1.putField(2, 1.31f);
        BOOST_CHECK( recordMgr->insertRecord(record1) );
    }
    
    auto beginPos = recordMgr->getRecordBeginPos();
    Record record2(recordInfo);
    for (int i = 0;  i< times; i++) {
        BOOST_CHECK( INVALID_FILEPOSITION != beginPos );
        BOOST_CHECK( recordMgr->getRecord(beginPos, record2) );
        {
            int i;
            record2.getField(0, i);
            BOOST_CHECK_EQUAL( 123456, i );
            float f;
            record2.getField(2, f);
            BOOST_CHECK_EQUAL( 1.31f, f );
            std::string c;
            record2.getField(1, c);
            BOOST_CHECK_EQUAL( std::string("1234567890"), c );
        }
        recordMgr->moveToNextPos(beginPos, RECORD_VERSION);
    }
    BOOST_CHECK( INVALID_FILEPOSITION == beginPos );
}

BOOST_AUTO_TEST_SUITE_END()

