//Ivaylo Ivanov
#pragma once
#include "Search.h"
#include "SearchFactory.h"
#include "Definitions.h"


class CMainController
{
public:
	const int CHARCOUNT = 48;
	const int ALGORITHMS_SIZE = 3;
	const int ALGORITHM_MIN = 1;

	//Search and map variables and pointers for concrete public access by derived classes
	ISearch* mpPathFinder;
	TerrainMap mMyMap;
	SNodePointer mpStart;
	SNodePointer mpGoal;
	SNodePointer mpStartCopy;
	SNodePointer mpGoalCopy;
	NodeList mpPath;
	fstream mMapFile;
	fstream mMapCoordinates;
	vector <ETerrainCost> mPushBack;
	int mMapHeight;
	int mMapWidth;
	int mPassValue;
	int mAlgorithmChoice;
	string mLineOfValues;
	string mMapName;
	string mMapCoordinatesName;
	ofstream mOutputFile;
	
	string mOutputPath;
	SNodePointer mpPathFromFile;
	NodeList mpConstructedPath;
	NodeList mpStateCheckedNodeList;
	fstream mLoadPathInformation;
	float mFrameTime;

	//States varaible
	EGameStates kGameStates;
	
	//Check for the program be able to swithc between statements
	bool mSuccess;
	bool mPathDisplayed;
	bool mOutPutFileCreated;
	bool mPathLoadedFromFile;
	bool mStateListPassed;
	bool mSearchFailed;
	//Allow derived classes to use Engine controller
	I3DEngine*mpMyEngine;

	CMainController();
	void GetMyEngine(I3DEngine* getEngine);


	void ConstructMapInf();
	void LoadMapInformationFromFile();
	void ConstrustPath();
	void CreateOutputFile();
	void PickAlgorithm();
	void LoadPathInformation();
	void GetFrameTime(float frameTimeIn);
	//Restarts the program 
	void FailReseter();

	//PathFinding and subpoints generation
	VectorList PathSmoothing(VectorList& mainNodes);
	CVector3 CMainController::SubPointBezierCalculation(float tFactor, CVector3 point0, CVector3 point1, CVector3 point2, CVector3 point3);

};