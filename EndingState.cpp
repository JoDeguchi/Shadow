#include "EndingState.h"
#include "TitleState.h"
#include "GameManager.h"
#include "Dxlib.h"

EndingState::EndingState(GameManager* manager, EndingType type)
	: GameState(manager), ending_type(type)
{}

void EndingState::Init()
{
	if (ending_type == EndingType::Goal) {

		successBG = GraphDraw("data/Success.jpg", 0, 0);

	}
	else {
		failedBG = GraphDraw("data/Failed.jpg", 0, 0);
	}
}

void EndingState::Input()
{
	key.Read();	//	キーの読み込み

	//	エンターキーでゲーム画面へ
	if (key.CheckReleaseKey(KEY_INPUT_RETURN)) {

		manager->ChangeState(
			std::make_unique<TitleState>(manager)
		);
	}
}

void EndingState::Update()
{

}

void EndingState::Draw()
{
	if (ending_type == EndingType::Goal) {
		successBG.Draw();
	}
	else{
		failedBG.Draw();
	}
}
