//
// Created by alston on 12/15/17.
//

#include <cxxtest/TestSuite.h>
#include <unistd.h>
#include "../utility.h"

class UtilityTest : public CxxTest::TestSuite
{
public:
    void testCurrentTime() {
        uint32_t s = Skagway::current_time();
        sleep(1);
        uint32_t e = Skagway::current_time();
        
        TS_ASSERT_LESS_THAN(e - s, 1500);
        TS_ASSERT_LESS_THAN(700, e - s);
    }

    void testTimevalToTime() {
        timeval time = {1000000000, 500000};

        TS_ASSERT_EQUALS(Skagway::time_val_to_time(time), 3567587828);
    }
};
