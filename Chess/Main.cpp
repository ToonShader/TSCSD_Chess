//#include "702/702_Create.h"
//#include "Main.h"
#include "Chess.h"
//#include "GameView.h"
//#include "GameLogic.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{

#if defined (DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Chess chessGame(hInstance);
	if ( ! chessGame.Init())
	{
		//something
	}
	else
		return chessGame.Run();

	//if ( ! thisapp.Init() )
	//{
		//return 0;
	//}

	//thisapp.NoFullscreen();
	//return thisapp.Run();

}