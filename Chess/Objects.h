#ifndef OBJECTS
#define OBJECTS

#include "702/702_Util.h"
#include "Common.h"

/*namespace QuickDef
{
	enum Player	{Black, Red};
	Player& operator++ (Player& lhs)//for simplification of player switching
{
	if (lhs == 1)
	{
		lhs = Black;
		return lhs;
	}
	else
	{
		lhs = Red;
		return lhs;
	}
};

enum MoveType	{King, Queen, Knight, Bishop, Rook, Pawn, None};
enum Type {Board, Piece, Menu};
};// end namespace

struct Location2D
{
	int x;
	int z;
};*/

class Object
{
public:
	virtual ~Object();

	XMFLOAT4X4 GetWorldTransform();
	void SetLocation(int x, int z);
	void UpdateLocation(int dxiffX, int diffZ);
	Location2D GetLocation();


	QuickDef::MoveType GetMoveType();
	QuickDef::Player GetPlayer();
	QuickDef::Type GetObjectType();

	bool HasSpecial();
	void EndSpecial();
	void MakeSelected();
	void Deselect();


	XMFLOAT4 mColorBase;
	XMFLOAT4 mColorHighlight;

public:
	const UINT ID;

protected:
	//Object();
	Object(QuickDef::Player player, QuickDef::MoveType moveType, QuickDef::Type Type,  int x, int z);

protected:
	static UINT NextID;

	XMFLOAT4X4 mWorld;


	QuickDef::MoveType mMoveType;
	QuickDef::Player mFaction;
	QuickDef::Type mType;

	bool mSpecialMove;
	bool mSelected;

	Location2D mLocation;
	
};

class King : public Object
{
public:
	King();
	King(QuickDef::Player player, int x, int z);
};

class Queen : public Object
{
public:
	Queen();
	Queen(QuickDef::Player player, int x, int z);
};

class Knight : public Object
{
public:
	Knight();
	Knight(QuickDef::Player player, int x, int z);
};

class Bishop : public Object
{
public:
	Bishop();
	Bishop(QuickDef::Player player, int x, int z);
};

class Rook : public Object
{
public:
	Rook();
	Rook(QuickDef::Player player, int x, int z);
};

class Pawn : public Object
{
public:
	Pawn();
	Pawn(QuickDef::Player player, int x, int z);
};

class Board : public Object
{
public:
	Board();
	Board(QuickDef::Player player, int x, int z);
};

class Menu //: public Object
{
public:
	Menu(float xOffset, float yOffset, MenuDef::MenuType type);
	~Menu();


//	void Draw(ID3D11DeviceContext* d3dImmediateContext);

	MenuDef::MenuType GetMenuAction();
	XMMATRIX GetWorldTransform();
	//void GetBufferOffsets(UINT& indexCount, UINT& indexOffset, UINT&

private:
	XMFLOAT4X4 mWorld;

	MenuDef::MenuType mMenuType;

	//static ID3D11Buffer* mBackDropVertexBuffer;
	//static ID3D11Buffer* mBackDropIndexBuffer;
	//static UINT mBackDropIndexCount;

	//static UINT mBaseReferenceCount;
	/*static UINT mMenuNewRefCount;
	static UINT mMenuLoadRefCount;
	static UINT mMenuSaveRefCount;
	static UINT mMenuExitRefCount;
	static UINT mMenuPlayerRefCount;
	static UINT mMenuReplayRefCount;
	static UINT mMenuQueenRefCount;
	static UINT mMenuKnightRefCount;
	static UINT mMenuBishopRefCount;
	static UINT mMenuRookRefCount;
	static UINT mMenuRedRefCount;
	static UINT mMenuBlackRefCount;
	static UINT mMenuWinRefCount;*/

//protected:
	//static ID3D11Buffer* mMenuVertexBuffer;
	//static ID3D11Buffer* mMenuIndexBuffer;

	//UINT mMenuIndexCount;


};

//*****************************************
//		Menu Child Declarations
//*****************************************

class MenuNew : public Menu
{
public:
	MenuNew(int xOffset, int yOffset, ID3D11DeviceContext* immediateContext);
	~MenuNew();

	void Draw(ID3D11DeviceContext* immediateContext);
};

class MenuLoad : public Menu
{
public:
	MenuLoad(int xOffset, int yOffset, ID3D11DeviceContext* immediateContext);
	~MenuLoad();

	void Draw(ID3D11DeviceContext* immediateContext);
};

class MenuSave : public Menu
{
public:
	MenuSave(int xOffset, int yOffset, ID3D11DeviceContext* immediateContext);
	~MenuSave();

	void Draw(ID3D11DeviceContext* immediateContext);
};

class MenuExit : public Menu
{
public:
	MenuExit(int xOffset, int yOffset, ID3D11DeviceContext* immediateContext);
	~MenuExit();

	void Draw(ID3D11DeviceContext* immediateContext);
};




#endif