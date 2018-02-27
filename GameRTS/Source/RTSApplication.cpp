#include <gePrerequisitesUtil.h>

#if GE_PLATFORM == GE_PLATFORM_WIN32
# include <Win32/geWin32Windows.h>
#endif

#include <geRTTIPlainField.h>
#include <geException.h>
#include <geMath.h>

#include <geCrashHandler.h>
#include <geDynLibManager.h>
#include <geTime.h>

#include <SFML/Graphics.hpp>

#include "RTSConfig.h"
#include "RTSApplication.h"

RTSApplication::RTSApplication() : m_window(nullptr)
{}

RTSApplication::~RTSApplication() {}

int32
RTSApplication::run() {
  CrashHandler::startUp();
  DynLibManager::startUp();
  Time::startUp();
  GameOptions::startUp();

  __try {
    initSystems();
    gameLoop();
    destroySystems();
  }
  __except (g_crashHandler().reportCrash(GetExceptionInformation())) {
    PlatformUtility::terminate(true);
  }

  GameOptions::shutDown();
  Time::shutDown();
  DynLibManager::shutDown();
  CrashHandler::shutDown();

  return 0;
}

void
RTSApplication::initSystems() {
  if (nullptr != m_window) {  //Window already initialized
    return; //Shouldn't do anything
  }

  //Create the application window
  m_window = ge_new<sf::RenderWindow>(sf::VideoMode(1920, 1080), "RTS Game");
  if (nullptr == m_window) {
    GE_EXCEPT(InvalidStateException, "Couldn't create Application Window");
  }

  m_arialFont = ge_new<sf::Font>();
  if (nullptr == m_arialFont) {
    GE_EXCEPT(InvalidStateException, "Couldn't create a Font");
  }
  
  
  if (!m_arialFont->loadFromFile("Fonts/arial.ttf")) {
    GE_EXCEPT(FileNotFoundException, "Arial font not found");
  }
}

void
RTSApplication::destroySystems() {
  if (nullptr != m_window) {
    m_window->close();
    ge_delete(m_window);
  }

  if (nullptr != m_arialFont) {
    ge_delete(m_arialFont);
  }
}

void
RTSApplication::gameLoop() {
  if (nullptr == m_window) {  //Windows not yet initialized
    return; //Shouldn't do anything
  }

  postInit();

  while (m_window->isOpen()) {
    sf::Event event;
    while (m_window->pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        m_window->close();
    }

    g_time()._update();
    updateFrame();
    renderFrame();
  }

  postDestroy();
}

void
RTSApplication::updateFrame() {

}

void
RTSApplication::renderFrame() {
  sf::Sprite sprite;
  sf::Text text;
  sf::Texture& texture = *m_testTexture.getTexture();
  
  m_window->clear(sf::Color::White);

  sprite.setTexture(texture);
  sprite.setPosition(m_window->getSize().x*0.5f, m_window->getSize().y*0.5f);
  sprite.setOrigin(texture.getSize().x*0.5f, texture.getSize().y*0.5f);
  sprite.setRotation(g_time().getTimeMs());
  m_window->draw(sprite);
  
  text.setFont(*m_arialFont);
  text.setCharacterSize(24);
  text.setFillColor(sf::Color::Red);
  text.setString( toString(g_time().getFrameDelta()).c_str() );
  m_window->draw(text);

  m_window->display();
}

void
RTSApplication::postInit() {
  m_testTexture.loadFromFile("Textures/Terrain/terrain_1.png");
}

void
RTSApplication::postDestroy() {

}
