#pragma once
#include "GameState.h"
#include "KeyReader.h"

class EndingState : public GameState 
{
private:
    KeyReader key;
public:

    EndingState(GameManager* mgr) : GameState(mgr) {}

    void Init() override;
    void Input()override;
    void Update() override;
    void Draw() override;
    
};