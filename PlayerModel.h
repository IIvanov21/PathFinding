#pragma once
#include "Controller.h"

class PlayerModel:public MainController
{
private:
	
	SNodePointer startNodeIn;
	SNodePointer goalNodeIn;
public:
	PlayerModel();
	void LoadMeshes();
	void CreateModel();
	void SetUpMovement();
	void GetMapInformation();
};