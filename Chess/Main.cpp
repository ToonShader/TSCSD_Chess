#include "Chess.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{

#if defined (DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Chess chessGame(hInstance);
	if ( ! chessGame.Init())
	{
		// TODO: handle init error
	}
	else
		return chessGame.Run();

}