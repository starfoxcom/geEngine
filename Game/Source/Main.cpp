//#include <vld.h>
#include <iostream>

#if GE_PLATFORM == GE_PLATFORM_WIN32
# include <windows.h>
#endif

#include <gePrerequisitesUtil.h>
#include <geRTTIPlainField.h>
#include <geException.h>

using std::cout;
using std::endl;

using namespace geEngineSDK;

int
main() {
  CrashHandler::startUp();

  __try {
    //EditorApplication::startUp(EditorRenderAPI::DX11);
    //EditorApplication::instance().runMainLoop();
    //EditorApplication::shutDown();
  }
  __except(g_crashHandler().reportCrash(GetExceptionInformation())) {
    PlatformUtility::terminate(true);
  }
  
  CrashHandler::shutDown();

  return 0;
}
