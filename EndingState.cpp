#include "EndingState.h"
#include "TitleState.h"
#include "GameManager.h"

void EndingState::Init()
{

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
	DrawString(100, 100, "End", GetColor(255, 255, 255));
}
