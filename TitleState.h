#pragma once

#include "GameState.h"
#include "KeyReader.h"
#include "Graph.h"

#define LIGHT_COUNT 10

struct LightParticle
{
	float x, y;
	float speed;
	int size;
};


class TitleState :public GameState
{
private:

	KeyReader key;

	GraphDraw bg;

	LightParticle light[LIGHT_COUNT];
	
public:

	TitleState(GameManager* mgr) : GameState(mgr) {}

	void Init() override;
	void Input() override;
	void Update()override;
	void Draw()override;

};