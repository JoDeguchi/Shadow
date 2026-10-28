#include "TitleState.h"
#include "GameManager.h"
#include "StartGameState.h"

void TitleState::Init()
{
	bg = GraphDraw("data/bg2.png", 0, 0);
	rogo = GraphDraw("data/rogo.png", 230, 20);
	SpaceStart = GraphDraw("data/SpaceStart.png", 380, 400);
}

void TitleState::Input()
{
	key.Read();	//	キーの読み込み

	//	エンターキーでゲーム画面へ
	if (key.CheckReleaseKey(KEY_INPUT_SPACE)){

		manager->ChangeState(
			std::make_unique<StartGameState>(manager)
		);
    }
}

void TitleState::Update()
{
	
}

void TitleState::Draw()
{
	bg.Draw();			//	背景
	rogo.Draw();		//	ロゴ
	SpaceStart.Draw();	//	スペーススタートボタン
}

