#include <WindowsX.h>
#include "GameLogic.h"

RenderTarget* GameView::mRenderTarget = 0;
GameLogic* GameView::mGameLogic = 0;

GameLogic::GameLogic()
	:mBlackObjectCount(16), mRedObjectCount(16), mCurrPlayer(QuickDef::Black), mLastPlayer(QuickDef::Red), mReplay(0)
{
	for (UINT i = 0; i < 16; ++i)
	{
		mBlackObjects[i] = 0;
		mRedObjects[i] = 0;
	}
	mPlayers[0] = 0;
	mPlayers[1] = 0;

	InitObjects();
};

GameLogic::~GameLogic()
{
	for (UINT i = 0; i < 16; ++i)
	{
		delete mBlackObjects[i];
		delete mRedObjects[i];
		delete mGameBoard[i];
		delete mGameBoard[i + 16];
		delete mGameBoard[i + 32];
		delete mGameBoard[i + 48];
	}
};

LRESULT GameLogic::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
			if (mReplay)
				mPlayers[0]->MouseInput(msg, wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

			else if (mPlayers[mCurrPlayer] != 0)
				mPlayers[mCurrPlayer]->MouseInput(msg, wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
};

void GameLogic::Frame()
{
	ObjectInfo objects;
	objects.BlackObjects = mBlackObjects;
	objects.RedObjects = mRedObjects;
	objects.BoardObjects = mGameBoard;
	objects.numBlack = mBlackObjectCount;
	objects.numRed = mRedObjectCount;
	objects.numBoards = 64;

	if (mReplay)
		mPlayers[0]->Frame(objects);
	else
		mPlayers[mCurrPlayer]->Frame(objects);
};

bool GameLogic::EventProcess(const Event& data)
{
	if (data.Move)
	{
		if (MoveProcess(data.Selected, data.diffX, data.diffZ, data.Real))// TODO: Checl: possible issue not referencing player?
		{
			if (! data.Real)
			{
				return true;
			}
			Location2D location = data.Picked->GetLocation();

			data.Selected->UpdateLocation(data.diffX, data.diffZ);
			data.Selected->EndSpecial();

			if (data.Picked->GetObjectType() == QuickDef::Piece)
			{
				if (CheckWin(data.Picked))
				{
					if (mReplay)
						mPlayers[0]->Win();
					else
						mPlayers[mCurrPlayer]->Win();
				}

				RemoveObject(data.Picked);
			}
			else
			{
				Object** objects = 0;
				UINT count = 0;
				if (data.CurrPlayer == QuickDef::Black)
				{
					objects = mRedObjects;
					count = mRedObjectCount;
				}
				else
				{
					objects = mBlackObjects;
					count = mBlackObjectCount;
				}

				location = data.Selected->GetLocation();
				UINT finalX = location.x;
				UINT finalZ = location.z;
				for (UINT i = 0; i < count; ++i)
				{
					location = objects[i]->GetLocation();
					if (location.x == finalX && location.z == finalZ)
					{
						if (CheckWin(objects[i]))
							mPlayers[mCurrPlayer]->Win();

						RemoveObject(objects[i]);
						break;
					}
				}
			}

			SwapPlayer(mReplay);
			return true;
		}
	}
	else
	{
		Object** objects = 0;
		UINT count =  0;
		UINT targetZ = 3;
		if (data.CurrPlayer == QuickDef::Black)
		{
			objects = mBlackObjects;
			count = mBlackObjectCount;
			targetZ = 7;
		}
		else
		{
			objects = mRedObjects;
			count = mRedObjectCount;
			targetZ = 0;
		}

		for (UINT i = 0; i < count; ++i)
		{
			if (objects[i]->GetMoveType() == QuickDef::Pawn)
			{
				Location2D loc = objects[i]->GetLocation();
				if (loc.z == targetZ)
				{
					reinterpret_cast<PlayerView*>(&(mPlayers[mCurrPlayer]))->LogPromotion(data.Picked->GetMoveType());
					Object* replacement = data.Picked;
					delete objects[i];
					objects[i] = replacement;
					replacement->UpdateLocation(loc.x, loc.z);
					return true;
				}
			}
		}
	}
	return false;
};

bool GameLogic::AttachViews(UINT numViews, bool player, GameView** views)
{
	for (UINT i = 0; i < numViews; ++i)
	{
		mPlayers[i] = views[i];
	}

	if (! player )
	{
		mReplay = true;
		mPlayers[1] = 0;
	}
	else
		mReplay = false;

	return true;
};

void GameLogic::QuickRun()
{
	if (! mReplay)
		return;

	dynamic_cast<ReplayView*>(mPlayers[0])->QuickRun();
	mCurrPlayer = mLastPlayer;
};

void GameLogic::SaveGame()
{
	mPlayers[mCurrPlayer]->SaveGame();
};

void GameLogic::Reset()
{
	mCurrPlayer = QuickDef::Black;
	mLastPlayer = QuickDef::Black;

	for (UINT i = 0; i < 16; ++i)
	{
		delete mBlackObjects[i];
		mBlackObjects[i] = 0;
		delete mRedObjects[i];
		mRedObjects[i] = 0;
		delete mGameBoard[i];
		mGameBoard[i] = 0;
		delete mGameBoard[i + 16];
		mGameBoard[i + 16] = 0;
		delete mGameBoard[i + 32];
		mGameBoard[i + 32] = 0;
		delete mGameBoard[i + 48];
		mGameBoard[i + 48] = 0;
	}
	InitObjects();

	if (mReplay)
	{
		mPlayers[0]->Reset();
	}
	else
	{
		mPlayers[0]->Reset();
		mPlayers[1]->Reset();
	}
};

ObjectInfo GameLogic::ObjectRequest()
{
	ObjectInfo currInfo;
	currInfo.BlackObjects = mBlackObjects;
	currInfo.numBlack = mBlackObjectCount;
	currInfo.RedObjects = mRedObjects;
	currInfo.numRed = mRedObjectCount;
	currInfo.BoardObjects = mGameBoard;
	currInfo.numBoards = 64;

	return currInfo;
};

void GameLogic::InitObjects()
{
	mBlackObjectCount = 16;
	mRedObjectCount = 16;

	UINT k = 0;
	for (UINT i = 0; i < 8; ++i)
	{
		for (UINT j = 0; j < 8; ++j, ++k)
		{
			if ( (i + j) % 2 == 0 )
			{
				mGameBoard[k] = new Board(QuickDef::Black, j, i);
			}
			else
			{
				mGameBoard[k] = new Board(QuickDef::Red, j, i);
			}
		}
	}

	int zB = 1;
	int zR = 6;
	QuickDef::Player player = QuickDef::Black;
	//set pawns
	for (UINT x = 0; x < 8; ++x)
	{
		mBlackObjects[x] = new Pawn(QuickDef::Black, x, zB);
		mRedObjects[x] = new Pawn (QuickDef::Red, x, zR);
	}

	zB = 0;
	zR = 7;
	int x = 0;

	for (UINT i = 8; i < 10; ++i)
	{
		mBlackObjects[i] = new Rook(QuickDef::Black, x, zB);
		mRedObjects[i] = new Rook(QuickDef::Red, x, zR);
		x = 7;
	}

	x = 1;

	for (UINT i = 10; i < 12; ++i)
	{
		mBlackObjects[i] = new Knight(QuickDef::Black, x, zB);
		mRedObjects[i] = new Knight (QuickDef::Red, x, zR);
		x = 6;
	}

	x = 2;

	for (UINT i = 12; i < 14; ++i)
	{
		mBlackObjects[i] = new Bishop(QuickDef::Black, x, zB);
		mRedObjects[i] = new Bishop (QuickDef::Red, x, zR);
		x = 5;
	}

	x = 3;

	mBlackObjects[14] = new Queen(QuickDef::Black, x, zB);
	mRedObjects[14] = new King (QuickDef::Red, x, zR);

	x = 4;

	mBlackObjects[15] = new King(QuickDef::Black, x, zB);
	mRedObjects[15] = new Queen (QuickDef::Red, x, zR);
};

void GameLogic::SwapPlayer(bool replay)
{
	if ( ! replay )
		++mCurrPlayer;
	else
		++mLastPlayer;
};

void GameLogic::RemoveObject(Object* const removed)
{
	UINT id;
	Object* tempObjects[16];
	if (removed->GetPlayer() == QuickDef::Black)
	{
		id = removed->ID;
		UINT k = 0;
		for (UINT i = 0; i < 16; ++i)
		{
			if (mBlackObjects[i]->ID != id)
			{
				tempObjects[k] = mBlackObjects[i];
				++k;
			}
		}

		for (UINT i = 0; i < 15; ++i)
		{
			mBlackObjects[i] = tempObjects[i];
		}
		mBlackObjects[15] = removed;

		--mBlackObjectCount;
	}
	else if(removed->GetPlayer() == QuickDef::Red)
	{
		id = removed->ID;
		UINT k = 0;
		for (UINT i = 0; i < 16; ++i)
		{
			if (mRedObjects[i]->ID != id)
			{
				tempObjects[k] = mRedObjects[i];
				++k;
			}
		}

		for (UINT i = 0; i < 15; ++i)
		{
			mRedObjects[i] = tempObjects[i];
		}
		mRedObjects[15] = removed;

		--mRedObjectCount;
	}
};

bool GameLogic::MoveProcess(Object* selected, int diffX, int diffZ, bool real)
{
	QuickDef::Player player = selected->GetPlayer();
	QuickDef::MoveType moveType = selected->GetMoveType();
	Location2D base = selected->GetLocation();
	bool special = selected->HasSpecial();

	bool falseMove = true;

	if (diffX == 0 && diffZ == 0)
		return false;

	if (moveType != QuickDef::Knight)
	{
		if (! CheckPositions(base, player, diffX, diffZ, false, real))
		{
			return false;
		}
	}

	switch (moveType)
	{
	case QuickDef::King:
		if (diffX == 0 && diffZ == 0)
			falseMove = false;
		if ( ! (diffX > 1 || diffX < -1) )
			if ( ! (diffZ > 1 || diffZ < -1) )
				falseMove = false;
		if ( (diffX == 2 || diffX == -2) && diffZ == 0)
			if (special)
				if (CheckPositions(base, player, diffX, diffZ, true, real))
					falseMove = false;
		break;

	case QuickDef::Queen:
		if ( diffX == 0 || diffZ == 0)
			falseMove = false;
		if ( diffX == diffZ || -diffX == diffZ)
			falseMove = false;
		break;

	case QuickDef::Knight:
		if ( (diffX == 2 || diffX == -2) && (diffZ == 1 || diffZ == -1) )
			falseMove = false;
		if ( (diffZ == 2 || diffZ == -2) && (diffX == 1 || diffX == -1) )
			falseMove = false;
		break;

	case QuickDef::Bishop:
		if ( diffX == diffZ || diffX == -diffZ )
			falseMove = false;
		break;
		
	case QuickDef::Rook:
		if ( diffX == 0 || diffZ == 0 )
			falseMove = false;
		break;

	case QuickDef::Pawn:
		if (player == QuickDef::Red)
		{
			if (diffZ == -1 && diffX == 0)
				if (CheckPositions(base, player, diffX, diffZ - 1, false, real))
					falseMove = false;

			if (diffZ == -2 && diffX == 0)
				if (selected->HasSpecial())
					if(CheckPositions(base, player, diffX, diffZ - 1, false, real))
						falseMove = false;

			if ((diffX == 1 || diffX == -1) && diffZ == -1)
			{
				UINT finalX = base.x + diffX;
				UINT finalZ = base.z + diffZ;
				for (UINT i = 0; i < mBlackObjectCount; ++i)
				{
					Location2D location = mBlackObjects[i]->GetLocation();
					if ( location.x == finalX && location.z == finalZ)
						falseMove = false;
				}
			}
		}
		else
		{
			if (diffZ == 1 && diffX == 0 )
				if (CheckPositions(base, player, diffX, diffZ + 1, false, real))
					falseMove = false;

			if (diffZ == 2 && diffX == 0)
				if (selected->HasSpecial())
					if (CheckPositions(base, player, diffX, diffZ + 1, false, real))
						falseMove = false;

			if ((diffX == 1 || diffX == -1) && diffZ == 1)
			{
				UINT finalX = base.x + diffX;
				UINT finalZ = base.z + diffZ;
				for (UINT i = 0; i < mRedObjectCount; ++i)
				{
					Location2D location = mRedObjects[i]->GetLocation();
					if ( location.x == finalX && location.z == finalZ)
						falseMove = false;
				}
			}
		}
		break;

	}

	if (diffX == 0 && diffZ == 0)
		falseMove = true;

	UINT finalX = base.x + diffX;
	UINT finalZ = base.z + diffZ;
	Object** objects = 0;
	UINT count = 0;
	if (player == QuickDef::Black)
	{
		objects = mBlackObjects;
		count = mBlackObjectCount;
	}
	else
	{
		objects = mRedObjects;
		count = mRedObjectCount;
	}

	for (UINT i = 0; i < count; ++i)
	{
		Location2D location = objects[i]->GetLocation();
		if (location.x == finalX && location.z == finalZ)
			falseMove = true;
	}


	if (falseMove)
	{
		return false;
	}

	return true;
};

bool GameLogic::CheckPositions(Location2D base, QuickDef::Player currPlayer, int diffX, int diffZ, bool castle, bool real)
{
	Object** playerObjects = 0;
	Object** opponentObjects = 0;
	Object** fullObjects = new Object*[mRedObjectCount + mBlackObjectCount];
	UINT fullCount = mRedObjectCount + mBlackObjectCount;
	UINT plCount = 0;
	UINT opCount = 0;
	for(UINT i = 0; i < mBlackObjectCount; ++i)
	{
		fullObjects[i] = mBlackObjects[i];
	}
	for(UINT i = 0; i < mRedObjectCount; ++i)
	{
		fullObjects[i + mBlackObjectCount] = mRedObjects[i];
	}

	if (currPlayer == QuickDef::Black)
	{
		playerObjects = mBlackObjects;
		opponentObjects = mRedObjects;
		plCount = mBlackObjectCount;
		opCount = mRedObjectCount;
	}
	else
	{
		playerObjects = mRedObjects;
		opponentObjects = mBlackObjects;
		plCount = mRedObjectCount;
		opCount = mBlackObjectCount;
	}


	if (castle)
	{
		for(UINT i = 0; i < plCount; ++i)
		{
			if (playerObjects[i]->GetMoveType() == QuickDef::Rook)
			{
				if (playerObjects[i]->HasSpecial())
				{
					Location2D location = playerObjects[i]->GetLocation();
					if (diffX < 0 && location.x == 0)
					{
						if (CheckPositions(base, currPlayer, (-base.x), diffZ, false, real))
						{
							if (real)
							{
								if (currPlayer == QuickDef::Black)
								{
									playerObjects[i]->UpdateLocation(-(diffX - 1), diffZ);
									playerObjects[i]->EndSpecial();
								}
								else
								{
									playerObjects[i]->UpdateLocation(-diffX, diffZ);
									playerObjects[i]->EndSpecial();
								}
							}
							delete fullObjects;
							return true;
						}
					}
					else if (diffX > 0 && location.x == 7)
					{
						if (CheckPositions(base, currPlayer, 7 - base.x, diffZ, false, real))
						{
							if (real)
							{
								if (currPlayer == QuickDef::Black)
								{
									playerObjects[i]->UpdateLocation(-diffX, diffZ);
									playerObjects[i]->EndSpecial();
								}
								else
								{
									playerObjects[i]->UpdateLocation(-(diffX + 1), diffZ);
									playerObjects[i]->EndSpecial();
								}
							}
							delete fullObjects;
							return true;
						}
					}
				}
			}
		}
		delete fullObjects;
		return false;
	}
	if (diffX == 0 && diffZ == 0)
	{
		delete fullObjects;
		return false;
		for (UINT i = 0; i < plCount; ++i)
		{
			Location2D location = playerObjects[i]->GetLocation();
			if (location.x == base.x && location.z == base.z)
				return false;
		}
		for (UINT i = 0; i < opCount; ++i)
		{
			Location2D location = opponentObjects[i]->GetLocation();
			if (location.x == base.x && location.z == base.z)
				return false;
		}
		return true;
	}

	bool virtualBoard[8][8];
	for (UINT i = 0; i < 8; ++i)
	{
		for (UINT j = 0; j < 8; ++j)
		{
			virtualBoard[i][j] = false;
		}
	}
	for (UINT i = 0; i < plCount; ++i)
	{
		Location2D location = playerObjects[i]->GetLocation();
		virtualBoard[location.z][location.x] = true;
	}
	for (UINT i = 0; i < opCount; ++i)
	{
		Location2D location = opponentObjects[i]->GetLocation();
		virtualBoard[location.z][location.x] = true;
	}

	Location2D location;

	if (diffX == 0 || diffZ == 0)
	{
		if (diffX == 0)
		{
			UINT finalZ = base.z + diffZ;

			if (diffZ > 0)
			{
				for (int z = finalZ - 1; z > base.z; --z)
				{
					for(UINT i = 0; i < fullCount; ++i)
					{
						location = fullObjects[i]->GetLocation();
						if (location.z == z && location.x == base.x)
						{
							delete fullObjects;
							return false;
						}
					}
				}
				delete fullObjects;
				return true;
			}
			if (diffZ < 0)
			{
				for (int z = finalZ + 1; z < base.z; ++z)
				{
					for (UINT i = 0; i < fullCount; ++i)
					{
						location = fullObjects[i]->GetLocation();
						if (location.z == z && location.x == base.x)
						{
							delete fullObjects;
							return false;
						}
					}
				}
				delete fullObjects;
				return true;
			}
		}

		if (diffZ == 0)
		{
			UINT finalX = base.x + diffX;

			if (diffX > 0)
			{
				for (UINT x = base.x + 1; x < finalX; ++x)
				{
					for (UINT i = 0; i < fullCount; ++i)
					{
						location = fullObjects[i]->GetLocation();
						if (location.x == x && location.z == base.z)
						{
							delete fullObjects;
							return false;
						}
					}
				}
				delete fullObjects;
				return true;
			}

			if (diffX < 0)
			{
				for (UINT x = base.x - 1; x > finalX; --x)
				{
					for (UINT i = 0; i < fullCount; ++i)
					{
						location = fullObjects[i]->GetLocation();
						if (location.x == x && location.z == base.z)
						{
							delete fullObjects;
							return false;
						}
					}
				}
				delete fullObjects;
				return true;
			}
		}
	}
	UINT finalX = base.x + diffX;
	UINT finalZ = base.z + diffZ;
	// + X   + Z
	if (diffX > 0 && diffZ > 0)
	{
		for (UINT x = base.x + 1, z = base.z + 1; x < finalX; ++x, ++z)
		{
			for (UINT i = 0; i < fullCount; ++i)
			{
				location = fullObjects[i]->GetLocation();
				if (location.x == x && location.z == z)
				{
					delete fullObjects;
					return false;
				}
			}
		}
		delete fullObjects;
		return true;
	}
	//+ X   - Z
	else if (diffX > 0 && diffZ < 0)
	{
		for (UINT x = base.x + 1, z = base.z - 1; x < finalX; ++x, --z)
		{
			for (UINT i = 0; i < fullCount; ++i)
			{
				location = fullObjects[i]->GetLocation();
				if (location.x == x && location.z == z)
				{
					delete fullObjects;
					return false;
				}
			}
		}
		delete fullObjects;
		return true;
	}
	//- X   - Z
	else if (diffX < 0 && diffZ < 0)
	{
		for (UINT x = base.x - 1, z = base.z - 1; x > finalX; --x, --z)
		{
			for (UINT i = 0; i < fullCount; ++i)
			{
				location = fullObjects[i]->GetLocation();
				if (location.x == x && location.z == z)
				{
					delete fullObjects;
					return false;
				}
			}
		}
		delete fullObjects;
		return true;
	}
	// - X   + Z
	else if (diffX < 0 && diffZ > 0)
	{
		for (UINT x = base.x - 1, z = base.z + 1; x > finalX; --x, ++z)
		{
			for (UINT i = 0; i < fullCount; ++i)
			{
				location = fullObjects[i]->GetLocation();
				if (location.x == x && location.z == z)
				{
					delete fullObjects;
					return false;
				}
			}
		}
		delete fullObjects;
		return true;
	}
	delete fullObjects;
	return false;
};

bool GameLogic::CheckWin(Object* const picked)
{
	if (picked->GetMoveType() == QuickDef::King)
		return true;
	else
		return false;
};