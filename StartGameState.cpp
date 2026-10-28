#include "StartGameState.h"
#include "EndingState.h"
#include "GameManager.h"

void StartGameState::Init()
{
	map.Init();

	//	プレイヤーのマップスタート地点から
	float sx = 0.0f, sy = 0.0f;
	map.GetStartPos(sx, sy);
	player=Player
	("data/spy.png", static_cast<int>(sx), static_cast<int>(sy));

	// 敵を配置(タイル座標x, タイル座標y, 種類)
	enemies.clear();
	enemies.push_back(Enemy(18, 14, 'h'));  // 横巡回の予定
	enemies.push_back(Enemy(25, 6, 'r'));  // 回転の予定
	enemies.push_back(Enemy(5, 10, 'v'));  // 縦巡回予定

	lastTime = GetNowCount();
}

void StartGameState::Input()
{
	key.Read();	//	キーの読み込み

	//	エンターキーでゲーム画面へ
	if (key.CheckReleaseKey(KEY_INPUT_RETURN)) {

		manager->ChangeState(
			std::make_unique<EndingState>(manager)
		);
	}
}

void StartGameState::Update()
{
	//----------------------------------------------
	//	敵
	const int now = GetNowCount();
	float dt = (now - lastTime) / 1000.0f;
	lastTime = now;
	if (dt > 0.05f) dt = 0.05f;   // 長い停止で瞬間移動しないように

	const bool hidden = player.IsInShadow() && !player.IsMoving();

	for (Enemy& e : enemies) {
		e.Update(map, dt);
		e.CheckPlayer(map, player.GetX(), player.GetY(),hidden);

		// 1体でも見つけたら即ゲームオーバーにする
		if (e.IsAlert()) {
			
			isFound = true;
			break;
		}
	}
	//------------------------------------------------
	//	プレイヤー

	if (isFound) {
		return;
	}

	if (!isGoal) {
		player.Update(map);

		if (player.IsOnGoal(map))
		{
			isGoal = true;
		}
	}
}

void StartGameState::Draw()
{
	map.Draw();
	
	for (const Enemy& e : enemies) {
		e.DrawLight(map);      // ① ライト(床の上、敵の下)
	}

	//	範囲for文
	for (Enemy& e : enemies) {
		e.Draw();
	}
	player.Draw();

	if (isGoal)
	{
		SetFontSize(48);
		DrawString(480, 280, "GOAL!", GetColor(255, 255, 0));
		SetFontSize(16);
	}

	if (isFound) {
		DrawString(480, 280, "GAME OVER!", GetColor(255, 255, 0));
	}

	ScreenFlip();
}





