#ifndef CHESS
#define CHESS
//#include "702/702_Create.h"
//#include "702/702_Util.h"
//#include "GameLogic.h"
#include "GameView.h"

class Chess
{
public:
	Chess(HINSTANCE hInstance);
	~Chess();

	//LRESULT MsgProcMain(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	int Run();

	bool Init();

private:

	void ProcessMenu(MenuDef::MenuType action);

private:

	bool mCurrentlyPlayer;

	HINSTANCE mhInstance;

	GameLogic* mGameLogic;
	GameView* mPlayerViews[2];
	GameView* mReplayView;
//	GameView* mReplayView;
	RenderTarget* mRenderTarget;
};
#endif