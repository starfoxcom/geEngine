#pragma once

#include "RTSTexture.h"

using namespace geEngineSDK;

namespace sf{
  class RenderWindow;
  class Font;
}

class RTSApplication
{
 public:
  RTSApplication();
  virtual ~RTSApplication();

  int32
  run();

 protected:

 private:
   void
   initSystems();

   void
   destroySystems();

   void
   gameLoop();

   void
   postInit();

   void
   postDestroy();

   void
   updateFrame();
   
   void
   renderFrame();

   sf::RenderWindow* m_window;
   sf::Font* m_arialFont;
   
   RTSTexture m_testTexture;
};
