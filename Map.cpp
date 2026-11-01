#include "Map.h"
#include <cmath>

namespace {
    constexpr float LEAF_SIZE = 48.0f;        // 葉っぱ1枚の表示サイズ(px)
    constexpr int   LEAVES_PER_TILE = 3;      // 1タイルあたりの枚数
    constexpr int   KUSA_MARGIN = 8;   // 草むらを1マスより外側へはみ出させる量(px)
    constexpr float LOG_THICKNESS = 56.0f;   // 倒木の太さ(px)。TILE_SIZE より少し大きいと自然
}

// マップを初期化
void Map::Init()
{
    if (leaf_hnd == -1) {
        leaf_hnd = LoadGraph("data/leaf.png");
        int w = 0, h = 0;
        GetGraphSize(leaf_hnd, &w, &h);
        if (w > 0) leaf_scale = LEAF_SIZE / w;   // 画像サイズに関わらずLEAF_SIZEに揃える
    }

    if (kusa_hnd == -1) {
        kusa_hnd = LoadGraph("data/kusamura.png");
    }

    if (log_hnd == -1) {
        log_hnd = LoadGraph("data/touboku.png");
        GetGraphSize(log_hnd, &log_w, &log_h);
    }

    // 最初はすべて床にする
    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {
            tiles[y][x] = '.';
        }
    }

    // 外周を壁にする
    for (int x = 0; x < MAP_W; x++)
    {
        tiles[0][x] = '#';
        tiles[MAP_H - 1][x] = '#';
    }
    for (int y = 0; y < MAP_H; y++)
    {
        tiles[y][0] = '#';
        tiles[y][MAP_W - 1] = '#';
    }

    logs.clear();

    // 縦の倒木(x=12, y=6～14)
    for (int y = 6; y < 15; y++)
    {
        tiles[y][12] = 'T';
    }
    logs.push_back({
		12 * TILE_SIZE + TILE_SIZE / 2.0f,        // ピクセル座標左端＋半分
        6 * TILE_SIZE + 9 * TILE_SIZE / 2.0f,     // 中心Y(9マス分の真ん中)
        9.0f * TILE_SIZE,
        true
        }
    );

    // 横の倒木(y=10, x=20～27)
    for (int x = 20; x < 28; x++)
    {
        tiles[10][x] = 'T';
    }
    logs.push_back({
        20 * TILE_SIZE + 8 * TILE_SIZE / 2.0f,    // 中心X(8マス分の真ん中)
        10 * TILE_SIZE + TILE_SIZE / 2.0f,        // 中心Y
        8.0f * TILE_SIZE,
        false
        });

    // 影を配置
    for (int y = 7; y < 11; y++)
    {
        for (int x = 7; x < 11; x++)
        {
            tiles[y][x] = 's';
        }
    }

    for (int y = 14; y < 19; y++)
    {
        for (int x = 1; x < 4; x++)
        {
            tiles[y][x] = 's';
        }
    }

    for (int y = 9; y < 16; y++)
    {
        for (int x = 30; x < 35; x++)
        {
            tiles[y][x] = 's';
        }
    }
    for (int y = 2; y < 5; y++)
    {
        for (int x = 20; x < 25; x++)
        {
            tiles[y][x] = 's';
        }
    }

    // スタート
    for (int y = 1; y < 3; y++)
    {
        for (int x = 1; x <3; x++)
        {
            tiles[y][x] = 'S';
        }
    }
    //  ゴール
    for (int y = 17; y < 19; y++)
    {
        for (int x = 37; x < 39; x++)
        {
            tiles[y][x] = 'G';
        }
    }

    // 影タイルの範囲に、葉っぱをランダムに配置(最初に1回だけ決める)
    leaves.clear();
    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {
            if (tiles[y][x] != 's') continue;

            for (int i = 0; i < LEAVES_PER_TILE; i++)
            {
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