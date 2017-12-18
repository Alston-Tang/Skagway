//
// Created by alston on 12/13/17.
//

#include <cxxtest/TestSuite.h>
#include <arpa/inet.h>
#include "../protocol.h"

class ProtoTest : public CxxTest::TestSuite
{
public:
    void testKeepAliveC() {
        Skagway::KeepAliveC p{};
        TS_ASSERT_EQUALS(sizeof(p), 4);
    }
    void testKeepAliveS() {
        Skagway::KeepAliveS p{};
        TS_ASSERT_EQUALS(sizeof(p), 8);
    }
    void testDataSeg() {
        Skagway::DataSegment p{};
        TS_ASSERT_EQUALS(sizeof(p), 12);
    }
};
