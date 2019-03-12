//Ivaylo Ivanov
#pragma once
#include "Search.h"
#include "Definitions.h"
class CAStarSearch :public ISearch
{
private:
	//Rules to be executed each time
	const int RULES_SIZE = 4;
	//Pointers used to generate new states and create path
	SNodePointer mpCurrentNode;
	SNodePointer mpPendingNode;
	SNodePointer mpPassGoalNode;
	SNodePointer mpTmpPath;
	SNodePointer mpTmpPathIterative;
	SNodePointer mpStateCopy;
	SNode* mpPathCoordinates;
	SNode* mpPathCoordinatesIterative;
	NodeList mpOpenList;
	NodeList mpClosedList;
	NodeList mpStateNodesList;
	NodeList mpIterativePath;
	TerrainMap mpMapCopy;
	
	//Checks for new states 
	bool mGoalNode;
	bool mPushOnList;
	bool mWithInBounds;
	bool mPushStart;
	bool mPathPassed;
	bool mIsOpenListEmpty;
	int mHeuristicValue;
	int mMovementValue;
	int mMapWidth;
	int mMapHeight;
	int mIterationCount;
	//Get Engine Controller
	I3DEngine* mpMyEngine;
	//Key Codes for Engine Contoller
	EKeyCode mpMultipleIterations = Key_K;
	EKeyCode mpSingleIteration = Key_L;
public:
	CAStarSearch();
	void CheckListsForPending();
	bool FindPath(TerrainMap& terrain, SNodePointer start, SNodePointer goal, NodeList& path);
	bool CheckWithinBounds();
	void GenerateNodes(TerrainMap& terrain);
	void SortList(NodeList& unsorted);
	void GetMapSize(int width, int height);
	void NorthRule();
	void EastRule();
	void SouthRule();
	void WestRule();
	void GetMyEngine(I3DEngine* myEngine);
	void LoadStateList();
	NodeList PassStatesList(NodeList StatesListOut);
	void CheckListForPathNode();
	bool FailSafeOptions();
	void Reseter();
};
