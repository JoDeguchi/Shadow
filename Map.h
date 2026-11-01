#pragma once
#include "DxLib.h"
#include <vector>

struct Leaf {
	float  x, y;     // 画面上の中心(ピクセル座標)
	double angle;    // 回転角(ラジアン)
};

struct Log {
	float cx, cy;      // 倒木の中心(ピクセル座標)
	float length;      // 長さ(px)
	bool  vertical;    // true なら縦向き
};


class Map
{
private:
	//	マップサイズ
	//	static（静的メンバ変数）で値を変えずに保持
	//	constexprで実行時ではなくコンパイル時に値が求められる
	static constexpr int TILE_SIZE = 32;
	static constexpr int MAP_W = 40;	//	横マスの数
	static constexpr int MAP_H = 20;	//	縦マスの数

	// マップデータ
	char tiles[MAP_H][MAP_W];

	int leaf_hnd = -1;   // 葉っぱ画像のハンドル
	double leaf_scale = 1.0;
	std::vector<Leaf> leaves;

	int kusa_hnd = -1;   // 草むら画像のハンドル

	//	倒木
	int  log_hnd = -1;
	int  log_w = 0, log_h = 0;   // 倒木画像の元サイズ
	std::vector<Log> logs;
	void DrawLogs() const;

public:

	void Init();
	void Draw();
	void DrawLeaves() const;   // プレイヤーより手前に描く

	bool IsWall(float x, float y) const;	// 壁か
	bool IsShadow(float x, float y) const;  // 影か
	bool IsGoal(float x, float y) const;    // ゴールか
	bool GetStartPos(float& x, float& y) const;  // スタート設定 
	char GetTile(float x, float y) const;   // その位置のタイル文字を取得
};