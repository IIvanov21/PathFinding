//Ivaylo Ivanov
#include "AStarAlgorithm.h"
/*========================================================Initializing the A* Search=================================================================*/
//Initializes member variables and booleans
CAStarSearch::CAStarSearch()
{
	mGoalNode = false;
	mPushOnList = true;
	mWithInBounds = true;
	mPushStart = true;
	mPathPassed = false;
	mIsOpenListEmpty = false;
	mIterationCount = 0;
}
//Resets checks needed to default for swithcing between Game Statements when program restarts
void CAStarSearch::Reseter()
{
	mGoalNode = false;
	mPushOnList = true;
	mWithInBounds = true;
	mPushStart = true;
	mPathPassed = false;
	mIsOpenListEmpty = false;
	mIterationCount = 0;
}
//Get Engine Controller
void CAStarSearch::GetMyEngine(I3DEngine* myEngineIn)
{
	mpMyEngine = myEngineIn;
}
//Get the map size from Controller Class
void CAStarSearch::GetMapSize(int width, int height)
{
	mMapWidth = width;
	mMapHeight = height;
}
/*========================================================Find Path =================================================================*/
//Generates new states every iteration
bool CAStarSearch::FindPath(TerrainMap& terrain, SNodePointer start, SNodePointer goal, NodeList& path)
{
	//Constructs start node and pushes on the open list
	if (mPushStart)
	{
		mpMapCopy = terrain;
		mpPassGoalNode.reset(new SNode);
		start->cost = 0;
		start->score = 0;
		mHeuristicValue = (goal->x - start->x) + (goal->y - start->y);
		start->score = mHeuristicValue;
		mpOpenList.push_back(move(start));
		mpPassGoalNode->x = goal->x;
		mpPassGoalNode->y = goal->y;
		mPushStart = false;
	}
	
	if (!mGoalNode)
	{
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
				if (mpCurrentNode->x == mpPassGoalNode->x 
					&& mpCurrentNode->y == mpPassGoalNode->y)mGoalNode = true; 
				//Then generate new states using current
				GenerateNodes(terrain);
				//Fill list with visited nodes for visual representation and push current to closed list 
				CheckListForPathNode();
				mpClosedList.push_back(move(mpCurrentNode));
			}
			//If goal reached construct path and return it main program
			if (mGoalNode && !mPathPassed)
			{
				cout << "Iteration count: " << mIterationCount << endl;
				mpPathCoordinates = (mpClosedList.back()).get();
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
				return mPathPassed;

			}
		}
	}

	
}
/*========================================================Generating Models=================================================================*/

void CAStarSearch::GenerateNodes(TerrainMap& terrain)
{
	//Generate new nodes going through each rule in order
	for (int i = 0; i < RULES_SIZE; i++)
	{
		mpPendingNode.reset(new SNode);
		mPushOnList = true;
		if (i == North)NorthRule();
		if (i == East)EastRule();
		if (i == West)WestRule();
		if (i == South)SouthRule();


		if (mWithInBounds)
		{
			//Get terrain value
			mMovementValue = terrain[mpPendingNode->y][mpPendingNode->x];
			mpPendingNode->cost = mpCurrentNode->cost + mMovementValue; //New cost calculation
			CheckListsForPending();
			if (mPushOnList)//If pushOnList false move to next generation and ignore n
			{	
				if (mMovementValue != Wall)//Ignore n if its a Wall
				{
					//Calculate heuristicValue and Score value of the node
					mHeuristicValue = abs((mpPassGoalNode->x - mpPendingNode->x) + (mpPassGoalNode->y - mpPendingNode->y));
					mpPendingNode->score = mpPendingNode->cost + mHeuristicValue;
					mpPendingNode->nodeVisited = true;
					mpPendingNode->parent = mpCurrentNode.get();
					mpOpenList.push_back(move(mpPendingNode));
				}
				
			}
			
		}
			
	}
	
	if (mPushOnList)//After generating new nodes from each rule sort OpenList
	{
		SortList(mpOpenList);
		mIterationCount++;
	}
	
}
/*========================================================Check List for the New State=================================================================*/
void CAStarSearch::CheckListsForPending()
{
	//Check if  n->cost is bigger and on open list
	for (auto openIt = mpOpenList.begin(); openIt != mpOpenList.end(); openIt++)
	{
		//If n is on open list with smaller cost ignore n
		if (((*openIt)->x == mpPendingNode->x && (*openIt)->y == mpPendingNode->y))
		{
			if (mpPendingNode->cost >= (*openIt)->cost)mPushOnList = false;
		}
		//If n is on open list with bigger cost pop existing_n of list and make it current n
		if (mPushOnList)
		{
			if ((*openIt)->x == mpPendingNode->x && (*openIt)->y == mpPendingNode->y)
			{
				mpPendingNode.reset(new SNode);
				mpPendingNode = move(*openIt);
				mpOpenList.erase(openIt);
				break;
			}
		}
	}
	for (auto closedIt = mpClosedList.begin(); closedIt != mpClosedList.end(); closedIt++)//Check if n->cost is bigger and on closed list
	{
		//If n is on open list with smaller cost ignore n
		if (((*closedIt)->x == mpPendingNode->x && (*closedIt)->y == mpPendingNode->y))
		{
			if (mpPendingNode->cost >= (*closedIt)->cost)mPushOnList = false;
		}
		if (mPushOnList)
		{
			//If n is on closed list with bigger cost pop existing_n of list and make it current n
			if ((*closedIt)->x == mpPendingNode->x && (*closedIt)->y == mpPendingNode->y)
			{
				mpPendingNode.reset(new SNode);
				mpPendingNode = move(*closedIt);
				mpClosedList.erase(closedIt);
				break;
			}
		}
	}
}
/*========================================================Path Fail return=================================================================*/
//If open list  is empty return fail and restart program
bool CAStarSearch::FailSafeOptions()
{
	return mIsOpenListEmpty;
}
/*========================================================Rules=================================================================*/
//Generate new state using the chosen rule if state is not a Wall and within bounds
//Push new state on visited list for visual representation

void CAStarSearch::NorthRule()
{
	mpPendingNode->x = mpCurrentNode->x;
	mpPendingNode->y = mpCurrentNode->y + 1;
	mWithInBounds = CheckWithinBounds();
	if(mWithInBounds && mpMapCopy[mpPendingNode->y][mpPendingNode->x]!=Wall)LoadStateList();
}

void CAStarSearch::EastRule()
{
	mpPendingNode->x = mpCurrentNode->x + 1;
	mpPendingNode->y = mpCurrentNode->y;
	mWithInBounds = CheckWithinBounds();
	if (mWithInBounds && mpMapCopy[mpPendingNode->y][mpPendingNode->x] != Wall)LoadStateList();
}

void CAStarSearch::SouthRule()
{
	mpPendingNode->x = mpCurrentNode->x;
	mpPendingNode->y = mpCurrentNode->y - 1;
	mWithInBounds = CheckWithinBounds();
	if (mWithInBounds && mpMapCopy[mpPendingNode->y][mpPendingNode->x] != Wall)LoadStateList();
}

void CAStarSearch::WestRule()
{
	mpPendingNode->x = mpCurrentNode->x - 1;
	mpPendingNode->y = mpCurrentNode->y;
	mWithInBounds = CheckWithinBounds();
	if (mWithInBounds && mpMapCopy[mpPendingNode->y][mpPendingNode->x] != Wall)LoadStateList();

}

/*======================================================== VisitedStateList =================================================================*/
//A list which contains information about visited nodes each iteration
void CAStarSearch::LoadStateList()
{
	mpStateCopy.reset(new SNode);
	mpStateCopy->x = mpPendingNode->x;
	mpStateCopy->y = mpPendingNode->y;
	mpStateNodesList.push_back(move(mpStateCopy));
}
//Returns the visited list back to the main program every iteration 
NodeList CAStarSearch::PassStatesList(NodeList StatesListOut)
{
	return move(mpStateNodesList);
}

/*========================================================Check node within bounds and Sort List=================================================================*/
//Checks if new state is withing the bounds and based on that tells the algorithms to ignore or proceed with the new state
bool CAStarSearch::CheckWithinBounds()
{
	if((mpPendingNode->x>=0 && mpPendingNode->y>=0) &&
	(mpPendingNode->x<mMapWidth && mpPendingNode->y<mMapHeight)) return true;
	else return false;
}
//At the end of each iteration sorts the open list
//Sorts the open list based on score from smallest to biggest
void CAStarSearch::SortList(NodeList& unsortedList)
{
	sort(unsortedList.begin(), unsortedList.end(), [](SNodePointer& lhs,
		SNodePointer& rhs) { return lhs->score < rhs->score; });
}
/*========================================================Tracking=================================================================*/
//It displays the Open and Closed List at end of each iteration in the console
//It also generates a path to the current node and displays it in the console
void CAStarSearch::CheckListForPathNode()
{
	//Generating the path to current node
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

	//Displaying the information using iterators
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
	cout << "Closed list:";
	for (auto closedIt = mpClosedList.begin(); closedIt != mpClosedList.end(); closedIt++)
	{
		cout << "x:" << (*closedIt)->x << " y:" << (*closedIt)->y << ", ";
	}
	cout << endl;
}
