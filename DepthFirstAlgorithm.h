//Ivaylo Ivanov
#pragma once
#include "Controller.h"

class CDepthFirstSearch:public ISearch
{
private:
	//Rules to be executed each iteration
	const int RULES_SIZE = 4;
	//Nodes used to create path and new states
	NodeList mpOpenList;
	NodeList mpClosedList;
	NodeList mpStateNodesList;
	NodeList mpIterativePath;
	TerrainMap mpMapCopy;
	SNodePointer mpCurrentNode;
	SNodePointer mpPendingNode;
	SNodePointer mpTmpPointer;
	SNodePointer mpPassGoalNode;
	SNodePointer mpTmpPath;
	SNodePointer mpTmpPathIterative;
	SNodePointer mpStateCopy;
	SNode* mpPathCoordinatesIterative;
	SNode* mpPathPointer;
	SNode* mpPriorNode;
	//Checks for new state and path creation
	bool mGoalFound;
	bool mOnClosedList;
	bool mOnOpenList;
	int mNorthLimit;
	int mEastLimit;
	int mSouthLimit;
	int mWestLimit;
	int mMapWidth;
	int mMapHeight;
	int mDepthLimit;
	bool mDepthLimitCheck;
	bool mWithInBounds;
	bool mPushStart;
	bool mPathPassed;
	bool mIsOpenListEmpty;

	//Get Engine controller
	I3DEngine* mpMyEngine;
	//Key Codes for Engine controller
	EKeyCode mpMultipleIterations = Key_K;
	EKeyCode mpSingleIteration = Key_L;
public:
	CDepthFirstSearch();
	void GenerateNodes(TerrainMap& terrain);
	void GetMapSize(int width, int height);
	bool CheckWithinBounds();
	void CheckListsForPending();
	bool FindPath(TerrainMap& terrain, SNodePointer start, SNodePointer goal, NodeList& path);
	void NorthRule() ;
	void EastRule() ;
	void SouthRule();
	void WestRule();
	void GetMyEngine(I3DEngine* myEngine);
	NodeList PassStatesList(NodeList StatesListOut);
	void CheckListForPathNode();
	void LoadStateList();
	void Reseter();
	bool FailSafeOptions();

};