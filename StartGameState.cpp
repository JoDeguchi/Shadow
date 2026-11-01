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
	("data/konohamusi.png", static_cast<int>(sx), static_cast<int>(sy));

	//	すべて初期化
	enemies.clear();
	foundEnemy = nullptr;	timer = 0;
	isFound = false;		isGoal = false;
	// 敵を配置(タイル座標x, タイル座標y, 種類)
	enemies.push_back(Enemy(18, 14, 'h'));  // 横巡回の予定
	enemies.push_back(Enemy(25, 6, 'r'));   // 回転の予定
	enemies.push_back(Enemy(5, 10, 'v'));   // 縦巡回予定

	lastTime = GetNowCount();
}

void StartGameState::Input()
{
	key.Read();	//	キーの読み込み
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

		// 見つけた敵は止まって、プレイヤーの方を向くだけ
		if (isFound && &e == foundEnemy) {
			e.LookAt(player.GetX(), player.GetY(), dt);
			continue;
		}

		// それ以外の敵は、発見後も巡回を続ける
		e.Update(map, dt);

		// 発見の判定は、まだ誰も見つけていないときだけ
		if (!isFound) {
			e.CheckPlayer(map, player.GetX(), player.GetY(), hidden);

			if (e.IsAlert()) {
				isFound = true;
				foundEnemy = &e;   // 最初に見つけた敵を覚える
			}
		}
	}

	//------------------------------------------------
	//	プレイヤー処理とエンディング切替

	if (!isGoal && !isFound) {
		player.Update(map);

		if (player.IsOnGoal(map))
		{
			isGoal = true;
		}
	}
	//-----------------------------------------------------------------
	//	エンディング切替
	if (isFound) {
		timer++;
		if (timer > 120) {
			manager->ChangeState(
				std::make_unique<EndingState>(manager, EndingType::Found)
			);
		}
		return;
	}

	if (isGoal) {
		timer++;
		if (timer > 120) {
			manager->ChangeState(
				std::make_unique<EndingState>(manager, EndingType::Goal)
			);
		}
	}

	//	バグ対策。見つかったらゴールは無効、ゴールしたら見つかりは無効
	if (isFound) isGoal = false;  
	if (isGoal) isFound = false;
}


void StartGameState::Draw()
{
	map.Draw();
	
	for (const Enemy& e : enemies) {
		e.DrawLight(map);      // ① ライト(床の上、敵の下)
	}

	map.DrawLeaves();

	player.Draw();

	//	範囲for文
	for (Enemy& e : enemies) {
		e.Draw();
	}
	

	if (isGoal)
	{
		SetFontSize(48);
		DrawString(480, 280, "GOAL!", GetColor(255, 255, 0));
	}

	if (isFound) {
		SetFontSize(48);
		DrawString(450, 280, "見つかってしまった！", GetColor(255, 0, 0));
	}

	ScreenFlip();
}





