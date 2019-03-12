//Ivaylo Ivanov
#include "Models.h"
/*========================================================Models Constructor=================================================================*/
CEnvironmentModels::CEnvironmentModels()
{
	//Initialize variables at run time
	mCharacterMatrixX = 0.0f;
	mCharacterMatrixY = 0.0f;
	mCharacterMatrixZ = 0.0f;
	mFrameTime = 0.0f;
	mCharacterRadius = 0.3f;
	mDummyCounter = 0;
	mDummyCounterTwo = 0;
	mMoveCharacter = false;
	mGenerateSubList = true;
	mUserPickedMethod = false;
	mUserPathChoice = 0;
	mXStart = 0;
	mYStart = 0;
	mCameraX = 5.0f;
	mCameraY = 25.0f;
	mCameraZ = 5.0f;
	mCameraSpeed = 15.0f;
	mCharacterSpeed = 0.0f;
	mGameSpeed = 1.0f;
	mWhiteBlock = "white.png";
	mGreenBlock = "green.png";
	mBlueBlock = "blue.png";
	mGreyBlock = "grey.png";
	mPurpleBlock = "purple.png";
	mMediaFolder = ".\\Media";
	mFloorMeshString = "floor.x";
	mSquareMeshString = "Square.x";
	mCharacterModelString = "sierra.x";
	mDummyMeshString = "dummy.x";
	mRedSquare = "red.png";
	mStartSquare = "start.png";
	mGoalSquare = "goal.png";
}
/*========================================================Reseting models=================================================================*/
/*Resets all the models infomation stored into the program and sets the member varaibles to 0
prepraing the program to able to load another map information*/
void CEnvironmentModels::ResetModels()
{
	mCharacterMatrixX = 0.0f;
	mCharacterMatrixY = 0.0f;
	mCharacterMatrixZ = 0.0f;
	mFrameTime = 0.0f;
	mCharacterRadius = 0.5f;
	mDummyCounter = 0;
	mDummyCounterTwo = 0;
	mMoveCharacter = false;
	mGenerateSubList = true;
	mUserPickedMethod = false;
	mUserPathChoice = 0;
	mXStart = 0;
	mYStart = 0;

	for (int i = 0; i < mpSquareTiles.size(); i++)
	{
		for (int j = 0; j < mpSquareTiles[i].size(); j++)
		{
			mpSquareMesh->RemoveModel(mpSquareTiles[i][j]);
		}

	}
	for (int i = 0; i < mpDummyTiles.size(); i++)
	{
		mpDummyMesh->RemoveModel(mpDummyTiles[i]);

	}
	mpDummyTiles.clear();
	mDummyPosition.clear();
	mSquareTilesPosition.clear();
	mpSquareTiles.clear();
	mpCharacterMesh->RemoveModel(mpCharacterModel);
	mCharacterPosition = { 0.0f,0.0f,0.0f };
	mpSubPointsList.clear();
}
/*========================================================Load Meshes=================================================================*/
/*Loads all the models meshes that are going to be used for creating the map visuals using the engine controller*/
void CEnvironmentModels::LoadMeshes()
{
	mpMyEngine->AddMediaFolder(mMediaFolder);
	mpFloorMesh = mpMyEngine->LoadMesh(mFloorMeshString);
	mpSquareMesh = mpMyEngine->LoadMesh(mSquareMeshString);
	mpCharacterMesh = mpMyEngine->LoadMesh(mCharacterModelString);
	mpDummyMesh = mpMyEngine->LoadMesh(mDummyMeshString);
	
}

/*========================================================Model Creation=================================================================*/

//It consturcts all the models using the information about he map loaded into MainController
//Also constructs a character model to show path smoothing
void CEnvironmentModels::CreateModel()
{
	mpMyCamera = mpMyEngine->CreateCamera();
	mpFloor = mpFloorMesh->CreateModel();
	mpMyCamera->SetPosition(mCameraX, mCameraY, mCameraZ);
	
	mpMyCamera->RotateX(RIGHTANGLE);

	//Sets vector size to allow correct presentation of the map
	mpSquareTiles.resize(mMapWidth, vector<IModel*>(mMapHeight));
	mSquareTilesPosition.resize(mMapWidth, vector<CVector3>(mMapHeight));

	//Creates square grid to represent the map
	for (int i = 0; i < mMapWidth; i++)
	{
		mXStart += 1;
		mYStart = 0;
		for (int j = 0; j < mMapHeight; j++)
		{
			mYStart += 1;
			mpSquareTiles[i][j] = mpSquareMesh->CreateModel(mXStart, YPOS, mYStart);
			if (mMyMap[j][i] == Wall)mpSquareTiles[i][j]->SetSkin(mGreyBlock);
			if (mMyMap[j][i] == Water)mpSquareTiles[i][j]->SetSkin(mBlueBlock);
			if (mMyMap[j][i] == Wood)mpSquareTiles[i][j]->SetSkin(mGreenBlock);
			if (mMyMap[j][i] == Clear)mpSquareTiles[i][j]->SetSkin(mWhiteBlock);
			
		}

	}
	//Loads all the square coordinates in a vector list for collision detection and loading the path
	for (int i = 0; i < mMapWidth; i++)
	{
		for (int j = 0; j < mMapHeight; j++)
		{
			mSquareTilePositionX = mpSquareTiles[i][j]->GetX();
			mSquareTilePositionY = mpSquareTiles[i][j]->GetY();
			mSquareTilePositionZ = mpSquareTiles[i][j]->GetZ();
			mSquareTilesPosition[i][j] = { mSquareTilePositionX,mSquareTilePositionY,mSquareTilePositionZ };
		}
	}
	//Creates character model for path smoothing
	mpCharacterModel = mpCharacterMesh->CreateModel(mSquareTilesPosition[mpStartCopy->x][mpStartCopy->y].x, mSquareTilesPosition[mpStartCopy->x][mpStartCopy->y].y,
		mSquareTilesPosition[mpStartCopy->x][mpStartCopy->y].z);

	//Changes the state to path building and executing the search algorithms
	kGameStates = BuildingPath;
}

/*========================================================Printing path on map=================================================================*/
//To show the search has worked correctly it high lights all the path nodes into red also indicates start and goal node
void CEnvironmentModels::LoadPathOnMap()
{
	for (auto it = mpConstructedPath.begin(); it != mpConstructedPath.end(); it++)
	{
		mpSquareTiles[(*it)->x][(*it)->y]->SetSkin(mRedSquare);
	}
	mpSquareTiles[mpStartCopy->x][mpStartCopy->y]->SetSkin(mStartSquare);
	mpSquareTiles[mpGoalCopy->x][mpGoalCopy->y]->SetSkin(mGoalSquare);
		

}

/*========================================================RealTime pathfinding visuals=================================================================*/
//On each iteration it retrieves all the nodes that were visited this iteration and highlights them 
//to simulate real time pathfinding
void CEnvironmentModels::HighLightPath()
{
	//All the nodes that were visited stored into StateCheck list which then allows to know which squares need to be highlighted
	//Purple colour used to show visited nodes
	for (auto it = mpStateCheckedNodeList.begin(); it != mpStateCheckedNodeList.end(); it++)
	{
		//As long as node is not out of bound or a Wall highlight it
		if (mMyMap[(*it)->y][(*it)->x] != Wall)
		{
			if ((*it)->y == mMapHeight)
			{
				mpSquareTiles[(*it)->x][(*it)->y - 1]->SetSkin(mPurpleBlock);
			}
			else if ((*it)->x == mMapWidth)
			{
				mpSquareTiles[(*it)->x - 1][(*it)->y]->SetSkin(mPurpleBlock);

			}
			else if ((*it)->x == mMapWidth && (*it)->y == mMapHeight)
			{
				mpSquareTiles[(*it)->x - 1][(*it)->y - 1]->SetSkin(mPurpleBlock);
			}
			else mpSquareTiles[(*it)->x][(*it)->y]->SetSkin(mPurpleBlock);
		}

	}

}
/*========================================================Generate Path Movement=================================================================*/
//Using the path list we pass all the coordinates into subPoints list 
void CEnvironmentModels::CreatePathSubPoints()
{
	if (mGenerateSubList)
	{
		for (auto it = mpConstructedPath.begin(); it != mpConstructedPath.end(); it++)
		{
			mpSubPointsList.push_back({ mSquareTilesPosition[(*it)->x][(*it)->y].x,mSquareTilesPosition[(*it)->x][(*it)->y].z,mSquareTilesPosition[(*it)->x][(*it)->y].y });
		}

		//The List is then used to generate sub points between main path nodes
		mpSubPointsList = PathSmoothing(mpSubPointsList);


		//Using the subpoints coordiantes list create dummy models which the character model will follow from start to goal node
		mpDummyTiles.resize(mpSubPointsList.size());
		mDummyPosition.resize(mpSubPointsList.size());
		for (auto it = mpSubPointsList.begin(); it != mpSubPointsList.end(); it++)
		{
			mpDummyTiles[mDummyCounter] = mpDummyMesh->CreateModel((*it).x, YPOS, (*it).y);
			mDummyPositionsX.push_back({ mpDummyTiles[mDummyCounter]->GetX() });
			mDummyPositionsY.push_back({ mpDummyTiles[mDummyCounter]->GetY() });
			mDummyPositionsZ.push_back({ mpDummyTiles[mDummyCounter]->GetZ() });
			mDummyPosition[mDummyCounter] = { mDummyPositionsX[mDummyCounter],mDummyPositionsY[mDummyCounter],mDummyPositionsZ[mDummyCounter] };
			mDummyCounter++;
		}
		//Makes sure the list is generated only once until program is restarted
		mGenerateSubList = false;
	}
}
/*========================================================Point to Sphere Collision=================================================================*/
//Using the path as list of coordinates for waypoints allows us to set up a character movement
void CEnvironmentModels::SetUpCharacterMovement()
{
	//Allow the use to pick between different moving methods 
	if (!mUserPickedMethod)
	{
		cout << "Pick method for moving across the map: " << endl;
		cout << "1. Normal(No path smoothing)." << endl;
		cout << "2. Path smoothing(not fully working)" << endl;
		cout << "Enter choice:";
		cin >> mUserPathChoice;
		mUserPickedMethod = true;
		mMoveCharacter = true;
	}
	//Check for collision between waypoint and charactermodel while moving towards waypoint
	//Update character speed based on frameTime
	//If collision with dummyPoint changes focus to next dummy 
	//When goal node reached transition to Final State 
	//No path smoothing 
	if (mUserPathChoice == NormalPath)
	{
		

		if (mMoveCharacter)
		{
			
			mCharacterSpeed = mGameSpeed * mFrameTime;
			auto constructedIt = mpConstructedPath.begin();
			bool mConstructedPoint = pointToSphereCollision(mCharacterPositionX, mCharacterPositionZ, mCharacterRadius,
				mpSquareTiles[(*constructedIt)->x][(*constructedIt)->y]->GetX(), mpSquareTiles[(*constructedIt)->x][(*constructedIt)->y]->GetZ());

			LookAtFunction(mSquareTilesPosition[(*constructedIt)->x][(*constructedIt)->y], mCharacterPosition, mpCharacterModel);
			mpCharacterModel->MoveLocalZ(mCharacterSpeed);
			
			if (mConstructedPoint)
			{
				constructedIt++;
				if (constructedIt != mpConstructedPath.end())mpConstructedPath.pop_front();

			}
			if (constructedIt == mpConstructedPath.end())
			{
				mMoveCharacter = false;
				kGameStates = FinalPathState;
			}
		}
	}
	//Path smoothing (not fully working)
	if (mUserPathChoice == PathSmoothed)
	{
		if (mMoveCharacter)
		{
			auto subPointsIt = mpSubPointsList.begin();
			mCharacterSpeed = mGameSpeed * mFrameTime;
			bool mCheckSubPoint = pointToSphereCollision(mCharacterPositionX, mCharacterPositionZ, mCharacterRadius, mpDummyTiles[mDummyCounterTwo]->GetX(), mpDummyTiles[mDummyCounterTwo]->GetZ());
			LookAtFunction(mDummyPosition[mDummyCounterTwo], mCharacterPosition, mpCharacterModel);
			mpCharacterModel->MoveLocalZ(mCharacterSpeed);
			if (mCheckSubPoint)
			{
				subPointsIt++;
				mDummyCounterTwo++;
				if (subPointsIt != mpSubPointsList.end())mpSubPointsList.pop_front();

			}
			if (subPointsIt == mpSubPointsList.end())
			{
				mMoveCharacter = false;
				kGameStates = FinalPathState;
			}
		}
	}
}

/*========================================================Character Position=================================================================*/
//Constatly retireves characters position coordinates to allows us to set up collision for waypoints
void CEnvironmentModels::UpdateModelPosition()
{
	mCharacterPositionX = mpCharacterModel->GetX();
	mCharacterPositionY = mpCharacterModel->GetY();
	mCharacterPositionZ = mpCharacterModel->GetZ();
	mCharacterPosition = { mCharacterPositionX, mCharacterPositionY , mCharacterPositionZ };
}
/*========================================================Character Facing Vector=================================================================*/
void CEnvironmentModels::characterFacingVector()
{
	//It loads the  characters front facing side so the model can face forwards
	mpCharacterModel->GetMatrix(mCharacterMatrix);
	CVector3 characterFacingVector;
	characterFacingVector.Set(&mCharacterMatrix[8]);

	mCharacterMatrixX = mCharacterMatrix[8];
	mCharacterMatrixY = mCharacterMatrix[9];
	mCharacterMatrixZ = mCharacterMatrix[10];
}

/*========================================================Point to Sphere Collision=================================================================*/
bool CEnvironmentModels::pointToSphereCollision(float sphereXPositionOne, float sphereZPositionOne, float sphereRadiusOne, float sphereXPositionTwo, float sphereZPositionTwo)
{
	//Its used to determine if an object has passed through a set of coordinates checking them against the object radius
	float distanceX = sphereXPositionTwo - sphereXPositionOne;
	float distanceZ = sphereZPositionTwo - sphereZPositionOne;
	float distance = sqrt(distanceX * distanceX + distanceZ * distanceZ);

	return(distance < (sphereRadiusOne));
}

/*========================================================Point to Sphere Collision=================================================================*/
//Using the character matrix it allows us to make the character look at different coordinates 
//With LookAtFunction and collision we can set list of dummies as waypoints for the character to follow
void CEnvironmentModels::LookAtFunction(CVector3 squareTileTarget, CVector3 characterTarget, IModel*characterModel)
{

	CVector3 vecZ = Normalise(Subtract(squareTileTarget, characterTarget));
	CVector3 vecX = Normalise(Cross(kYAxis, vecZ));
	CVector3 vecY = Normalise(Cross(vecZ, vecX));

	CMatrix4x4 characterMat;

	characterMat.MakeIdentity();
	characterMat.SetRow(0, vecX);
	characterMat.SetRow(1, vecY);
	characterMat.SetRow(2, vecZ);
	characterMat.SetRow(3, characterTarget);

	characterModel->SetMatrix(&characterMat.e00);

}
/*========================================================Camera Movement=================================================================*/
//Set up camera movement for up, down, left and right movement 
void CEnvironmentModels::SetUpCameraMovement()
{
	if (mpMyEngine->KeyHeld(Key_Up))mpMyCamera->MoveY(mCameraSpeed*mFrameTime);
	if (mpMyEngine->KeyHeld(Key_Down))mpMyCamera->MoveY(-(mCameraSpeed*mFrameTime));
	if (mpMyEngine->KeyHeld(Key_Right))mpMyCamera->MoveX(mCameraSpeed*mFrameTime);
	if (mpMyEngine->KeyHeld(Key_Left))mpMyCamera->MoveX(-(mCameraSpeed*mFrameTime));
}