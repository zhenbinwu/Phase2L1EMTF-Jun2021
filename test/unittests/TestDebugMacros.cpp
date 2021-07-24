#include "Utilities/Testing/interface/CppUnit_testdriver.icpp"
#include "cppunit/extensions/HelperMacros.h"

#include "L1Trigger/Phase2L1EMTF/interface/Defines.h"

class TestDebugTools : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(TestDebugTools);
  CPPUNIT_TEST(test_assert);
  CPPUNIT_TEST(test_dump);
  CPPUNIT_TEST_SUITE_END();

public:
  TestDebugTools() {}
  ~TestDebugTools() {}
  void setUp() {}
  void tearDown() {}

  void test_assert();
  void test_dump();
};

///registration of the test so that the runner can find it
CPPUNIT_TEST_SUITE_REGISTRATION(TestDebugTools);

// Make sure EMTF_ALLOW_ASSERT is not active
void TestDebugTools::test_assert() {
#ifdef EMTF_ALLOW_ASSERT
  CPPUNIT_FAIL("Please make sure EMTF_ALLOW_ASSERT is not defined in interface/Defines.h");
#endif
}

// Make sure EMTF_DUMP_INFO is not defined
void TestDebugTools::test_dump() {
#ifdef EMTF_DUMP_INFO
  CPPUNIT_FAIL("Please make sure EMTF_DUMP_INFO is not defined in interface/Defines.h");
#endif
}
