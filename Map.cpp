#include "Map.h"
#include <cmath>

// マップを初期化
void Map::Init()
{
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

    // 壁を配置
    for (int x = 8; x < 15; x++)
    {
        tiles[5][x] = '#';
    }
    for (int y = 6; y < 15; y++)
    {
        tiles[y][12] = '#';
    }
    for (int x = 20; x < 28; x++)
    {
        tiles[10][x] = '#';
    }

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
   
}

// マップを描画
void Map::Draw()
{
    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {
            int drawX = x * TILE_SIZE;
            int drawY = y * TILE_SIZE;

            unsigned int color;

            switch (tiles[y][x])
            {
            case '#':
                color = GetColor(100, 100, 130);
                break;

            case 's':
                color = GetColor(12, 12, 20);
                break;

            case 'G':
                color = GetColor(50, 180, 90);
                break;

            default:
                color = ((x + y) % 2 == 0)
                    ? GetColor(78, 78, 92)
                    : GetColor(68, 68, 82);
                break;
            }

            DrawBox(
                drawX,
                drawY,
                drawX + TILE_SIZE,
                drawY + TILE_SIZE,
                color,
                TRUE
            );

            // マスの境界線
            DrawBox(
                drawX,
                drawY,
                drawX + TILE_SIZE,
                drawY + TILE_SIZE,
                GetColor(45, 45, 55),
                FALSE
            );
        }
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

bool Map::IsWall    (float x, float y) const { return GetTile(x, y) == '#'; }
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