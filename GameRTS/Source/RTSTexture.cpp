#include "RTSTexture.h"

#include <geException.h>

#include <SFML/Graphics.hpp>

using namespace geEngineSDK;

RTSTexture::RTSTexture()
  : m_texture(nullptr)
{}

RTSTexture::~RTSTexture() {
  if (nullptr != m_texture) {
    ge_delete(m_texture);
  }
}

void
RTSTexture::loadFromFile(const String& fileName) {
  GE_ASSERT(nullptr == m_texture);

  m_texture = ge_new<sf::Texture>();
  
  if (!m_texture->loadFromFile(fileName.c_str())) {
    GE_EXCEPT(FileNotFoundException, "Texture not found: " + fileName);
  }
}
