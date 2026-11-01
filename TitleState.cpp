#include "TitleState.h"
#include "GameManager.h"
#include "StartGameState.h"


void TitleState::Init()
{
	bg = GraphDraw("data/background.jpg", 0, 0);
	//	光の粒を初期化
	for (int i = 0; i < LIGHT_COUNT; i++)
	{
		light[i].x = GetRand(1200);
		light[i].y = GetRand(600);
		light[i].speed = 0.5f + GetRand(20) / 10.0f;
		light[i].size = 1;
	}
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
	//	光の粒を動かす
	for (int i = 0; i < LIGHT_COUNT; i++)
	{
		light[i].y += light[i].speed;

		if (light[i].y > 650)
		{
			light[i].y = -10;
			light[i].x = GetRand(1200);
		}
	}
}

void TitleState::Draw()
{
	bg.Draw();			//	背景
	
	//	光の粒を描画
	for (int i = 0; i < LIGHT_COUNT; i++)
	{
		DrawCircle(
			(int)light[i].x,
			(int)light[i].y,
			light[i].size,
			GetColor(255, 255, 180),
			TRUE
		);
	}

}

