#pragma once

#include "GameState.h"
#include "KeyReader.h"
#include "Graph.h"

class TitleState :public GameState
{
private:

	KeyReader key;

	GraphDraw rogo;
	GraphDraw bg;
	GraphDraw SpaceStart;

public:

	TitleState(GameManager* mgr) : GameState(mgr) {}

	void Init() override;
	void Input() override;
	void Update()override;
	void Draw()override;

};