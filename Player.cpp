#include "Player.h"

//	バグの原因にもなりやすいため、defineを使わずに設計
namespace {

	constexpr int FRAME_COUNT	= 8;		//	最大８コマ
	//	1コマサイズ
	constexpr int SOURCE_Y		= 45;		//	画像の左上
	constexpr int FRAME_WIDTH	= 75;
	constexpr int FRAME_HEIGHT	= 95;
	// 動きの設定
	constexpr float MOVE_SPEED		= 3.0f;		// 1フレームに進むpx
	constexpr int   ANIM_INTERVAL	= 10;		// 何フレームでコマを変えるか
	constexpr float DRAW_SCALE		= 1.0f;
	// 向き(画像は下向きが0度として回転させる)
	constexpr float ROT_DOWN	= 0.0f;
	constexpr float ROT_LEFT	= 90.0f;
	constexpr float ROT_RIGHT	= 270.0f;
	constexpr float ROT_UP		= 180.0f;

	//	当たり判定(頭の中心からの半径)
	constexpr float HIT_SIDE = 10.0f;   // 左右・後ろ側の半径(今までの BODY_RADIUS)
	constexpr float FOOT_REACH = 25.0f;   // 進行方向に足が出ている分のばす
}

//	コンストラクタ 
Player::Player(std::string arg_file_name, int x, int y)
	:anim_frame(0),anim_count(0)
{
	Load_Image(arg_file_name);
	fx = static_cast<float>(x);
	fy = static_cast<float>(y);
	SetPos(x,y);
	rot_y = ROT_DOWN;	//	ここで初期化しておく
}

//	開始地点
void Player::SetStartPos(float x, float y)
{
	fx = x;
	fy = y;
	SetPos(static_cast<int>(fx), static_cast<int>(fy));
}

//	体を中心としたあたり判定４隅の箱を作る
void Player::GetHitBox(float x, float y, float& l, float& t, float& r, float& b) const
{
	l = x - HIT_SIDE;  r = x + HIT_SIDE;
	t = y - HIT_SIDE;  b = y + HIT_SIDE;

	//	進行方向分判定をさらに前に出す　
	if (rot_y == ROT_DOWN)		b += FOOT_REACH;
	else if (rot_y == ROT_UP)    t -= FOOT_REACH;
	else if (rot_y == ROT_LEFT)  l -= FOOT_REACH;
	else if (rot_y == ROT_RIGHT) r += FOOT_REACH;
}

//	体の４隅が壁に当たるか
bool Player::Collides(const Map& map, float x, float y) const
{
	float l, t, r, b;
	GetHitBox(x, y, l, t, r, b);

	//	４パターンあるのでそれを返す
	return map.IsWall(l, t) || map.IsWall(r, t) ||
		map.IsWall(l, b) || map.IsWall(r, b);
}

//	プレイヤー　アニメーション処理
void Player::Update(const Map& map)
{
	// ---- 移動と向き ----
	int mvx = 0, mvy = 0;
	if (CheckHitKey(KEY_INPUT_DOWN))	{ mvx = 0 ; mvy = 1 ; rot_y = ROT_DOWN ; }
	if (CheckHitKey(KEY_INPUT_LEFT))	{ mvx = -1; mvy = 0 ; rot_y = ROT_LEFT ; }
	if (CheckHitKey(KEY_INPUT_RIGHT))	{ mvx = 1 ; mvy = 0 ; rot_y = ROT_RIGHT; }
	if (CheckHitKey(KEY_INPUT_UP))		{ mvx = 0 ; mvy = -1; rot_y = ROT_UP   ; }

	isMoving = (mvx != 0 || mvy != 0);

	if (isMoving) {

		// X と Y を別々に試す(壁に沿って滑れる)
		const float nx = fx + mvx * MOVE_SPEED;
		if (!Collides(map, nx, fy)) fx = nx;
		const float ny = fy + mvy * MOVE_SPEED;
		if (!Collides(map, fx, ny)) fy = ny;

		SetPos(static_cast<int>(fx), static_cast<int>(fy));
	}

	// ---- 影の判定 ----
	// 足元(中心)のタイルが影かどうか
	inShadow = map.IsShadow(fx, fy);

	// ---- アニメーション ----
	if (isMoving) {
		anim_frame++;
		if (anim_frame >= ANIM_INTERVAL) {
			anim_frame = 0;
			anim_count = (anim_count + 1) % FRAME_COUNT;
		}
	}
	else {
		anim_frame = 0;
		anim_count = 0;
	}
	
}

void Player::Draw()
{
	// 現在のコマの画像位置を計算
	const int src_x = anim_count * FRAME_WIDTH;
	//	ラジアンに変換
	const double angle = rot_y * DX_PI / 180.0;

	// 影の中で止まっているときだけ、半透明で暗くする
	const bool hiding = inShadow && !isMoving;
	if (hiding) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
		SetDrawBright(110, 140, 190);
	}

	// 画像の一部分だけを描画
	DrawRectRotaGraph(
		pos_x, pos_y,            // 画面上の描画位置
		src_x, SOURCE_Y,         // 画像内の切り出し開始位置
		FRAME_WIDTH,             //幅
		FRAME_HEIGHT,            // 高さ
		DRAW_SCALE,
		angle,
		graph_hnd,
		TRUE
	);

	// 必ず元に戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	SetDrawBright(255, 255, 255);
}

//	プレイヤーの中心がゴールにいるか
bool Player::IsOnGoal(const Map& map) const
{
	return map.IsGoal(fx, fy);
}