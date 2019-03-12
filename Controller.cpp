//Ivaylo Ivanov
#include "Controller.h"
#include "Models.h"
CMainController::CMainController()
{
	kGameStates = StartGame;
	mMapHeight = 0;
	mMapWidth = 0;
	mPassValue = 0;
	mPathDisplayed = false;
	mPathLoadedFromFile = false;
	mOutPutFileCreated = false;
	mStateListPassed = true;
	mSuccess = false;
	mSearchFailed = false;
	mOutputPath = "output.txt";
}
//===============Reseter ===================//
/*Reset all the information held by data members */
void CMainController::FailReseter()
{
	//Give a user choice to exit or try another map
	if (kGameStates == FinalPathState)
	{
		int userChoice = 0;
		cout << "Would you like to test another map!" << endl;
		cout << "1.Yes!" << endl;
		cout << "2. No! Exit the program!" << endl;
		cout << "Enter choice 1 or 2:";
		cin >> userChoice;
		if (userChoice == 1)cout << "Restarting program!" << flush;
		if (userChoice == 2)mpMyEngine->Stop();
	}
	if (kGameStates == FailSafe)
	{
		cout << "Path search failed! Going back to Start!" << endl;
	}
	mpConstructedPath.clear();
	mMapHeight = 0;
	mMapWidth = 0;
	mPassValue = 0;
	mPathDisplayed = false;
	mPathLoadedFromFile = false;
	mOutPutFileCreated = false;
	mStateListPassed = true;
	mSuccess = false;
	mSearchFailed = false;
	mpPath.clear();
	mpStateCheckedNodeList.clear();
	mpPathFinder->Reseter();
	kGameStates = StartGame;
}
//Get Engine controller and frameTime from main
void CMainController::GetMyEngine(I3DEngine* getEngine)
{
	mpMyEngine = getEngine;
}

void CMainController::GetFrameTime(float frameTimeIn)
{
	mFrameTime = frameTimeIn;
}
//=============== Load map infomations ===================//
/*Load map infomation from files and load it into the program*/
void CMainController::LoadMapInformationFromFile()
{
	//Let the user choose map and search type
	ConstructMapInf();
	PickAlgorithm();

	//Open mapFile and mapCoordinates information
	mMapCoordinates.open(mMapCoordinatesName);
	mMapFile.open(mMapName);
	if (mMapFile.is_open())
	{
		//Get map size
		mMapFile >> mMapWidth;
		mMapFile >> mMapHeight;
		cout << "Map size:" << mMapWidth << " " << mMapHeight << endl;
		string lineOfValues;
		cout << "Loading map file!" << endl;
		//Split map into individual elements using string and load it into 2D vector 
		while (!mMapFile.eof())
		{
			mMapFile >> lineOfValues;
			for (auto it = lineOfValues.begin(); it != lineOfValues.end(); it++)
			{
				mPassValue = *it - CHARCOUNT;
				if (*it == CHARCOUNT)mPassValue = 0;
				mPushBack.push_back((ETerrainCost)mPassValue);
			}
			if (mPushBack.size() != 0)mMyMap.push_back(mPushBack);
			mPushBack.clear();
			lineOfValues.clear();
		}
		mMapFile.close();
		cout << "Map succesfully loaded!" << endl;
	}
	if (mMyMap.empty())cout << "Map failed to load please restart program!" << endl;
	//Reversing map so we can use the correct coordinate system x(0-9) y(0-9)
	reverse(mMyMap.begin(), mMyMap.end());
	cout << "Printing out map: " << endl;
	for (int i = 0; i < mMapHeight; i++)
	{
		for (int j = 0; j < mMapWidth; j++)
		{
			cout << mMyMap[i][j] << " ";
		}
		cout << endl;
	}
	cout << "Loading start & goal coordinates!" << endl;
	mpStart.reset(new SNode);
	mpGoal.reset(new SNode);
	//Load start and goal coordinates from file for search to use 
	if (mMapCoordinates.is_open())
	{
		while (!mMapCoordinates.eof())
		{
			mMapCoordinates >> mpStart->x;
			mMapCoordinates >> mpStart->y;
			mMapCoordinates >> mpGoal->x;
			mMapCoordinates >> mpGoal->y;

		}
		mMapCoordinates.close();
		cout << "Coordinates loaded successfully!" << endl;
	}
	else cout << "Failed loading coordinates!Please restart program!" << endl;
	mpStartCopy.reset(new SNode);
	mpGoalCopy.reset(new SNode);
	mpStartCopy->x = mpStart->x;
	mpStartCopy->y = mpStart->y;
	mpGoalCopy->x = mpGoal->x;
	mpGoalCopy->y = mpGoal->y;

	//Pass map size information and program controls to search 
	mpPathFinder->GetMapSize(mMapWidth, mMapHeight);
	mpPathFinder->GetMyEngine(mpMyEngine);

	//If search tries to start from a wall restart program
	if (mMyMap[mpStart->y][mpStart->x] == Wall)
	{
		kGameStates = FailSafe;
	}
}
//===============Map Choice ===================//
/*Build string with mapName and mapCoords so it's able to use correct files*/
void CMainController::ConstructMapInf()
{
	cout << "Please input map name: " << endl;
	cin >> mMapName;
	mMapCoordinatesName = mMapName + "Coords.txt";
	mMapName = mMapName + "Map.txt";
}

//===============Path Construction ===================//
/*Uses mSuccess that calls search until the search finds a path or fails */
void CMainController::ConstrustPath()
{
	//Iterate search until result is found
	if (!mSuccess && !mPathDisplayed)
	{
		mSuccess = mpPathFinder->FindPath(mMyMap, move(mpStart), move(mpGoal), mpPath);
		mpStateCheckedNodeList = mpPathFinder->PassStatesList(move(mpStateCheckedNodeList));
		mSearchFailed = mpPathFinder->FailSafeOptions();
	}
	//If result is success display path
	if (mSuccess && !mPathDisplayed)
	{
		//reverse path to be able to show all elements in vector from start to end
		reverse(mpPath.begin(), mpPath.end());
		for (auto it = mpPath.begin(); it != mpPath.end(); it++)
		{
			cout << (*it)->x << " " << (*it)->y << endl;
		}
		if (mpPath.empty())cout << "Fail" << endl;
		else cout << "Success!" << endl;
		mPathDisplayed = true;
	}
	//If fails to find result restart program
	if (mSearchFailed)kGameStates = FailSafe;

}
//===============Algorithm choice ===================//
/*Lets the user choose from the algorithms and creates a new search interface to let
the interface class ISearch which search to use*/
void CMainController::PickAlgorithm()
{
	while (mAlgorithmChoice < ALGORITHM_MIN || mAlgorithmChoice>ALGORITHMS_SIZE)
	{
		cout << "Choose which algorithm you want to generate path: " << endl;
		cout << "1. AStarSearch" << endl;
		cout << "2. DijkstraSearch" << endl;
		cout << "3. DepthFirstSearch" << endl;
		cout << "Please pick a choice from 1 to 3:";
		cin >> mAlgorithmChoice;
	}
	mAlgorithmChoice--;
	//Tells the program which SearchType to use
	mpPathFinder = NewSearch((ESearchType)mAlgorithmChoice);

}
//===============Generate Output File===================//
/*This algorithm creates the output file using ofstream and stores the path information*/
void CMainController::CreateOutputFile()
{
	mOutputFile.open(mOutputPath);//Creates File
	if (!mOutputFile) cout << "Failed to create Output file!" << endl;
	if (mOutputFile.is_open())//Loads path into file
	{
		cout << "Wrtiting path to a output.txt!" << endl;
		for (auto it = mpPath.begin(); it != mpPath.end(); it++)
		{
			mOutputFile << (*it)->x << " " << (*it)->y << endl;
		}
		cout << "Done writing to a file!" << endl;
		mOutputFile.close();//Closes file
	}
	else cout << "Failed to create Output file!" << endl;
	mOutPutFileCreated = true;
}

//===============Load Path from file===================//
/*This algorithms simply opens the output file and reads the coordinates of the path into mpConstructed Path*/
void CMainController::LoadPathInformation()
{
	mLoadPathInformation.open(mOutputPath);
	if (!mLoadPathInformation) cout << "Failed to open Output file!" << endl;
	string lineOfValues;
	if (mLoadPathInformation.is_open())
	{
		cout << "Loading path information!" << endl;
		while (!mLoadPathInformation.eof())
		{
			mpPathFromFile.reset(new SNode);
			mLoadPathInformation >> mpPathFromFile->x;
			mLoadPathInformation >> mpPathFromFile->y;
			mpConstructedPath.push_back(move(mpPathFromFile));

		}
		mpConstructedPath.pop_back();
		mLoadPathInformation.close();

	}
	mPathLoadedFromFile = true;
}

//Bezier curve calculation allows me to generate dummy points between main path points 
//Source for formula adaptation: https://en.wikipedia.org/wiki/B%C3%A9zier_curve
//Formula used for calculatetion: B(t)=(1-t)3 *P0 + 3(1-t) t2*P1+3(1-t)t2*P2+t3*P3
VectorList CMainController::PathSmoothing(VectorList& mainNodesList)
{
	VectorList generatePathCurve;
	int subPointCount = 150;
	int n = 3;
	for (auto i = 0; i < mainNodesList.size() - n; i += n)
	{
		int regulatorPoint = i;
		int regulatorPointTwo = i + (n - 2);
		int regulatorPointThree = i + (n - 1);
		int regulatorPointFour = i + n;
		int paddingZ = 0;

		if (regulatorPointTwo >= mainNodesList.size())regulatorPointTwo = regulatorPoint;
		if (regulatorPointThree >= mainNodesList.size())regulatorPointThree = regulatorPointTwo;
		if (regulatorPointFour >= mainNodesList.size())regulatorPointFour = regulatorPointThree;

		CVector3 pointOne = { mainNodesList[regulatorPoint].x,mainNodesList[regulatorPoint].y, mainNodesList[paddingZ].z};
		CVector3 pointTwo = { mainNodesList[regulatorPointTwo].x,mainNodesList[regulatorPointTwo].y,mainNodesList[paddingZ].z };
		CVector3 pointThree = { mainNodesList[regulatorPointThree].x,mainNodesList[regulatorPointThree].y,mainNodesList[paddingZ].z };
		CVector3 pointFour = { mainNodesList[regulatorPoint].x,mainNodesList[regulatorPoint].y,mainNodesList[paddingZ].z };

		if (i == 0)
		{
			generatePathCurve.push_back(SubPointBezierCalculation(0, pointOne, pointTwo, pointThree, pointFour));
		}

		for (auto j = 1; j < subPointCount; j++)
		{
			float point = 0;
			point = (float)j / subPointCount;
			generatePathCurve.push_back(SubPointBezierCalculation(point, pointOne, pointTwo, pointThree, pointFour));
		}
		i += n;
	}

	return generatePathCurve;
}
CVector3 CMainController::SubPointBezierCalculation(float tFactor, CVector3 point0, CVector3 point1, CVector3 point2 ,CVector3 point3)
{
	float n = 1 - tFactor;
	float multiplier = 3.0f;
	float tTwo = tFactor * tFactor;
	float nTwo = n * n;
	float nThree = nTwo * n;
	float tThree = tTwo * tFactor;

	//Bezier cruve formula split into 4 parts
	CVector3 dummyPoints;
	//X-cooridnates calculation
	dummyPoints.x = nThree * point0.x;
	dummyPoints.x += multiplier * tFactor * nTwo * point1.x;
	dummyPoints.x += multiplier * tTwo * n *  point2.x;
	dummyPoints.x += tThree * point3.x;

	//Y-coordinates calculation
	dummyPoints.y = nThree * point0.y;
	dummyPoints.y += multiplier * tFactor * nTwo *  point1.y;
	dummyPoints.y += multiplier * tTwo * n *  point2.y;
	dummyPoints.y += tThree * point3.y;
	
	return dummyPoints;
}
