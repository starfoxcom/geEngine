#pragma once
#include <gePrerequisitesUtil.h>
#include <geVector2I.h>

using namespace geEngineSDK;

class RTSNode
{
public:

  RTSNode(Vector2I _position, int8 _terrainType) :
    m_position(_position), m_terrainType(_terrainType) 
  { 
    m_parent = nullptr; 
  }

  RTSNode(Vector2I _position, int8 _cost, int32 _totalCost, int8 _terrainType) :
    m_position(_position), m_cost(_cost), m_totalCost(_totalCost), m_terrainType(_terrainType)
  { 
    m_parent = nullptr; 
  }

  RTSNode(Vector2I _position, Vector2I _distance, int8 _terrainType) :
    m_position(_position), m_distance(_distance), m_terrainType(_terrainType) 
  {
    m_parent = nullptr; 
  }

  RTSNode(Vector2I _position, Vector2I _distance, int8 _cost, int32 _totalCost, int8 _terrainType) :
    m_position(_position), m_distance(_distance), m_cost(_cost), m_totalCost(_totalCost), 
    m_terrainType(_terrainType)
  {
    m_parent = nullptr;
  }

  bool operator==(const RTSNode& _node);

  ~RTSNode();

  Vector2I m_position;
  int8 m_cost;
  int32 m_totalCost;
  Vector2I m_distance;
  int8 m_terrainType;
  RTSNode* m_parent;
};