#pragma once
#include "GameState.h"
#include "KeyReader.h"
#include "Map.h"
#include "Player.h"
#include <vector>
#include "Enemy.h"

class StartGameState:public GameState
{
private:
    KeyReader key;
    Map map;
    Player player;
    std::vector<Enemy> enemies; //  vectorで配列（自由に拡張可能）

    int lastTime = 0;

    bool isGoal = false;	//	ゴール地点にいるか

public:

    StartGameState(GameManager* mgr) : GameState(mgr) {}

    void Init() override;
    void Input()override;
    void Update() override;
    void Draw() override;

};

