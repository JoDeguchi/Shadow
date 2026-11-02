#pragma once
#include "DxLib.h"
#include <vector>

//	マップクラス
class Map
{
private:

public:

	void Init(int stage);
	void Draw();
	void DrawLeaves() const;	// 葉の描画
	void DrawLogs() const;		// 倒木の描画

	bool IsWall(float x, float y) const;		// 壁か
	bool IsShadow(float x, float y) const;		// 影か
	bool IsGoal(float x, float y) const;		// ゴールか
	bool GetStartPos(float& x, float& y) const; // スタート設定 
	char GetTile(float x, float y) const;		// その位置のタイル文字を取得
};