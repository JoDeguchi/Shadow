#include "Enemy.h"
#include "Map.h"
#include <cmath>

namespace {
	int sharedGraph = -1;   // 全ての敵で共有する画像ハンドル

	// コマの順番:静止 → 右足前 → 静止 → 左足前 の繰り返し
	constexpr int ANIM_ORDER[4] = { 0, 1, 0, 2 };
	constexpr int ANIM_INTERVAL = 10;   // 何フレームでコマを変えるか
}

// 角度を -π ～ π （度数　ー１８０°～１８０°）の範囲に収める
float NormalizeAngle(float arg_angle)
{
	//	180°より大きい場合
	//	今回１８０°内になるまで何回でも計算するのでループ
	while (arg_angle > 3.14159265)
	{
		arg_angle = arg_angle - (2.0f * 3.14159265f);	//	２πラジアン（360°）
	}
	//	-180°より小さい場合
	while (arg_angle < -3.14159265f)
	{
		arg_angle = arg_angle + (2.0f * 3.14159265f);
	}

	return arg_angle;
}

//	コンストラクタで初期化
Enemy::Enemy(int tileX, int tileY, char type)
	: enemy_type(type)
{
	// 画像は一回だけ読み込むように
	if (sharedGraph == -1) {
		sharedGraph = LoadGraph("data/police.png");  
	}
	graph_hnd = sharedGraph;
	//	ピクセル座標に変換（中心に）
	fx = tileX * TILE_SIZE + TILE_SIZE / 2.0f;
	fy = tileY * TILE_SIZE + TILE_SIZE / 2.0f;

	pos_x = static_cast<int>(fx);
	pos_y = static_cast<int>(fy);

	// 種類ごとの最初の向き
	switch (type)
	{
	case 'h':
		dirX = 1; dirY = 0;     // 右へ出発
		break;
	case 'v':
		dirX = 0; dirY = 1;     // 下へ出発
		break;
	default:
		break;                  // 'r' は動かない
	}

	//	atan2で特定の角度に向ける
	angle = atan2(dirY, dirX);   // 進行方向に向ける
}

void Enemy::Update(const Map& map, float dt)
{
	// 'h' と 'v' だけ巡回する('r' は後で回転を作る)
	if (enemy_type == 'h' || enemy_type == 'v') {
		Patrol(map, dt);
	}
	else if (enemy_type == 'r') {
		Rotate(dt);
	}


	// アニメ:歩いているときだけコマを進める
	if (isMoving) {
		anim_timer++;
		if (anim_timer >= ANIM_INTERVAL) {
			anim_timer = 0;
			anim_step = (anim_step + 1) % 4;
		}
	}
	else {
		anim_timer = 0;
		anim_step = 0;
	}
}

void Enemy::Draw()
{
	// 今のコマの、画像内での左端の位置
	const int frame = ANIM_ORDER[anim_step];
	const int srcX = frame * FRAME_W;

	// 画像は下向きなので、angle(右が0)との差を回転量にする
	const double rot = angle - PI * 0.5f;

	DrawRectRotaGraph(
		pos_x, pos_y,        // 描画位置(画像の中心)
		srcX, 0,             // 切り出し開始位置
		FRAME_W, FRAME_H,    // 切り出しサイズ
		1.0f,				 // 拡大率
		rot,                 // 回転(ラジアン)
		graph_hnd,
		TRUE
	);
}

// 光が壁に当たる位置を調べる
void Enemy::RayEnd(const Map& map, float direction,
	float& endX, float& endY) const
{
	// 光を進める方向
	//	角度０度のときはXは１Yは０
	float moveX = cos(direction);
	float moveY = sin(direction);

	// 敵からどれくらい進んだか
	float distance = 0.0f;

	// 射程の範囲内で調べる
	while (distance < RANGE)
	{
		distance += 4.0f;

		// 光が進んだ先の座標
		float checkX = pos_x + moveX * distance;
		float checkY = pos_y + moveY * distance;

		// 壁に当たったら終了
		if (map.IsWall(checkX, checkY))
		{
			distance -= 4.0f;
			break;
		}
	}

	// 光の終点を決める
	endX = pos_x + moveX * distance;
	endY = pos_y + moveY * distance;
}

void Enemy::CheckPlayer(const Map& map, float px, float py,bool playerHidden)
{
	alert = false;

	// 影の中にいる間は見つからない
	if (playerHidden) return;

	//	敵からプレイヤーまでの距離
	const float dx = px - pos_x;
	const float dy = py - pos_y;
	const float distance = sqrt(dx * dx + dy * dy);
	// 射程の外なら見えない
	if (distance > RANGE) return;

	// 敵からプレイヤーへの角度を調べる
	float playerAngle = atan2(dy, dx);

	// 敵が向いている方向との差を調べる
	float angleDifference = NormalizeAngle(playerAngle - angle);

	// 視界の中にプレイヤーがいるか
	//	左右３５度ずつ
	bool insideView =fabs(angleDifference) <= FOV / 2.0f;

	// 視界の外で、15pxより遠ければ発見できない
	if (insideView == false && distance > 15.0f)
	{
		return;
	}

	// 敵からプレイヤーまでの道に壁がないか調べる
	int checkCount = static_cast<int>(distance / 4.0f) + 1;

	for (int i = 1; i <= checkCount; i++)
	{
		// 敵からプレイヤーに向かって少しずつ進む
		float rate = static_cast<float>(i) / checkCount;

		float checkX = pos_x + dx * rate;
		float checkY = pos_y + dy * rate;

		// 途中に壁があれば発見できない
		if (map.IsWall(checkX, checkY)){
			return;
		}
	}

	alert = true;   // 全部通ったら発見
}

// 敵の視界を描画する
void Enemy::DrawLight(const Map& map) const
{
	unsigned int lightColor;

	// 発見中は赤、通常時は黄色
	if (alert == true)
	{
		lightColor = GetColor(255, 60, 60);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 110);
	}
	else
	{
		lightColor = GetColor(255, 235, 120);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 75);
	}

	// 1つ前の光の終点
	float previousX = 0.0f;
	float previousY = 0.0f;

	// 視界を少しずつ分割して描く
	for (int i = 0; i <= FAN_DIV; i++)
	{
		// 今回調べる光の角度
		float direction =angle - FOV / 2.0f+ FOV * i / FAN_DIV;

		// 今回の光の終点
		float endX;
		float endY;

		RayEnd(map, direction, endX, endY);

		// 2本目以降は三角形を描く
		if (i > 0)
		{
			DrawTriangle(
				static_cast<int>(pos_x),
				static_cast<int>(pos_y),

				static_cast<int>(previousX),
				static_cast<int>(previousY),

				static_cast<int>(endX),
				static_cast<int>(endY),

				lightColor,
				TRUE);
		}

		// 今回の終点を次回のために保存
		previousX = endX;
		previousY = endY;
	}

	// 透明度の設定を元に戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void Enemy::Patrol(const Map& map, float dt)
{
	isMoving = false;

	if (waitTimer > 0) {
		// 待機中:動かない(向きだけ変わる)
		waitTimer -= dt;
	}
	else {
		const float nx = fx + dirX * SPEED * dt;
		const float ny = fy + dirY * SPEED * dt;

		// 進行方向の16px先が壁なら折り返す
		if (map.IsWall(nx + dirX * 16.0f, ny + dirY * 16.0f)) {
			dirX = -dirX;
			dirY = -dirY;
			waitTimer = WAIT_TIME;
		}
		else {
			fx = nx;
			fy = ny;
			isMoving = true;   // 歩いているときだけアニメする
		}
	}

	// 進行方向へ滑らかに向きを変える
	const float target = std::atan2(dirY, dirX);
	const float diff = NormalizeAngle(target - angle);
	const float step = TURN_SPEED * dt;
	if (std::fabs(diff) <= step) {
		angle = target;
	}
	else {
		angle = NormalizeAngle(angle + (diff > 0 ? step : -step));
	}

	// 描画用の整数座標に反映
	pos_x = static_cast<int>(fx);
	pos_y = static_cast<int>(fy);
}

void Enemy::Rotate(float dt)
{
	// 位置は動かさず、向きだけを回す
	angle = NormalizeAngle(angle + ROTATE_SPEED * dt);
	isMoving = false;   // 足踏みはしない(静止コマのまま)
}