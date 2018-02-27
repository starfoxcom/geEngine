#pragma once

#include <gePlatformUtility.h>
#include <geVector2.h>
#include <geVector2I.h>

#include "RTSConfig.h"

using namespace geEngineSDK;

class RTSTexture;

namespace TERRAIN_TYPE {
  enum E {
    kWater = 0,
    kGrass,
    kMarsh,
    kObstacle,
    kNumObjects
  };
}

class RTSTiledMap
{
  class MapTile
  {
   public:
    MapTile();
    MapTile(const int8 idType, const int8 cost);
    MapTile(const MapTile& copy);
    
    MapTile&
    operator=(const MapTile& rhs);

    FORCEINLINE uint8
    getType() const {
      return m_idType;
    }

    void
    setType(const int8 idType) {
      m_idType = idType;
    }

    FORCEINLINE int8
    getCost() const {
      return m_cost;
    }

    void
    setCost(const int8 cost) {
      m_cost = cost;
    }

   private:
    uint8 m_idType;
    int8 m_cost;
  };

 public:
  RTSTiledMap();
  RTSTiledMap(const int32 mapSize);
  ~RTSTiledMap();

 public:
  bool
  init(const int32 mapSize);

  void
  destroy();

  void
  update(float deltaTime);
  
  void
  render();

  int32
  getMapSize() const {
    return m_mapSize;
  }

  bool
  loadFromImageFile(String fileName);

  bool
  saveToImageFile(String fileName);

  int8
  getCost(const int32 x, const int32 y) const;

  void
  setCost(const int32 x, const int32 y, const int8 cost);

  int8
  getType(const int32 x, const int y) const;

  void
  setType(const int32 x, const int32 y, const uint8 idtype);

  void
  setStart(const uint32 x, const uint32 y) {
    m_scrStart.x = x;
    m_scrStart.y = y;
    preCalc();
  }

  void
  getStart(uint32 &x, uint32 &y) const {
    x = m_scrStart.x;
    y = m_scrStart.y;
  }

  void
  setEnd(const uint32 x, const uint32 y) {
    m_scrEnd.x = x;
    m_scrEnd.y = y;
    preCalc();
  }

  void
  getEnd(uint32 &x, uint32 &y) const {
    x = m_scrEnd.x;
    y = m_scrEnd.y;
  }

  void
  preCalc() {
    m_PreCalc_MidResolution = (m_scrEnd - m_scrStart) / 2;
#ifdef MAP_IS_ISOMETRIC
    m_PreCalc_MaxCameraCoord.x = m_mapSize * GameOptions::TILEHALFSIZE.x;
#else	//Estos cálculos aplican solo en el caso de un mapa cuadrado
    m_PreCalc_MaxCameraCoord.x = m_mapSize * TILESIZE_X;
#endif
    m_PreCalc_MaxCameraCoord.y = m_mapSize * TILESIZE_Y;
  }

  void
  moveCamera(const float dx, const float dy);

  void
  setCameraStartPosition(const int32 x, const int32 y);

  void
  getCameraStartPosition(int32 &x, int32 &y) const {
    x = m_iCamera.x;
    y = m_iCamera.y;
  }

  void
  getScreenToMapCoords(const int32 scrX,
                       const int32 scrY,
                       int32 &mapX,
                       int32 &mapY);

  void
  getMapToScreenCoords(const int32 mapX,
                       const int32 mapY,
                       int32 &scrX,
                       int32 &scrY);

 private:
  MapTile** m_mapGrid;
  int32 m_mapSize;

  Vector2I m_iCamera;
  Vector2 m_fCamera;

  Vector2I m_scrStart;
  Vector2I m_scrEnd;

  Vector2I m_PreCalc_MidResolution;
  Vector2I m_PreCalc_MaxCameraCoord;
  Vector2I m_PreCalc_ScreenDeface;

  bool m_bShowGrid;

  RTSTexture* m_mapTextures;
};
