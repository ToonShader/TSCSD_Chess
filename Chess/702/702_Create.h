//***********************************************************************************************
//***********************************************************************************************
//							Componet Framework of Engine 702
//
//							Initilization of D3D and Win32
//
//*******************************  All Rights Reserved  *****************************************
//***********************************************************************************************

#ifndef CREATE702_H
#define CREATE702_H

#include "GameTimer.h"
#include "702_Util.h"
#include <string>
//#include <cstring>
//#include <cstdlib>

class D3DAppControl
{

public:
	D3DAppControl(HINSTANCE hInstance);
	virtual ~D3DAppControl();

	HINSTANCE	getInstance()const;
	HWND		getMainWndHandle()const;
	float		getAspectRatio()const;

	int Run();


	virtual bool Init();
	virtual void OnResize();
//	virtual void UpdateScene(float deltaTime) = 0;
//	virtual void DrawScene() = 0;
	virtual LRESULT MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;//do not forget to edit


	//virtual void OnMouseDown(WPARAM btnState, int x, int y) { }
	//virtual void OnMouseUp (WPARAM btnState, int x, int y)  { }//------------------------------------------------------------
	//virtual void OnMouseMove(WPARAM btnState, int x, int y) { }



//	UINT CheckMsaaQuality();//-------virtual?? seems to not exist

protected:
	bool InitMainWind();
	bool InitDirect3D();

	void CalculateFrameStats();


protected:
	HINSTANCE	mhAppInstance;
	HWND		mhMainWnd;
	bool		mAppPaused;
	bool		mInactive;
	bool		mResizing;
	bool		mMinimized;
	bool		mMaximized;
	UINT		m4xMsaaQuality;

	GameTimer mTimer;

	ID3D11Device*				md3dDevice;//these done in Init
	ID3D11DeviceContext*		md3dImmediateContext;//
	IDXGISwapChain*				mSwapChain;
	ID3D11Texture2D*			mDepthStencilBuffer;
	ID3D11RenderTargetView*		mRenderTargetView;//one without view?
	ID3D11DepthStencilView*		mDepthStencilView;
	D3D11_VIEWPORT				mScreenViewport;

	// variables child class should override
	std::wstring	mMainWndCaption;// is std:: already declared
	//wstring blank;
	D3D_DRIVER_TYPE	md3dDriverType;
	int				mClientHeight;
	int				mClientWidth;
	bool			mEnable4xMsaa;


};

#endif // defined CREATE702_H

//D3D_FEATURE_LEVEL mfeatureLevel; /*where dooes this go?------------------------ */

//***********************************************************************************************
//***********************************  Questions  ***********************************************
//1. 
//2.
//3.
//***********************************************************************************************
//***********************************************************************************************