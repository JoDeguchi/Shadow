#pragma once
#include "Graph.h"
#include "DxLib.h"

class Map;	//	前方宣言

class Enemy:public GraphDraw
{
private:
	static constexpr int TILE_SIZE = 32;		//	タイルの大きさ
	static constexpr float PI = 3.14159265f;	//	円周率	

	float angle = 0;        // 敵が向いている方向
	char  enemy_type = 'h'; // 'h':横 / 'v':縦 / 'r':回転(敵の状態)

	//------------------------------------------------------------
	// ライト関連
	// ライト(視界)の設定
	static constexpr float FOV = 70.0f * PI / 180.0f;  // 視野角（ラジアン変換）
	static constexpr float RANGE = 150.0f;             // 届く距離
	static constexpr int   FAN_DIV = 30;               // 扇形の分割数

	//	指定した方向に光を伸ばし、終点を調べる
	void RayEnd(const Map& map,float direction,float& endX,float& endY) const;

	bool alert = false;	//	プレイヤーを見つけたか

	//--------------------------------------------------------------
	// アニメーション関連
	// 画像の設定
	static constexpr int   FRAME_W = 100;		// 1コマの幅(300÷3)
	static constexpr int   FRAME_H = 80;		// 1コマの高さ

	// アニメーション(巡回を作るときに使う)
	bool isMoving = false;   // 動いているか
	int  anim_timer = 0;     // フレームを数える
	int  anim_step = 0;      

	// 巡回用
	static constexpr float SPEED = 55.0f;       // 移動速度(px/秒)
	static constexpr float TURN_SPEED = 5.0f;   // 振り向く速さ(rad/秒)
	static constexpr float WAIT_TIME = 0.8f;    // 折り返しの待機時間(秒)

	float fx = 0, fy = 0;		// float の座標(pos_x, pos_y は描画用に残す)
	float dirX = 0, dirY = 0;	// 進む方向
	float waitTimer = 0;		// 待機の残り時間
	//	回転用
	static constexpr float ROTATE_SPEED = 0.7f;   // 回転の速さ(rad/秒)

public:
	Enemy() {}

	// タイル座標に配置
	Enemy(int tileX, int tileY, char type);

	void Update(const Map& map, float dt);   
	void Draw();
	void DrawLight(const Map& map) const;   

	// プレイヤーがライトに入っているか判定して alert を更新する
	void CheckPlayer(const Map& map, float px, float py);
	// 巡回の処理
	void Patrol(const Map& map, float dt);   
	//	敵の回転
	void Rotate(float dt);
};