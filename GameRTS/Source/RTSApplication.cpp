#include <gePrerequisitesUtil.h>

#if GE_PLATFORM == GE_PLATFORM_WIN32
# include <Win32/geWin32Windows.h>
#endif

#include <geRTTIPlainField.h>
#include <geException.h>
#include <geMath.h>

#include <geCrashHandler.h>
#include <geDynLibManager.h>
#include <geFileSystem.h>
#include <geTime.h>
#include <geUnicode.h>

#include <SFML/Graphics.hpp>

#include <commdlg.h>
#include <imgui.h>
#include <imgui-sfml.h>

#include "RTSConfig.h"
#include "RTSApplication.h"
#include "RTSTiledMap.h"
#include "RTSPathfinding.h"

std::vector<std::string> v_pathName =
{
  "Depth First Search",
  "Breath First Search",
  "Greedy Best First Search",
  "Dijkstra",
  "A*"
};

void
loadMapFromFile(RTSApplication* pApp);

void
mainMenu(RTSApplication* pApp);

RTSApplication::RTSApplication()
  : m_window(nullptr),
    m_fpsTimer(0.0f),
    m_fpsCounter(0.0f),
    m_framesPerSecond(0.0f)
{}

RTSApplication::~RTSApplication() {}

int32
RTSApplication::run() {
  CrashHandler::startUp();
  DynLibManager::startUp();
  Time::startUp();
  GameOptions::startUp();
  EditorOptions::startUp();
  PathfindingOptions::startUp();

  __try {
    initSystems();
    gameLoop();
    destroySystems();
  }
  __except (g_crashHandler().reportCrash(GetExceptionInformation())) {
    PlatformUtility::terminate(true);
  }

  PathfindingOptions::shutDown();
  EditorOptions::shutDown();
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
  m_window = ge_new<sf::RenderWindow>(sf::VideoMode(GameOptions::s_Resolution.x,
                                                    GameOptions::s_Resolution.y),
                                      "RTS Game",
                                      sf::Style::Fullscreen);
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

  //m_window->setVerticalSyncEnabled(true);

  initGUI();
}

void
RTSApplication::initGUI() {
  ImGui::SFML::Init(*m_window);
}

void
RTSApplication::destroySystems() {
  ImGui::SFML::Shutdown();

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
      ImGui::SFML::ProcessEvent(event);
      
      if (event.type == sf::Event::Closed) {
        m_window->close();
      }
    }

    g_time()._update();
    ge_frame_mark();
    updateFrame();
    renderFrame();
    ge_frame_clear();
  }

  postDestroy();
}

void
RTSApplication::updateFrame() {
  float deltaTime = g_time().getFrameDelta();
  sf::Clock clock;
  
  m_fpsTimer += deltaTime;
  if (1.0f < m_fpsTimer) {
    m_framesPerSecond = m_fpsCounter;
    m_fpsCounter = 0.0f;
    m_fpsTimer = 0.0f;
  }
  m_fpsCounter += 1.0f;

  //Update the interface
  ImGui::SFML::Update(*m_window, clock.getElapsedTime());

  //Begin the menu 
  mainMenu(this);

  auto tiledMap = m_gameWorld.getTiledMap();

  if (EditorOptions::s_editorIsOpen)
  {
    ImGui::Begin("Terrain Editor",
      &EditorOptions::s_editorIsOpen,
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
    {

      //Radio buttons
      for (int32 i = 0; i < TERRAIN_TYPE::E::kNumObjects; ++i)
      {
        ImGui::RadioButton(v_terrainName[i].c_str(), &EditorOptions::s_selected, i);
      }

      //Sliders
      ImGui::SliderInt("Brush size", &EditorOptions::s_brushSize, 1, 15);
    }
    ImGui::End();

    //Paint tile
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)
      && !ImGui::IsMouseHoveringAnyWindow()
      && !ImGui::IsAnyWindowFocused())
    {
      int32 tileX, tileY;
      auto mousePos = sf::Mouse::getPosition();
      tiledMap->getScreenToMapCoords(mousePos.x, mousePos.y, tileX, tileY);
      if (EditorOptions::s_brushSize == 1)
        tiledMap->setType(tileX, tileY, static_cast<uint8>(EditorOptions::s_selected));
      else
        for (int32 i = -EditorOptions::s_brushSize >> 1; i < EditorOptions::s_brushSize >> 1; ++i)
          for (int32 j = -EditorOptions::s_brushSize >> 1; j < EditorOptions::s_brushSize >> 1; ++j)
            if (tileX + j > -1 &&
              tileY + i > -1 &&
              tileX + j < tiledMap->getMapSize().x &&
              tileY + i < tiledMap->getMapSize().y)
              tiledMap->setType(tileX + j, tileY + i, static_cast<uint8>(EditorOptions::s_selected));
    }
  }

  if (PathfindingOptions::s_editorIsOpen)
  {
    ImGui::Begin("Path finding tools",
      &PathfindingOptions::s_editorIsOpen,
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
    {
      //Radio buttons
      for (int32 i = 0; i < PATHFINDING_TYPE::E::kNumObjects; ++i)
      {
        ImGui::RadioButton(v_pathName[i].c_str(), &PathfindingOptions::s_selected, i);
      }

      getWorld()->setCurrentWalker(PathfindingOptions::s_selected);

      ImGui::RadioButton("Start position", &PathfindingOptions::s_posSelected, 0);
      ImGui::SameLine();
      ImGui::RadioButton("Target position", &PathfindingOptions::s_posSelected, 1);

      if (sf::Mouse::isButtonPressed(sf::Mouse::Left)
        && !ImGui::IsMouseHoveringAnyWindow()
        && !ImGui::IsAnyWindowFocused())
      {
        getWorld()->clearPathLine();
        getWorld()->getCurrentWalker()->resetSearch();
        
        int32 tileX, tileY;

        auto mousePos = sf::Mouse::getPosition();

        if (PathfindingOptions::s_posSelected == 0)
        {
          tiledMap->getScreenToMapCoords(mousePos.x, mousePos.y,
            tileX,
            tileY);
          getWorld()->getCurrentWalker()->setStartPos(tileX, tileY);
        }
        else if (PathfindingOptions::s_posSelected == 1)
        {
          tiledMap->getScreenToMapCoords(mousePos.x, mousePos.y,
            tileX,
            tileY);
          getWorld()->getCurrentWalker()->setTargetPos(tileX, tileY);
        }
      }

      if (ImGui::Button("Start search"))
      {
        getWorld()->clearPathLine();
        getWorld()->getCurrentWalker()->startSearch();
      }
      ImGui::SameLine();
      if (ImGui::Button("Reset search"))
      {
        getWorld()->clearPathLine();
        getWorld()->getCurrentWalker()->resetSearch();
      }

    }
    ImGui::End();
  }


  //Check for camera movement
  Vector2 axisMovement(FORCE_INIT::kForceInitToZero);
  Vector2I mousePosition;
  mousePosition.x = sf::Mouse::getPosition(*m_window).x;
  mousePosition.y = sf::Mouse::getPosition(*m_window).y;

  if (0 == mousePosition.x ||
      sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
      sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
#ifdef MAP_IS_ISOMETRIC
    axisMovement += Vector2(-1.f, 1.f);
#else
    axisMovement += Vector2(-1.f, 0.f);
#endif
  }
  if (GameOptions::s_Resolution.x -1 == mousePosition.x ||
      sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
      sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
#ifdef MAP_IS_ISOMETRIC
    axisMovement += Vector2(1.f, -1.f);
#else
    axisMovement += Vector2(1.f, 0.f);
#endif
  }
  if (0 == mousePosition.y ||
      sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
      sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
#ifdef MAP_IS_ISOMETRIC
    axisMovement += Vector2(-1.f, -1.f);
#else
    axisMovement += Vector2(0.f, -1.f);
#endif
  }
  if (GameOptions::s_Resolution.y - 1 == mousePosition.y ||
      sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
      sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
#ifdef MAP_IS_ISOMETRIC
    axisMovement += Vector2(1.f, 1.f);
#else
    axisMovement += Vector2(0.f, 1.f);
#endif
  }

  axisMovement *= GameOptions::s_MapMovementSpeed * deltaTime;

  tiledMap->moveCamera(axisMovement.x, axisMovement.y);

  //Update the world
  m_gameWorld.update(deltaTime);
}

void
RTSApplication::renderFrame() {
  m_window->clear(sf::Color::Blue);

  m_gameWorld.render();

  ImGui::SFML::Render(*m_window);

  /*
  sf::Text text;
  text.setPosition(0.f, 30.f);
  text.setFont(*m_arialFont);
  text.setCharacterSize(24);
  text.setFillColor(sf::Color::Red);
  text.setString( toString(1.0f/g_time().getFrameDelta()).c_str() );
  m_window->draw(text);
  */
  m_window->display();
}

void
RTSApplication::postInit() {
  m_gameWorld.init(m_window);
  m_gameWorld.updateResolutionData();
}

void
RTSApplication::postDestroy() {
  m_gameWorld.destroy();
}



void
loadMapFromFile(RTSApplication* pApp) {
  OPENFILENAMEW ofn = { 0 };

  WString fileName;
  fileName.resize(MAX_PATH);
  bool bMustLoad = false;

  Path currentDirectory = FileSystem::getWorkingDirectoryPath();

  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = nullptr;
  ofn.lpstrDefExt = L".bmp";
  ofn.lpstrFilter = L"Bitmap File\0*.BMP\0All\0*.*\0";
  ofn.lpstrInitialDir = L"Maps\\";
  ofn.lpstrFile = &fileName[0];
  ofn.lpstrFile[0] = '\0';
  ofn.nMaxFile = MAX_PATH;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

  if (GetOpenFileNameW(&ofn)) {
    if (fileName.size() > 0) {
      bMustLoad = true;
    }
  }

  SetCurrentDirectoryW(UTF8::toWide(currentDirectory.toString()).c_str());

  if (bMustLoad) {
    pApp->getWorld()->getTiledMap()->loadFromImageFile(pApp->getRenderWindow(),
                                                       toString(fileName));
  }
}

void
mainMenu(RTSApplication* pApp) {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Map")) {
      if (ImGui::MenuItem("Load...", "CTRL+O")) {
        loadMapFromFile(pApp);
      }
      if (ImGui::MenuItem("Save...", "CTRL+S")) {

      }
      ImGui::Separator();

      if (ImGui::MenuItem("Quit", "CTRL+Q")) {
        pApp->getRenderWindow()->close();
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Terrain editor"))
    {
      if (ImGui::MenuItem("Open terrain editor...", "CTRL+T"))
      {
        EditorOptions::s_editorIsOpen = true;
      }

      ImGui::EndMenu();
    }
    
    ImGui::EndMainMenuBar();
  }

  ImGui::Begin("Game Options", 0, ImGuiWindowFlags_AlwaysAutoResize);
  {

    //Text
    ImGui::Text("Framerate: %f", pApp->getFPS());

    //Sliders
    ImGui::SliderFloat("Map movement speed X",
      &GameOptions::s_MapMovementSpeed.x,
      0.0f,
      10240.0f);
    ImGui::SliderFloat("Map movement speed Y",
      &GameOptions::s_MapMovementSpeed.y,
      0.0f,
      10240.0f);

    //Check boxes
    ImGui::Checkbox("Show grid", &GameOptions::s_MapShowGrid);
    ImGui::Checkbox("Show Terrain Editor", &EditorOptions::s_editorIsOpen);
    ImGui::Checkbox("Show Path finding tools", &PathfindingOptions::s_editorIsOpen);
  }
  ImGui::End();

}
