#include "StartGameState.h"
#include "EndingState.h"
#include "GameManager.h"

void StartGameState::Init()
{
	map.Init(stage);

	//	プレイヤーのマップスタート地点から
	float sx = 0.0f, sy = 0.0f;
	map.GetStartPos(sx, sy);
	player=Player
	("data/konohamusi.png", static_cast<int>(sx), static_cast<int>(sy));

	enemies.clear();
	foundEnemy = nullptr;	timer = 0;
	isFound = false;		isGoal = false;


	if (stage == 1) {
		enemies.push_back(Enemy(18, 14, 'h'));
		enemies.push_back(Enemy(25, 6, 'r'));
		enemies.push_back(Enemy(5, 10, 'v'));
	}
	else {
		// 2面の敵(仮。マップに合わせて変更してください)
		enemies.push_back(Enemy(10, 5, 'h'));
		enemies.push_back(Enemy(25, 12, 'v'));
		enemies.push_back(Enemy(30, 4, 'r'));
	}

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
		if (timer > 100) {
			manager->ChangeState(
				std::make_unique<EndingState>(manager, EndingType::Found)
			);
		}
		return;
	}

	if (isGoal) {
		timer++;
		if (timer > 100) {
			if (stage == 1) {
				// 1面クリア → 2面へ
				manager->ChangeState(
					std::make_unique<StartGameState>(manager, 2)
				);
			}
			else {
				// 2面クリア → エンディングへ
				manager->ChangeState(
					std::make_unique<EndingState>(manager, EndingType::Goal)
				);
			}
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
	
	// 発見マークは敵より手前に
	for (const Enemy& e : enemies) {
		e.DrawAlertMark();
	}


	if (isGoal)
	{
		SetFontSize(48);
		if (stage == 1) DrawString(400, 280, "STAGE1 CLEAR!", GetColor(255, 255, 0));
		else            DrawString(480, 280, "ALL CLEAR", GetColor(255, 255, 0));
	}

	if (isFound) {
		SetFontSize(48);
		DrawString(450, 280, "見つかってしまった！", GetColor(255, 0, 0));
	}

	ScreenFlip();
}





