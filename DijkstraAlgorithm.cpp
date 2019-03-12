//Ivaylo Ivanov
#include "DijkstraAlgorithm.h"
/*========================================================Initializing the Dijkstra Search=================================================================*/
//Initializes member variables and booleans
CDijkstraSearch::CDijkstraSearch()
{
	mGoalNode = false;
	mPushOnList = true;
	mWithInBounds = true;
	mPushStart = true;
	mPathPassed = false;
	mIsOpenListEmpty = false;

}
//Resets checks needed to default for swithcing between Game Statements when program restarts
void CDijkstraSearch::Reseter()
{

	mGoalNode = false;
	mPushOnList = true;
	mWithInBounds = true;
	mPushStart = true;
	mPathPassed = false;
	mIsOpenListEmpty = false;


}
//Get Engine Controller

void CDijkstraSearch::GetMyEngine(I3DEngine* myEngineIn)
{
	mpMyEngine = myEngineIn;
}

//Get the map size from Controller Class
void CDijkstraSearch::GetMapSize(int width, int height)
{
	mMapWidth = width;
	mMapHeight = height;
}
/*========================================================Path Fail return=================================================================*/
//If open list  is empty return fail and restart program
bool CDijkstraSearch::FailSafeOptions()
{
	return mIsOpenListEmpty;
}

/*========================================================Find Path =================================================================*/
//Generates new states every iteration
bool CDijkstraSearch::FindPath(TerrainMap& terrain, SNodePointer start, SNodePointer goal, NodeList& path)
{
	//Constructs start node and pushes on the open list
	if (mPushStart)
	{
		mpPassGoalNode.reset(new SNode);
		start->cost = 0;
		start->parent = 0;
		mpOpenList.push_back(move(start));
		mpPassGoalNode->x = goal->x;
		mpPassGoalNode->y = goal->y;
		mpMapCopy = terrain;
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
			mpOpenList.pop_front();
			//Check if current is goal node and return success if its true
			if (mpCurrentNode->x == mpPassGoalNode->x && mpCurrentNode->y == mpPassGoalNode->y)
			{
				mpPathCoordinates = mpCurrentNode.get();
				mGoalNode = true;
			}
			//Then generate new states using current
			GenerateNodes(terrain);
			//Fill list with visited nodes for visual representation and push current to closed list 
			CheckListForPathNode();
			mpClosedList.push_back(move(mpCurrentNode));
		}
	}
	//If goal reached construct path and return it main program
	if (mGoalNode && !mPathPassed)
	{
		while (mpPathCoordinates != 0)
		{
			if (mpPathCoordinates != NULL)
			{
				mpTmpPath.reset(new SNode);
				*mpTmpPath = *mpPathCoordinates;
				path.push_back(move(mpTmpPath));
			}
			mpPathCoordinates = mpPathCoordinates->parent;

		}
		mPathPassed = true;
		return true;

	}
}
/*========================================================Generating Models=================================================================*/
//Generate new nodes going through each rule in order
void CDijkstraSearch::GenerateNodes(TerrainMap& terrain)
{

	for (int i = 0; i < RULES_SIZE; i++)
	{
		mpPendingNode.reset(new SNode);
		mPushOnList = true;
		if (i == North)NorthRule();
		if (i == East)EastRule();
		if (i == West)WestRule();
		if (i == South)SouthRule();

		mWithInBounds = CheckWithinBounds();

		if (mWithInBounds)
		{	//Get terrain value
			mMovementValue = terrain[mpPendingNode->y][mpPendingNode->x];
			mpPendingNode->cost = mpCurrentNode->cost + mMovementValue; //New cost calculation
			mpPendingNode->parent = mpCurrentNode.get();
			CheckListsForPending();
			if (mPushOnList)//If pushOnList false move to next generation and ignore n
			{
				if (mMovementValue != Wall)//Ignore n if its a Wall
				{
					mpOpenList.push_back(move(mpPendingNode));
				}
			}
		}

	}
	//After generating new nodes from each rule sort OpenList
	if(mPushOnList)SortList(mpOpenList);
	
}
/*========================================================Check List for the New State=================================================================*/
void CDijkstraSearch::CheckListsForPending()
{
	for (auto openIt = mpOpenList.begin(); openIt != mpOpenList.end(); openIt++)//Check if  n->cost is bigger and on open list
	{
		//If eixsiting-n->cost is smaller ignore new n
		if (((*openIt)->x == mpPendingNode->x && (*openIt)->y == mpPendingNode->y))
		{
			if (mpPendingNode->cost >= (*openIt)->cost)mPushOnList = false;
		}
		if (mPushOnList)
		{//If n is on open list with smaller cost pop n from open list 
			if ((*openIt)->x == mpPendingNode->x && (*openIt)->y == mpPendingNode->y)
			{
				mpPendingNode.reset(new SNode);
				mpPendingNode = move(*openIt);
				mpOpenList.erase(openIt);
				break;
			}
		}
	}
	//If n is on closed list ignore it
	for (auto closedIt = mpClosedList.begin(); closedIt != mpClosedList.end(); closedIt++)//Check if n->cost is bigger and on closed list
	{
		if (((*closedIt)->x == mpPendingNode->x && (*closedIt)->y == mpPendingNode->y))
		{
			if (mpPendingNode->cost >= (*closedIt)->cost)mPushOnList = false;
		}
	}

}

/*========================================================Rules=================================================================*/
//Generate new state using the chosen rule if state is not a Wall and within bounds
//Push new state on visited list for visual representation
void CDijkstraSearch::NorthRule()
{
	mpPendingNode->x = mpCurrentNode->x;
	mpPendingNode->y = mpCurrentNode->y + 1;
	mWithInBounds = CheckWithinBounds();
	if (mWithInBounds && mpMapCopy[mpPendingNode->y][mpPendingNode->x] != Wall)LoadStateList();
}

void CDijkstraSearch::EastRule()
{
	mpPendingNode->x = mpCurrentNode->x + 1;
	mpPendingNode->y = mpCurrentNode->y;
	mWithInBounds = CheckWithinBounds();
	if (mWithInBounds && mpMapCopy[mpPendingNode->y][mpPendingNode->x] != Wall)LoadStateList();
}

void CDijkstraSearch::SouthRule()
{
	mpPendingNode->x = mpCurrentNode->x;
	mpPendingNode->y = mpCurrentNode->y - 1;
	mWithInBounds = CheckWithinBounds();
	if (mWithInBounds && mpMapCopy[mpPendingNode->y][mpPendingNode->x] != Wall)LoadStateList();
}

void CDijkstraSearch::WestRule()
{
	mpPendingNode->x = mpCurrentNode->x - 1;
	mpPendingNode->y = mpCurrentNode->y;
	mWithInBounds = CheckWithinBounds();
	if (mWithInBounds && mpMapCopy[mpPendingNode->y][mpPendingNode->x] != Wall)LoadStateList();

}
/*======================================================== VisitedStateList =================================================================*/
//A list which contains information about visited nodes each iteration
bool CDijkstraSearch::CheckWithinBounds()
{
	if ((mpPendingNode->x >= 0 && mpPendingNode->y >= 0) && (mpPendingNode->x < mMapWidth && mpPendingNode->y < mMapHeight)) return true;
	else return false;
}
//Sort open list at the end of each iteration 
//Sort open list based on cost smallest to largest
void CDijkstraSearch::SortList(NodeList& unsortedList)
{
	sort(unsortedList.begin(), unsortedList.end(), [](SNodePointer& lhs,
		SNodePointer& rhs) { return lhs->cost < rhs->cost; });
}
/*======================================================== VisitedStateList =================================================================*/
//A list which contains information about visited nodes each iteration

void CDijkstraSearch::LoadStateList()
{
	mpStateCopy.reset(new SNode);
	mpStateCopy->x = mpPendingNode->x;
	mpStateCopy->y = mpPendingNode->y;
	mpStateNodesList.push_back(move(mpStateCopy));
}
//Returns visited list to main program at the end of each iteration
NodeList CDijkstraSearch::PassStatesList(NodeList StatesListOut)
{
	return move(mpStateNodesList);
}
/*========================================================Tracking=================================================================*/
//It displays the Open List at end of each iteration in the console
//It also generates a path to the current node and displays it in the console
void CDijkstraSearch::CheckListForPathNode()
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
	//reversing map so its show from start to current node in order
	reverse(mpIterativePath.begin(), mpIterativePath.end());

	//Displaying information using iterators
	cout << "Current Node: x:" << mpCurrentNode->x << " y:" << mpCurrentNode->y << endl;

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
