// PathFindingWithVisuals.cpp: A program using the TL-Engine
//Ivaylo Ivanov

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include "Controller.h"
#include "Models.h"

using namespace tle;

void main()
{
	CMainController* gameController = new CMainController();
	CEnvironmentModels* modelCreator = new CEnvironmentModels();

	I3DEngine*myEngine = New3DEngine(kTLX);

	if (modelCreator->kGameStates == StartGame)
	{
		myEngine->StartWindowed();
		modelCreator->kGameStates = ChosingMap;

	}


	float frameTime = myEngine->Timer();

	/**** Set up your scene here ****/


	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();

		/**** Update your scene each frame here ****/
		//Pass frame Time into classes
		frameTime = myEngine->Timer();
		//Load Map Information based on user choice
		if (modelCreator->kGameStates == ChosingMap)
		{
			modelCreator->GetMyEngine(myEngine);
			modelCreator->LoadMapInformationFromFile();
			modelCreator->LoadMeshes();
			modelCreator->CreateModel();
		}
		//Build the mapVisuals and create character movement
		if (modelCreator->kGameStates == BuildingPath)
		{
			modelCreator->GetFrameTime(frameTime);
			modelCreator->ConstrustPath();
			if (!modelCreator->mSuccess)modelCreator->HighLightPath();
			modelCreator->SetUpCameraMovement();
			if (modelCreator->mSuccess)
			{
				if (!modelCreator->mOutPutFileCreated)modelCreator->CreateOutputFile();
				if (!modelCreator->mPathLoadedFromFile)modelCreator->LoadPathInformation();
				modelCreator->LoadPathOnMap();
				modelCreator->CreatePathSubPoints();
				modelCreator->UpdateModelPosition();
				modelCreator->SetUpCharacterMovement();

			}
		}
		//Restart program if search fails
		if (modelCreator->kGameStates == FailSafe)
		{
			modelCreator->ResetModels();
			modelCreator->FailReseter();
			modelCreator->kGameStates = ChosingMap;

		}
		//Close program or restart it
		if (modelCreator->kGameStates == FinalPathState)
		{
			modelCreator->ResetModels();
			modelCreator->FailReseter();
			modelCreator->kGameStates = ChosingMap;
		}
		if (myEngine->KeyHit(Key_Escape))myEngine->Stop();//Close program

	}
	delete modelCreator;

	// Delete the 3D engine now we are finished with it

	myEngine->Delete();
	delete gameController;
}
