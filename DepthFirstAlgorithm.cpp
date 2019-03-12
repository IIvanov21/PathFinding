//Ivaylo Ivanov
#include "DepthFirstAlgorithm.h"
/*========================================================Initializing the Dijkstra Search=================================================================*/
//Initializes member variables and booleans
CDepthFirstSearch::CDepthFirstSearch()
{
	mGoalFound=false;
	mWithInBounds = false;
	mDepthLimitCheck=false;
	mNorthLimit=0;
	mEastLimit=0;
	mSouthLimit=0;
	mWestLimit=0;
	mPushStart = true;
	mPathPassed = false;
	mIsOpenListEmpty = false;
}
//Resets checks needed to default for swithcing between Game Statements when program restarts
void CDepthFirstSearch::Reseter()
{
	
	mGoalFound = false;
	mWithInBounds = false;
	mDepthLimitCheck = false;
	mNorthLimit = 0;
	mEastLimit = 0;
	mSouthLimit = 0;
	mWestLimit = 0;
	mPushStart = true;
	mPathPassed = false;
	mIsOpenListEmpty = false;
	
}

//Get map size
void CDepthFirstSearch::GetMapSize(int width, int height)
{
	mMapWidth = width;
	mMapHeight = height;
}

//Get Engine Controller
void CDepthFirstSearch::GetMyEngine(I3DEngine* myEngineIn)
{
	mpMyEngine = myEngineIn;
}
/*========================================================Path Fail return=================================================================*/
//If open list  is empty return fail and restart program
bool CDepthFirstSearch::FailSafeOptions()
{
	return mIsOpenListEmpty;
}

/*========================================================Find Path =================================================================*/
//Generates new states every iteration
bool CDepthFirstSearch::FindPath(TerrainMap& terrain, SNodePointer start, SNodePointer goal, NodeList& path)
{
	//Constructs start node and pushes on the open list
	if (mPushStart)
	{
		mDepthLimit = abs((goal->x - start->x) + (goal->y - start->y));//Calculate depth limit based on heuristic value
		start->parent = 0;
		mpOpenList.push_back(move(start));
		mpMapCopy = terrain;
		mpPassGoalNode.reset(new SNode);
		mpPassGoalNode->x = goal->x;
		mpPassGoalNode->y = goal->y;
		mPushStart = false;
	}
	//If key hit perform a single iteration 
	// or if key held perform multiple iterations
	if (mpMyEngine->KeyHeld(mpMultipleIterations) || mpMyEngine->KeyHit(mpSingleIteration))
	{
		mpStateNodesList.clear();//Clear List with visited nodes on this iteration
		//Check if OpenList is empty before goal is reached
		if (mpOpenList.empty())
		{
			mIsOpenListEmpty = true;
		}

		//While open list is filled pop from front of the list and make it current
		if (!mpOpenList.empty())
		{
			mpCurrentNode.reset(new SNode);
			mpCurrentNode = move(mpOpenList.front());
			mpCurrentNode->nodeVisited = true;
			mpOpenList.pop_front();
			if (mpCurrentNode->x == mpPassGoalNode->x && mpCurrentNode->y == mpPassGoalNode->y)
			{
				mGoalFound = true;

			}
			//Generate new states using rules imposed
			GenerateNodes(terrain);
			//Create and Display path to current including open and closed list
			CheckListForPathNode();
			//When done generating push current to closed list
			mpClosedList.push_back(move(mpCurrentNode));
		}
	}
	
	//If goal found construct path and return to main program
	if (mGoalFound && !mPathPassed)
	{
		mpPathPointer = (mpClosedList.back()).get();
		while (mpPathPointer != 0)
		{
			if (mpPathPointer != NULL)
			{
				mpTmpPointer.reset(new SNode);
				*mpTmpPointer = *mpPathPointer;
				path.push_back(move(mpTmpPointer));
			}
			mpPathPointer = mpPathPointer->parent;

		}
		mPathPassed = true;
		return true;
	}
}
/*========================================================Generating Models=================================================================*/
//Generate new nodes going through each rule in order
void CDepthFirstSearch::GenerateNodes(TerrainMap& terrain)
{
	for (int i = 0; i < RULES_SIZE; i++)
	{
		mpPendingNode.reset(new SNode);
		mDepthLimitCheck = true;
		mOnOpenList = false;
		mOnClosedList = false;
		if (i == North)NorthRule();
		if (i == East)EastRule();
		if (i == South)SouthRule();
		if (i == West)WestRule();
		mWithInBounds = CheckWithinBounds();
		if (mWithInBounds && mDepthLimitCheck)
		{	//Get terrain value
			CheckListsForPending();
			//Check if new state is a wall
			if (terrain[mpPendingNode->y][mpPendingNode->x] != Wall)
			{
				//If new state is not visited and not on open or closed list
				//push new to open list
				if (mpPendingNode->nodeVisited==false && !mOnClosedList && !mOnOpenList)
				{
					mpPendingNode->nodeVisited = true;
					mpPendingNode->parent = mpCurrentNode.get();
					mpOpenList.push_front(move(mpPendingNode));
				}
			}
		}
		//if new state was on closed listor push new state on closed list
		else if (mOnClosedList)mpOpenList.push_back(move(mpPendingNode));
		else if(mOnClosedList) mpClosedList.push_back(move(mpPendingNode));
	}
}

/*========================================================Check List for the New State=================================================================*/
//Checek if pending is on Closed or Open List
//If its on a list ignore new state
void CDepthFirstSearch::CheckListsForPending()
{
	for (auto it = mpClosedList.begin(); it != mpClosedList.end(); it++)
	{
		if ((*it)->x == mpPendingNode->x && (*it)->y == mpPendingNode->y)
		{
			mOnClosedList = true;			
		}
	}
	for (auto it = mpOpenList.begin(); it != mpOpenList.end(); it++)
	{
		if ((*it)->x == mpPendingNode->x && (*it)->y == mpPendingNode->y)
		{
			mOnOpenList = true;
		}
	}
}
/*========================================================Rules=================================================================*/
//Generate new state using the chosen rule if state is not a Wall and within bounds
//Push new state on visited list for visual representation
//Execute rule only if it hasn't reached imposed limit
//Limit set to prevenet it from going one way only
void CDepthFirstSearch::NorthRule()
{
	mpPendingNode->x = mpCurrentNode->x;
	mpPendingNode->y = mpCurrentNode->y + 1;
	mWithInBounds = CheckWithinBounds();
	if (mWithInBounds && mpMapCopy[mpPendingNode->y][mpPendingNode->x] != Wall)LoadStateList();
	mNorthLimit++;
	if (mNorthLimit == mDepthLimit)mDepthLimitCheck = false;
	if (mEastLimit == mDepthLimit)mEastLimit = 0;
}

void CDepthFirstSearch::EastRule()
{
	mpPendingNode->x = mpCurrentNode->x + 1;
	mpPendingNode->y = mpCurrentNode->y;
	mWithInBounds = CheckWithinBounds();
	if (mWithInBounds && mpMapCopy[mpPendingNode->y][mpPendingNode->x] != Wall)LoadStateList();
	mEastLimit++;
	if (mEastLimit == mDepthLimit)mDepthLimitCheck = false;
	if (mSouthLimit == mDepthLimit) mSouthLimit = 0;
}

void CDepthFirstSearch::SouthRule()
{
	mpPendingNode->x = mpCurrentNode->x;
	mpPendingNode->y = mpCurrentNode->y - 1;
	mWithInBounds = CheckWithinBounds();
	if (mWithInBounds && mpMapCopy[mpPendingNode->y][mpPendingNode->x] != Wall)LoadStateList();
	mSouthLimit++;
	if (mSouthLimit == mDepthLimit)mDepthLimitCheck = false;
	if (mWestLimit == mDepthLimit)mWestLimit = 0;
}

void CDepthFirstSearch::WestRule()
{
	mpPendingNode->x = mpCurrentNode->x - 1;
	mpPendingNode->y = mpCurrentNode->y;
	mWithInBounds = CheckWithinBounds();
	if (mWithInBounds && mpMapCopy[mpPendingNode->y][mpPendingNode->x] != Wall)LoadStateList();
	mWestLimit++;
	if (mWestLimit == mDepthLimit)mDepthLimitCheck = false;
	if (mNorthLimit == mDepthLimit)mNorthLimit = 0;
}

bool CDepthFirstSearch::CheckWithinBounds()
{
	if ((mpPendingNode->x >= 0 && mpPendingNode->y >= 0) && (mpPendingNode->x < mMapWidth && mpPendingNode->y < mMapHeight)) return true;
	else return false;
}


/*======================================================== VisitedStateList =================================================================*/
//A list which contains information about visited nodes each iteration

void CDepthFirstSearch::LoadStateList()
{
	mpStateCopy.reset(new SNode);
	mpStateCopy->x = mpPendingNode->x;
	mpStateCopy->y = mpPendingNode->y;
	mpStateNodesList.push_back(move(mpStateCopy));
}
//Return inforamtion to main program
NodeList CDepthFirstSearch::PassStatesList(NodeList StatesListOut)
{
	return move(mpStateNodesList);
}
/*========================================================Tracking=================================================================*/
//It displays the Open and Closed List at end of each iteration in the console
//It also generates a path to the current node and displays it in the console
void CDepthFirstSearch::CheckListForPathNode()
{
	mpIterativePath.clear();
	mpPathCoordinatesIterative = mpCurrentNode.get();
	while (mpPathCoordinatesIterative != 0)
	{
		if (mpPathCoordinatesIterative != NULL)
		{
			mpTmpPathIterative.reset(new SNode);
			*mpTmpPathIterative = *mpPathCoordinatesIterative;
			mpIterativePath.push_back(move(mpTmpPathIterative));
		}
		mpPathCoordinatesIterative = mpPathCoordinatesIterative->parent;
	}
	reverse(mpIterativePath.begin(), mpIterativePath.end());
	//Display information using iterators
	cout << "Current Node: x:" << mpCurrentNode->x << " y:" << mpCurrentNode->y << endl;

	cout << "Path to Current Node:";
	for (auto it = mpIterativePath.begin(); it != mpIterativePath.end(); it++)
	{
		cout << "x:" << (*it)->x << " y:" << (*it)->y << ", ";
	}
	cout << endl;

	cout << "Path to Current Node:";
	for (auto it = mpIterativePath.begin(); it != mpIterativePath.end(); it++)
	{
		cout << "x:" << (*it)->x << " y:" << (*it)->y << ", ";
	}
	cout << endl;
	cout << "Open list:";
	for (auto openIt = mpOpenList.begin(); openIt != mpOpenList.end(); openIt++)
	{
		cout << "x:" << (*openIt)->x << " y:" << (*openIt)->y << ", ";
	}
	cout << endl;
}
