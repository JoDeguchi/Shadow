#pragma once
#include "DxLib.h"

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

public:

	void Init();
	void Draw();

	bool IsWall(float x, float y) const;	// 壁か
	bool IsShadow(float x, float y) const;  // 影か
	bool IsGoal(float x, float y) const;    // ゴールか
	bool GetStartPos(float& x, float& y) const;  // スタート設定 
	char GetTile(float x, float y) const;   // その位置のタイル文字を取得
};