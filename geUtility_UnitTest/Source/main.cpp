#include <gtest/gtest.h>
#include <gePrerequisitesUtil.h>
#include <geMath.h>

using namespace geEngineSDK;

TEST(geUtility, DEFINED_TYPES_SIZES) {
  ASSERT_TRUE(sizeof(uint8  ) == 1);
  ASSERT_TRUE(sizeof(uint16 ) == 2);
  ASSERT_TRUE(sizeof(uint32 ) == 4);
  ASSERT_TRUE(sizeof(uint64 ) == 8);
  ASSERT_TRUE(sizeof(uint128) == 16);
  
  ASSERT_TRUE(sizeof(int8  ) == 1);
  ASSERT_TRUE(sizeof(int16 ) == 2);
  ASSERT_TRUE(sizeof(int32 ) == 4);
  ASSERT_TRUE(sizeof(int64 ) == 8);
  ASSERT_TRUE(sizeof(int128) == 16);

  ASSERT_TRUE(sizeof(float ) == 4);
  ASSERT_TRUE(sizeof(double) == 8);

  ASSERT_TRUE(static_cast<uint32>(-1) == Math::MAX_UINT32);
}

TEST(geUtility, String) {
  String testString = "This is a test";

  ASSERT_TRUE(testString.size() == 14);
}