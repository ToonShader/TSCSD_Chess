#include "Objects.h"

//*********************************
//		Object definitions
//********************************

UINT Object::NextID = 1;


Object::Object(QuickDef::Player player, QuickDef::MoveType moveType, QuickDef::Type type, int x, int z)
	:ID(NextID), mMoveType(moveType), mFaction(player), mType(type), mSpecialMove(false), mSelected(false)
{
	++NextID;

	mLocation.x = x;
	mLocation.z = z;

	XMMATRIX world = XMMatrixTranslation(static_cast<float>(x), 0.0f, static_cast<float>(z));
	XMStoreFloat4x4(&mWorld, world);

	if (mFaction == QuickDef::Red)
	{
		XMVECTOR color = XMVectorSet(0.5106f, 0.4122f, 0.2316f, 1.0f);
		XMStoreFloat4(&mColorBase, color);
	}
	else
	{
		XMVECTOR color = XMVectorSet(0.12f, 0.12f, 0.12f, 1.0f);
		XMStoreFloat4(&mColorBase, color);
	}
	mColorHighlight = reinterpret_cast<float*>(&(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)));
};

Object::~Object()
{

};

XMFLOAT4X4 Object::GetWorldTransform()
{
	return mWorld;
};

void Object::SetLocation(int x, int z)
{
	mLocation.x = x;
	mLocation.z = z;
	XMMATRIX world = XMMatrixTranslation(static_cast<float>(x), 0.0f, static_cast<float>(z));
	XMStoreFloat4x4(&mWorld, world);
};

void Object::UpdateLocation(int x, int z)
{
	mLocation.x += x;
	mLocation.z += z;
	mWorld(3, 0) += static_cast<float>(x);
	mWorld(3, 2) += static_cast<float>(z);
};

Location2D Object::GetLocation()
{
	return mLocation;
};

QuickDef::MoveType Object::GetMoveType()
{
	return mMoveType;
};

QuickDef::Player Object::GetPlayer()
{
	return mFaction;
};

QuickDef::Type Object::GetObjectType()
{
	return mType;
};

bool Object::HasSpecial()
{
	return mSpecialMove;
};

void Object::EndSpecial()
{
	mSpecialMove = false;
};

void Object::MakeSelected()
{
	mSelected = true;
	mColorHighlight = reinterpret_cast<float*>(&(XMVectorSet(0.0f, 0.3f, 0.3f, 0.0f)));
};

void Object::Deselect()
{
	mSelected = false;
	mColorHighlight = reinterpret_cast<float*>(&(XMVectorZero()));
};

//*****************************
//		King definitions
//****************************
King::King(QuickDef::Player player, int x, int z)
	:Object(player, QuickDef::King, QuickDef::Piece, x, z)
{
	mSpecialMove = true;
	mWorld._42 = 0.6f;
};


//***************************
//		Queen definitions
//**************************

Queen::Queen(QuickDef::Player player, int x, int z)
	:Object(player, QuickDef::Queen, QuickDef::Piece, x, z)
{
	mWorld._42 = 0.6f;
};


//***************************
//		Knight definitions
//***************************

Knight::Knight(QuickDef::Player player, int x, int z)
	:Object(player, QuickDef::Knight, QuickDef::Piece, x, z)
{
	mWorld._42 = 0.35f;
};

//*******************************
//		Bishop definitions
//*******************************

Bishop::Bishop(QuickDef::Player player, int x, int z)
	:Object(player, QuickDef::Bishop, QuickDef::Piece, x, z)
{
	mWorld._42 = 0.6f;
};

//******************************
//		Rook definitions
//*********************************

Rook::Rook(QuickDef::Player player, int x, int z)
	:Object(player, QuickDef::Rook, QuickDef::Piece, x, z)
{
	mSpecialMove = true;
	mWorld._42 = 0.475f;
};

//******************************
//		Pawn definitions
//********************************

Pawn::Pawn(QuickDef::Player player, int x, int z)
	:Object(player, QuickDef::Pawn, QuickDef::Piece, x, z)
{
	mSpecialMove = true;
	mWorld._42 = 0.225f;
};

//*********************************
//		Board definitions
//*********************************

Board::Board(QuickDef::Player player, int x, int z)
	:Object(player, QuickDef::None, QuickDef::Board, x, z)
{
	if (player == QuickDef::Red)
	{
		mColorBase = reinterpret_cast<float*>(&(XMVectorSet(0.451f, 0.218f, 0.074f, 1.0f)));
	}
	else
	{
		mColorBase = reinterpret_cast<float*>(&(XMVectorSet(0.01f, 0.01f, 0.01f, 1.0f)));
	}

};

//*************************************
//		Menu definitions
//*************************************

Menu::Menu(float xOffset, float yOffset, MenuDef::MenuType type)
	:mMenuType(type)
{
	mWorld = reinterpret_cast<float*>(&(XMMatrixTranslation(xOffset, yOffset -0.75f, 0.0f)));
};

Menu::~Menu()
{

};

MenuDef::MenuType Menu::GetMenuAction()
{
	return mMenuType;
};

XMMATRIX Menu::GetWorldTransform()
{
	return reinterpret_cast<float*>(&(mWorld));
};







