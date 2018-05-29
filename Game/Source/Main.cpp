#include <vld.h>
#include <iostream>

#if GE_PLATFORM == GE_PLATFORM_WIN32
#include <Win32/geWin32Windows.h>
#endif

#include <gePrerequisitesUtil.h>
#include <geRTTIPlainField.h>
#include <geException.h>

#include <geCrashHandler.h>
#include <geDynLibManager.h>
#include <geTime.h>

#include <geCoreApplication.h>

using namespace geEngineSDK;

void
runApplication();

int
main() {
  CrashHandler::startUp();

  __try {
    runApplication();
  }
  __except(g_crashHandler().reportCrash(GetExceptionInformation())) {
    PlatformUtility::terminate(true);
  }
  
  CrashHandler::shutDown();

  return 0;
}

void
runApplication() {
  START_UP_DESC startUpDesc;
  /*
  startUpDesc.renderAPI = GE_RENDER_API_MODULE;
  startUpDesc.renderer = GE_RENDERER_MODULE;
  startUpDesc.audio = GE_AUDIO_MODULE;
  startUpDesc.physics = GE_PHYSICS_MODULE;
  startUpDesc.scripting = false;
  */
  startUpDesc.primaryWindowDesc.videoMode = VideoMode(800, 600);
  startUpDesc.primaryWindowDesc.title = "Game Title";
  startUpDesc.primaryWindowDesc.fullscreen = false;
  startUpDesc.primaryWindowDesc.hidden = false;
  startUpDesc.primaryWindowDesc.depthBuffer = false;

  CoreApplication::startUp(startUpDesc);
  CoreApplication::instance().runMainLoop();
  CoreApplication::shutDown();
}