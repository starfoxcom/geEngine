#pragma once

#include <gePrerequisitesUtil.h>

using geEngineSDK::String;

namespace sf {
  class Texture;
}

class RTSTexture
{
 public:
   RTSTexture();
   ~RTSTexture();

   void
   loadFromFile(const String&);

   sf::Texture*
   getTexture() {
     GE_ASSERT(m_texture);
     return m_texture;
   }

 protected:

 private:
   sf::Texture* m_texture;
};
