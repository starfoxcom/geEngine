#include <iostream>
#include <gePrerequisitesUtil.h>

using std::cout;
using std::endl;

using namespace geEngineSDK;

int
main() {
  cout << StringUtil::format("{0}__{1}__{2}", 123, 456.789f, "texto\n");
  return 0;
}
