#include <vld.h>
#include <iostream>

#if GE_PLATFORM == GE_PLATFORM_WIN32
#include <Win32/geWin32Windows.h>
#endif

#include <gePrerequisitesUtil.h>
#include <geRTTIPlainField.h>
#include <geException.h>

using std::cout;
using std::endl;

using namespace geEngineSDK;

Win32Window* m_Wnd;

LRESULT CALLBACK
WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  return DefWindowProc(hwnd, msg, wParam, lParam);
}

void
testDump() {
  GE_EXCEPT(UnitTestException, "This was a test");
}

int
main() {
  CrashHandler::startUp();

  __try {
    testDump();
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
