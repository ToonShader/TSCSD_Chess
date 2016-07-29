#pragma once

#include "GameView.h"

class Chess
{
public:
	Chess(HINSTANCE hInstance);
	~Chess();

	int Run();

	bool Init();

private:

	bool ProcessMenu(MenuDef::MenuType action);

private:

	bool mCurrentlyPlayer;

	HINSTANCE mhInstance;

	GameLogic* mGameLogic;
	GameView* mPlayerViews[2];
	GameView* mReplayView;
	RenderTarget* mRenderTarget;
};