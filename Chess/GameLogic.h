#ifndef GAMELOGIC
#define GAMELOGIC

#include "GameView.h"
//#include "Objects.h"
//#include "Common.h"

class GameView;
struct ObjectInfo;

/*struct Event
{
	QuickDef::Player CurrPlayer;
	Object* Selected;
	Object* Picked;
	bool Move;
	int diffX;
	int diffZ;
};*/

class GameLogic
{
public:
	GameLogic();
	~GameLogic();

	LRESULT MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void Frame();
	bool EventProcess(const Event& data);
	bool AttachViews(UINT numViews, bool player, GameView** views);
	void QuickRun();
	void SaveGame();
	void Reset();

	ObjectInfo ObjectRequest();

private:
	void InitObjects();
	void SwapPlayer(bool replay);
	void RemoveObject(Object* const removed);
	bool MoveProcess(Object* selected, int diffX, int diffZ, bool real);
	bool CheckPositions(Location2D base, QuickDef::Player currPlayer, int diffX, int diffZ, bool castle, bool real);
	bool CheckWin(Object* const picked);

private:
	Object* mGameBoard[64];
	//Object* mAllPlayerObjects[32];
	Object* mBlackObjects[16];
	Object* mRedObjects[16];

	//UINT mTotalPlayerObjCount;
	UINT mBlackObjectCount;
	UINT mRedObjectCount;

	GameView* mPlayers[2];
	QuickDef::Player mCurrPlayer;
	QuickDef::Player mLastPlayer;
	bool mReplay;// i dont lik the way i am managing the views!!!!!!
};


#endif