#include "Map.h"
#include <cmath>
#include <string>
#include <cstdio>

// 葉っぱ
struct Leaf {
    float  x, y;     // 画面上の中心(ピクセル座標)
    double angle;    // 回転角(ラジアン)
};
//	倒木
struct Log {
    float cx, cy;      // 倒木の中心(ピクセル座標)
    float length;      // 長さ(px)
    bool  vertical;    // true なら縦向き
};

//  constexprで意図しない値変換バグを防止
namespace {

    //  マップサイズ
    constexpr int TILE_SIZE = 32;
    constexpr int MAP_W = 40;	//	横マスの数
    constexpr int MAP_H = 20;	//	縦マスの数

    // マップデータ
    char tiles[MAP_H][MAP_W];

    //----------------------------------------------------
    //  葉っぱ
    int leaf_hnd = -1;                          // 葉っぱ画像のハンドル
    double leaf_scale = 1.0;
	std::vector<Leaf> leaves;                   // 葉っぱの量 vector
    constexpr float LEAF_SIZE = 48.0f;       
    constexpr int   LEAVES_PER_TILE = 3;        // 1タイルあたりの枚数
	//---------------------------------------------------
    //  草むら
    int kusa_hnd = -1;                          // 草むら画像のハンドル
    constexpr int   KUSA_MARGIN = 8;            // 草むらをはみださせるピクセル量
	//---------------------------------------------------
    //  倒木
    int  log_hnd = -1;
    int  log_w = 0, log_h = 0;                  // 倒木画像の元サイズ
    std::vector<Log> logs;
    constexpr float LOG_THICKNESS = 56.0f;      // 倒木の太さ(px)

    //int kirikabu_hnd = -1;   // 切り株画像のハンドル
    //double kirikabu_scale = 2.0;

     // # 外周の壁 / . 床 / s 影 / S スタート / G ゴール / T 倒木
    const char* STAGE1[MAP_H] = {
        "########################################",
        "#SS....................................#",
        "#SS.................sssss..............#",
        "#...................sssss..............#",
        "#...................sssss..............#",
        "#......................................#",
        "#...........T..........................#",
        "#......ssss.T..........................#",
        "#......ssss.T..........................#",
        "#......ssss.T.................sssss....#",
        "#......ssss.T.......TTTTTTTT..sssss....#",
        "#...........T.................sssss....#",
        "#...........T.................sssss....#",
        "#...........T.................sssss....#",
        "#sss........T.................sssss....#",
        "#sss..........................sssss....#",
        "#sss...................................#",
        "#sss.................................GG#",
        "#sss.................................GG#",
        "########################################",
    };

    // 2面はまず1面のコピーで動作確認(あとで書き換える)
    const char* STAGE2[MAP_H] = {
        "########################################",
        "#SS....................................#",
        "#SS.................sssss..............#",
        "#...................sssss..............#",
        "#...................sssss..............#",
        "#......................................#",
        "#...........T..........................#",
        "#......ssss.T..........................#",
        "#......ssss.T..........................#",
        "#......ssss.T..........................#",
        "#......ssss.T.......TTTTTTTT...........#",
        "#...........T..........................#",
        "#...........T.................sssss....#",
        "#...........T.................sssss....#",
        "#sss........T..........................#",
        "#sss..........................sssss....#",
        "#sss...................................#",
        "#sss.................................GG#",
        "#sss.................................GG#",
        "########################################",
    };
}

void Map::Init(int stage)
{
    //  画像読み込み
    if (leaf_hnd == -1) {
        leaf_hnd = LoadGraph("data/leaf.png");
    }
    if (kusa_hnd == -1) {
        kusa_hnd = LoadGraph("data/kusamura.png");
    }
    if (log_hnd == -1) {
        log_hnd = LoadGraph("data/touboku.png");
        GetGraphSize(log_hnd, &log_w, &log_h);
    }

    // 文字列 → タイル
    const char* const* data = (stage == 1) ? STAGE1 : STAGE2;
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            tiles[y][x] = data[y][x];
        }
    }

    // 倒木の自動登録
    logs.clear();

    // 横の倒木
    for (int y = 0; y < MAP_H; y++) {
        int x = 0;
        while (x < MAP_W) {
            if (tiles[y][x] != 'T') { x++; continue; }
            const int start = x;
            while (x < MAP_W && tiles[y][x] == 'T') x++;
            const int len = x - start;
            if (len >= 2) {
                logs.push_back({ 
                    start * TILE_SIZE + len * TILE_SIZE / 2.0f,
                    y * TILE_SIZE + TILE_SIZE / 2.0f,
                    static_cast<float>(len * TILE_SIZE),
                    false
                    });
            }
        }
    }

    // 縦の倒木
    for (int x = 0; x < MAP_W; x++) {
        int y = 0;
        while (y < MAP_H) {
            if (tiles[y][x] != 'T') { y++; continue; }
            const int start = y;
            while (y < MAP_H && tiles[y][x] == 'T') y++;
            const int len = y - start;
            if (len >= 2) {
                logs.push_back({
                    x * TILE_SIZE + TILE_SIZE / 2.0f,
                    start * TILE_SIZE + len * TILE_SIZE / 2.0f,
                    static_cast<float>(len * TILE_SIZE),
                    true
                    });
            }
        }
    }

    // 葉っぱ生成
    leaves.clear();
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            if (tiles[y][x] != 's') continue;
            for (int i = 0; i < LEAVES_PER_TILE; i++) {
                Leaf leaf;
                leaf.x = static_cast<float>(x * TILE_SIZE + GetRand(TILE_SIZE - 1));
                leaf.y = static_cast<float>(y * TILE_SIZE + GetRand(TILE_SIZE - 1));
                leaf.angle = GetRand(359) * DX_PI / 180.0;
                leaves.push_back(leaf);
            }
        }
    }
}

void Map::DrawLogs() const
{
    for (const Log& l : logs)
    {
        // 長い辺(log_h = 358)を length に合わせる。縦横共通の拡大率なので歪まない
        const double rate = l.length / log_h;

        // 元画像が縦長なので、横の倒木のときだけ回転する
        const double angle = l.vertical ? 0.0 : DX_PI / 2.0;

        DrawRotaGraph(
            static_cast<int>(l.cx), static_cast<int>(l.cy),
            rate, angle,
            log_hnd, TRUE
        );
    }
   
}

void Map::Draw()
{
    // ---- パス1:床・壁・ゴール ----
    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {
            const int drawX = x * TILE_SIZE;
            const int drawY = y * TILE_SIZE;

            unsigned int color;
            switch (tiles[y][x])
            {
            case '#': color = GetColor(100, 100, 130); break;
            case 'G': color = GetColor(50, 180, 90);   break;
            default:  color = GetColor(82, 58, 39);    break;   // 's' も床扱い
            }

            DrawBox(drawX, drawY, drawX + TILE_SIZE, drawY + TILE_SIZE, color, TRUE);
        }
    }

    // ---- パス2:境界線(ブレンド切り替えは1回だけ) ----
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 50);
    const unsigned int lineColor = GetColor(45, 45, 55);
    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {
            const int drawX = x * TILE_SIZE;
            const int drawY = y * TILE_SIZE;
            DrawBox(drawX, drawY, drawX + TILE_SIZE, drawY + TILE_SIZE, lineColor, FALSE);
        }
    }
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    DrawLogs();

    // ---- パス3:外周の草むら(はみ出しが隠れないよう最後に描く) ----
    if (kusa_hnd == -1) return;

    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {
            const bool isBorder = (x == 0 || y == 0 || x == MAP_W - 1 || y == MAP_H - 1);
            if (!isBorder || tiles[y][x] != '#') continue;

            const int drawX = x * TILE_SIZE;
            const int drawY = y * TILE_SIZE;

            DrawExtendGraph(
                drawX - KUSA_MARGIN, drawY - KUSA_MARGIN,
                drawX + TILE_SIZE + KUSA_MARGIN, drawY + TILE_SIZE + KUSA_MARGIN,
                kusa_hnd,
                TRUE
            );
        }
    }
}

void Map::DrawLeaves() const
{
    if (leaf_hnd == -1) return;

    for (const Leaf& l : leaves)
    {
        DrawRotaGraph(
            static_cast<int>(l.x), static_cast<int>(l.y),
            leaf_scale, l.angle,
            leaf_hnd, TRUE
        );
    }
}

//  タイル文字取得
char Map::GetTile(float x, float y) const
{
    //  タイル座標に変換　tx,ty
    const int tx = static_cast<int>(x / TILE_SIZE);
    const int ty = static_cast<int>(y / TILE_SIZE);

    if (tx < 0 || ty < 0 || tx >= MAP_W || ty >= MAP_H) return '#';  // 範囲外は壁扱い
    return tiles[ty][tx];
}

bool Map::IsWall(float x, float y) const
{
    const char t = GetTile(x, y);
    return t == '#' || t == 'T';
}
bool Map::IsShadow  (float x, float y) const { return GetTile(x, y) == 's'; }
bool Map::IsGoal    (float x, float y) const { return GetTile(x, y) == 'G'; }

// 'S' タイルの中心(ピクセル座標)を返す。見つからなければ false
bool Map::GetStartPos(float& x, float& y) const
{
    for (int ty = 0; ty < MAP_H; ty++)
    {
        for (int tx = 0; tx < MAP_W; tx++)
        {
            if (tiles[ty][tx] == 'S')
            {
                x = tx * TILE_SIZE + TILE_SIZE / 2.0f;
                y = ty * TILE_SIZE + TILE_SIZE / 2.0f;
                return true;
            }
        }
    }
    return false;
}