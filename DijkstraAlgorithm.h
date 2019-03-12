//Ivaylo Ivanov
#pragma once
#include "Controller.h"
class CDijkstraSearch :public ISearch
{
private:
	//Rules to be executed each time
	const int RULES_SIZE = 4;
	//Pointers used to generate path and new states
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

	//Checks to for new state
	bool mGoalNode;
	bool mPushOnList;
	bool mWithInBounds;
	bool mPushStart;
	bool mPathPassed;
	bool mIsOpenListEmpty;
	
	//Used to get map movement cost
	int mMovementValue;
	//Used for mapSize bounds
	int mMapWidth;
	int mMapHeight;
	//Engine controller getter
	I3DEngine* mpMyEngine;
	//Key Codes for engine controller
	EKeyCode mpMultipleIterations = Key_K;
	EKeyCode mpSingleIteration = Key_L;
public:
	CDijkstraSearch();
	bool FindPath(TerrainMap& terrain, SNodePointer start, SNodePointer goal, NodeList& path);
	bool CheckWithinBounds();
	void CheckListsForPending();
	void GenerateNodes(TerrainMap& terrain);
	void SortList(NodeList& unsorted);
	void GetMapSize(int width, int height);
	void NorthRule();
	void EastRule();
	void SouthRule();
	void WestRule();
	void GetMyEngine(I3DEngine* myEngine);
	NodeList PassStatesList(NodeList StatesListOut);
	void CheckListForPathNode();
	void LoadStateList();
	void Reseter();
	bool FailSafeOptions();

};