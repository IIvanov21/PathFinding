//Ivaylo Ivanov
#pragma once
#include "Controller.h"

class CEnvironmentModels:public CMainController
{
private:
	const int YPOS = 10;//Used with filling Y-position of a model

	//Camera member variables and pointes
	ICamera* mpMyCamera;
	float mCameraX;
	float mCameraY;
	float mCameraZ;
	const int RIGHTANGLE = 90;
	float mCameraSpeed;

	//Floor mesh and model used for background
	IMesh* mpFloorMesh;
	IModel* mpFloor;

	//Dummy models to use as waypoints
	IMesh* mpDummyMesh;
	vector <IModel*> mpDummyTiles;
	vector <float> mDummyPositionsX;
	vector <float> mDummyPositionsY;
	vector <float> mDummyPositionsZ;
	vector <CVector3> mDummyPosition;
	int mDummyCounter;
	int mDummyCounterTwo;
	bool mGenerateSubList;

	//Squre grid models to load the map
	PathTilesList mpSquareTiles;
	IMesh* mpSquareMesh;
	float mSquareTilePositionX;
	float mSquareTilePositionY;
	float mSquareTilePositionZ;
	LookAtVector mSquareTilesPosition;

	//Charcter position variables
	IMesh* mpCharacterMesh;
	IModel* mpCharacterModel;
	float mCharacterMatrixX;
	float mCharacterMatrixY;
	float mCharacterMatrixZ;
	float mCharacterPositionX;
	float mCharacterPositionY;
	float mCharacterPositionZ;
	bool mMoveCharacter;
	float mCharacterSpeed;
	float mGameSpeed;
	CVector3 mCharacterPosition;
	float mCharacterMatrix[16];
	float mCharacterRadius;
	
	//Map information for constructing models
	TerrainMap mpMapInformation;
	int mXStart;
	int mYStart;
	bool mUserPickedMethod;
	int mUserPathChoice;
	VectorList mpSubPointsList;
	CVector3 mpTmpTransferNode;

	//Skins and Meshes string names for loading in program
	string mWhiteBlock;
	string mGreenBlock;
	string mBlueBlock; 
	string mGreyBlock;
	string mPurpleBlock;
	string mMediaFolder;
	string mFloorMeshString;
	string mSquareMeshString;
	string mCharacterModelString;
	string mDummyMeshString;
	string mRedSquare;
	string mStartSquare;
	string mGoalSquare;
public:
	CEnvironmentModels();
	void ResetModels();
	void LoadMeshes();
	void CreateModel();
	
	void LoadPathOnMap();
	void SetUpCharacterMovement();
	void SetUpCameraMovement();
	void characterFacingVector();
	bool pointToSphereCollision(float sphereXPositionOne, float sphereZPositionOne, float sphereRadiusOne, float sphereXPositionTwo, float sphereZPositionTwo);
	void LookAtFunction(CVector3 squareTileTarget, CVector3 characterTarget, IModel*characterModel);
	void UpdateModelPosition();
	void HighLightPath();
	void CreatePathSubPoints();
};
