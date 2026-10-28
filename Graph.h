#pragma once
#include "DxLib.h"
#include <string>

//	画像描画用クラス
class GraphDraw
{
protected:

    int graph_hnd = -1;	                //	画像用変数　初期値未描画
    std::string file_name = "";	        //	ファイル名
    int pos_x = 0; int pos_y = 0;       //  描画位置
    float rot_x = 0; float rot_y = 0;   //  向き

public:
  
    //  コンストラクタ
    GraphDraw() = default;

    GraphDraw(std::string arg_file_name, int x, int y)
    {
        Load_Image(arg_file_name);
        SetPos(x, y);
    }

    //  画像読み込み
    void Load_Image(std::string arg_file_name)
    {
        file_name = arg_file_name;
        graph_hnd = LoadGraph(file_name.c_str());
    }
    // 描画位置の設定
    void SetPos(int x, int y)
    {
        pos_x = x;
        pos_y = y;
    }
    //  位置移動
    void Move(int mvx, int mvy)
    {
        pos_x += mvx;
        pos_y += mvy;
    }
    // 画像の描画
    void Draw()
    {
        if (graph_hnd != -1)
        {
            DrawGraph(pos_x, pos_y, graph_hnd, TRUE);
        }
    }
    // 画像の解放
    void Release()
    {
        if (graph_hnd != -1)
        {
            DeleteGraph(graph_hnd);
            graph_hnd = -1;
        }
    }

};