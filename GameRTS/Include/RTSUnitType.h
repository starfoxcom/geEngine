#pragma once

#include <geBox2D.h>
#include "RTSTexture.h"

namespace RTSGame {
  using geEngineSDK::Box2D;

  namespace ANIMATIONS {
    enum E {
      kIDLE = 0,
      kRUN,
      kATTACK,
      kDIE,
      kNUM_ANIMATIONS
    };
  }

  namespace DIRECTIONS {
    enum E {
      kN = 0,
      kNW,
      kW,
      kSW,
      kS,
      kSE,
      kE,
      kNE,
      kNUM_DIRECTIONS
    };
  }

  struct AnimationFrame
  {
    int32 x;
    int32 y;
    int32 w;
    int32 h;
    bool bSwap;
  };

  struct Animation
  {
    uint32 numFrames;
    float duration;
    String name;
    Vector<AnimationFrame> frames[DIRECTIONS::kNUM_DIRECTIONS];
  };

  class RTSUnitType
  {
   public:
     RTSUnitType(uint8 _terrainType);
     ~RTSUnitType();

     static RTSUnitType*
     loadFromFile(uint32 idUnitType);

     

     void
     loadAnimationData(sf::RenderTarget* pTarget, uint32 idUnitType);

   private:
     uint32 m_id;
     String m_name;

     uint8 m_terrainType;


     Vector<Animation> m_animationFrames;
     RTSTexture m_texture;
     sf::RenderTarget* m_pTarget;
  };
}
