#pragma once

#include <Windows.h>


class Object;

struct ObjectInfo
{
	Object** BlackObjects;
	Object** RedObjects;
	Object** BoardObjects;
	UINT numBlack;
	UINT numRed;
	UINT numBoards;
};

namespace QuickDef
{
	enum Player	{Black, Red, NoPlayer};
Player& operator++ (Player& lhs);

enum MoveType	{King, Queen, Knight, Bishop, Rook, Pawn, None};
enum Type {Board, Piece, Menu};

};// end namespace

namespace MenuDef
{
	enum MenuLevel {Main, PlayNew, PlayLoad, Ranked, Won};
	enum MenuType {New, Load, Save, Exit, NewPlayer, LoadPlayer, NewReplay, LoadReplay, Queen, Knight, Bishop, Rook, Red, Black, Win, None};
};

struct Location2D
{
	int x;
	int z;
};

struct Event
{
	QuickDef::Player CurrPlayer;
	Object* Selected;
	Object* Picked;
	bool Move;
	bool Real;
	int diffX;
	int diffZ;
};
