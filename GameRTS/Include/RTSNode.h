#pragma once
#include <gePrerequisitesUtil.h>
#include <geVector2I.h>

using namespace geEngineSDK;

class RTSNode
{
public:

  RTSNode(Vector2I _position, int8 _terrainType = 1) :
    m_position(_position), m_terrainType(_terrainType) { m_parent = nullptr; }

  RTSNode(Vector2I _position, int32 _cost, int8 _terrainType = 1) :
    m_position(_position), m_cost(_cost), m_terrainType(_terrainType) { m_parent = nullptr; }

  bool operator==(const RTSNode& _node);

  ~RTSNode();

  Vector2I m_position;
  int32 m_cost;
  int32 m_distance;
  int8 m_terrainType;
  RTSNode* m_parent;
};