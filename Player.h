#pragma once
#include "DxLib.h"
#include "Map.h"
#include "Graph.h"

/// <summary>
/// プレイヤー
/// </summary>
class Player:public GraphDraw
{
private:

	int anim_count = 0;		//	アニメーションのコマ数
	int anim_frame = 0;		//	アニメフレーム
	bool isMoving = false;	//　キーでアニメを動かすためのフラグ
	
	float fx = 0, fy = 0;   // float の座標(pos_x, pos_y は描画用)

	// 体の四隅が壁に当たるか
	bool Collides(const Map& map, float x, float y) const;

	void GetHitBox(float x, float y, float& l, float& t, float& r, float& b) const;

	//	開始地点
	void SetStartPos(float x, float y);

public:

	Player() = default;
	Player(std::string arg_file_name, int x, int y);
	void Update(const Map& map);
	void Draw();
	float GetX() const { return fx; }
	float GetY() const { return fy; }
	// 体の中心がゴールタイルにいるか
	bool IsOnGoal(const Map& map) const;  
	
};