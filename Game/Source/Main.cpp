#include <vld.h>
#include <iostream>
#include <gePrerequisitesUtil.h>

using std::cout;
using std::endl;

using namespace geEngineSDK;

int
main() {

  String testString = "This is a test";
  {
    String testWString = geEngineSDK::toString(WString(L"What!"));
  }

  cout << StringUtil::format("{0}\n", true);
  cout << testString << endl;
  //cout << testWString;

  return 0;
}
