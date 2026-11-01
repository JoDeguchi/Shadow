#pragma once
#include "GameState.h"
#include "KeyReader.h"
#include "Graph.h"

// EndingState.h(または共通のヘッダ)
enum class EndingType {
    Goal,    // ゴールした
    Found,   // 見つかった
};

class EndingState : public GameState 
{
private:

    EndingType ending_type;
    KeyReader key;

	GraphDraw successBG;  // 成功背景画像
    GraphDraw failedBG;   // 失敗背景画像
   
public:

    EndingState(GameManager* manager, EndingType type);

	void Init() override;
    void Input()override;
    void Update() override;
    void Draw() override;
    
};