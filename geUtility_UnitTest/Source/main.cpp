//#include <vld.h>
#include <gtest/gtest.h>
#include <gePrerequisitesUtil.h>
#include <geMath.h>
#include <geFileSystem.h>
#include <geDataStream.h>

using namespace geEngineSDK;

TEST(geUtility, DEFINED_TYPES_SIZES) {
  EXPECT_EQ(sizeof(unsigned char), 1);
  EXPECT_EQ(sizeof(uint8  ), 1);
  EXPECT_EQ(sizeof(uint16 ), 2);
  EXPECT_EQ(sizeof(uint32 ), 4);
  EXPECT_EQ(sizeof(uint64 ), 8);
  EXPECT_EQ(sizeof(uint128), 16);
  
  EXPECT_EQ(sizeof(char  ), 1);
  EXPECT_EQ(sizeof(int8  ), 1);
  EXPECT_EQ(sizeof(int16 ), 2);
  EXPECT_EQ(sizeof(int32 ), 4);
  EXPECT_EQ(sizeof(int64 ), 8);
  EXPECT_EQ(sizeof(int128), 16);

  EXPECT_EQ(sizeof(float ), 4);
  EXPECT_EQ(sizeof(double), 8);

  EXPECT_EQ(static_cast<uint32>(-1), Math::MAX_UINT32);
}

TEST(geUtility, String) {
  String testString = "This is a test";
  EXPECT_EQ(testString.size(), 14);

  WString testWString = L"This is a test";
  EXPECT_EQ(testWString.size(), 14);

  EXPECT_EQ(toString(testWString).size(), 14);
  EXPECT_EQ(toWString(testString).size(), 14);

  StringUtil::toLowerCase(testString);
  EXPECT_STRCASEEQ(testString.c_str(), "this is a test");
  StringUtil::toUpperCase(testString);
  EXPECT_STRCASEEQ(testString.c_str(), "THIS IS A TEST");

  StringUtil::toLowerCase(testWString);
  ASSERT_EQ(testWString, L"this is a test");
  StringUtil::toUpperCase(testWString);
  ASSERT_EQ(testWString, L"THIS IS A TEST");
}

TEST(geUtility, Path) {
  Path testPath;
  WString lastDirectory;
  
  testPath = FileSystem::getWorkingDirectoryPath();
  EXPECT_TRUE(testPath.isDirectory());
  EXPECT_TRUE(testPath.getNumDirectories());

  lastDirectory.append(testPath[testPath.getNumDirectories() - 1]);
  EXPECT_TRUE(lastDirectory.compare(L"BIN"));
}

TEST(geUtility, Parser) {
  DataStreamPtr fileData = FileSystem::openFile("Test/test.txt");
  EXPECT_TRUE(fileData);
  if (fileData) {
    String strParse = fileData->getAsString();
    Vector<String> lineList = StringUtil::split(strParse, "\n");

    for (auto& line : lineList) {
      StringUtil::trim(line);
    }

    EXPECT_EQ(lineList.size(), 9);
  }
}

TEST(geUtility, Math) {
  EXPECT_EQ(Math::abs(-1), 1);
  EXPECT_EQ(Math::abs(-1.000000000), 1.000000000);
  EXPECT_FLOAT_EQ(Math::abs(-1.0f), 1.0f);
  EXPECT_TRUE(Math::abs(Radian(-Math::PI)) == Radian(Math::PI));
  EXPECT_TRUE(Math::abs(Degree(-180.0f)) == Degree(180.0f));
  
  EXPECT_FLOAT_EQ(Math::sin(0.f), 0.0f);
  EXPECT_FLOAT_EQ(Math::cos(0.f), 1.0f);
  EXPECT_FLOAT_EQ(Math::tan(Math::HALF_PI*0.5f), 1.0f);

  EXPECT_NEAR(Math::acos(0.f).valueRadians(), 1.57070000f, Math::KINDA_SMALL_NUMBER);
  EXPECT_NEAR(Math::acos(0.f).valueRadians(), 1.57079600f, Math::SMALL_NUMBER);
  EXPECT_NEAR(Math::acos(0.f).valueRadians(), 1.57079633f, 0);

  EXPECT_NEAR(Math::asin(1.f).valueRadians(), 1.57070000f, Math::KINDA_SMALL_NUMBER);
  EXPECT_NEAR(Math::asin(1.f).valueRadians(), 1.57079600f, Math::SMALL_NUMBER);
  EXPECT_NEAR(Math::asin(1.f).valueRadians(), 1.57079633f, 0);

  EXPECT_NEAR(Math::atan(1.f).valueRadians(), 0.785390000f, Math::KINDA_SMALL_NUMBER);
  EXPECT_NEAR(Math::atan(1.f).valueRadians(), 0.785398100f, Math::SMALL_NUMBER);
  EXPECT_NEAR(Math::atan(1.f).valueRadians(), 0.785398185f, 0);

  EXPECT_FLOAT_EQ(Math::atan2(1.f, 1.f).valueRadians(), Math::HALF_PI*0.5f);
  EXPECT_FLOAT_EQ(Degree(Math::atan2(1.f, 1.f)).valueDegrees(), 45.0f);

  EXPECT_FLOAT_EQ(Math::atan2(-1.f, 1.f).valueRadians(), -(Math::HALF_PI*0.5f));
  EXPECT_FLOAT_EQ(Degree(Math::atan2(-1.f, 1.f)).valueDegrees(), -45.0f);

  EXPECT_FLOAT_EQ(Math::atan2(1.f, -1.f).valueRadians(), Math::PI*0.75f);
  EXPECT_FLOAT_EQ(Degree(Math::atan2(1.f, -1.f)).valueDegrees(), 135.0f);

  EXPECT_FLOAT_EQ(Math::atan2(-1.f, -1.f).valueRadians(), -(Math::PI*0.75f));
  EXPECT_FLOAT_EQ(Degree(Math::atan2(-1.f, -1.f)).valueDegrees(), -135.0f);
  
  EXPECT_EQ(Math::ceil(1.000001f), 2);
  EXPECT_EQ(Math::ceil(1.00000001f), 1);
  EXPECT_EQ(Math::ceilFloat(1.000001f), 2.0f);
  EXPECT_EQ(Math::ceilFloat(1.00000001f), 1.0f);
  EXPECT_EQ(Math::ceilDouble(1.000000000000001), 2.0);
  EXPECT_EQ(Math::ceilDouble(1.0000000000000001), 1.0);

  uint32 ceilTmp[] = {0, 0, 1, 2, 2, 3, 3, 3, 3, 4};
  uint32 floorTmp[] = {0, 0, 1, 1, 2, 2, 2, 2, 3, 3};
  for (uint32 i = 0; i < 10; ++i) {
    EXPECT_EQ(Math::ceilLog2(i), ceilTmp[i]);
    EXPECT_EQ(Math::floorLog2(i), floorTmp[i]);
  }

  EXPECT_EQ(Math::clamp(100.0, 1.0, 50.0), 50);
  EXPECT_EQ(Math::clamp(18.0, 1.0, 50.0),18.0);
  EXPECT_EQ(Math::clamp(-9.0, 1.0, 50.0), 1.0);

  EXPECT_EQ(Math::clamp01(100.0f), 1.0f);
  EXPECT_EQ(Math::clamp01(18.0f),  1.0f);
  EXPECT_EQ(Math::clamp01(-9.0f),  0.0f);

  EXPECT_NEAR(Math::fmod(18.5f, 4.2f), 1.7f, Math::SMALL_NUMBER);
  EXPECT_NEAR(Math::exp(1), 2.718282f, Math::SMALL_NUMBER);

}

TEST(geUtility, Platform) {
  PlatformUtility::copyToClipboard(L"Esta es una prueba del portapapeles!");
  WString szClipboardContent = PlatformUtility::copyFromClipboard();
  EXPECT_STRCASEEQ(toString(szClipboardContent).c_str(),
                   "Esta es una prueba del portapapeles!");
}
