#include "CppTest.h"
#include "Logger.h"

class CPPTest {
public:
    static void testLogFunc() {
        LOG_WARN("CPP WORKED");
    }
};

extern "C" void CPPTest_testLogFunc(void)
{
    CPPTest::testLogFunc();
}