//Ivaylo Ivanov
// Definitions.h
// =============
//
// Type definitions to support path finding 
//

#pragma once

#include <vector>
#include <deque>
#include <memory>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <TL-Engine.h>
#include "CVector3cut.h"
#include "CMatrix4x4cut.h"
using namespace tle;

// Represents a node in the search tree.
struct SNode
{
  int x;            // x coordinate
  int y;            // y coordinate
  int score;        // used in more complex algorithms
  int cost;
  bool nodeVisited=false;
  SNode* parent = 0; // note use of raw pointer here
};


// Lists of nodes (e.g Open, Closed and Paths) are implemented as double-ended queues
using NodeList = deque<unique_ptr<SNode>>;
using SNodePointer = unique_ptr<SNode>;
using PathTilesList = vector<vector<IModel*>>;
using LookAtVector = vector<vector<CVector3>>;
using VectorList = deque<CVector3>;
using PathSmoothingNodeList = deque<SNode>;
// Terrain costs for map squares
enum ETerrainCost
{
	Clear = 1,
	Water = 3,
	Wood = 2,
	Wall = 0
};

// Maps of any size are implemented as 2D vectors
using TerrainMap = vector<vector<ETerrainCost>>;

//Rule Generation
enum EGenerationRules
{
	North=0, 
	East=1, 
	South=2, 
	West=3

};
 
enum EGameStates
{
	StartGame,
	ChosingMap,
	BuildingPath,
	FinalPathState,
	FailSafe
};

enum EPathStates
{
	NormalPath=1,
	PathSmoothed=2
};