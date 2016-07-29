//***********************************************************************************************
//***********************************************************************************************
//							Componet Framework of Engine 702
//
//							Initilization of D3D and Win32
//
//*******************************  All Rights Reserved  *****************************************
//***********************************************************************************************

#pragma once

#include "GameTimer.h"
#include "702_Util.h"
#include <string>

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
	virtual LRESULT MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;// TODO: Check: do not forget to edit

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

	ID3D11Device*				md3dDevice;
	ID3D11DeviceContext*		md3dImmediateContext;
	IDXGISwapChain*				mSwapChain;
	ID3D11Texture2D*			mDepthStencilBuffer;
	ID3D11RenderTargetView*		mRenderTargetView;// TODO: Check for one without view
	ID3D11DepthStencilView*		mDepthStencilView;
	D3D11_VIEWPORT				mScreenViewport;

	// variables child class should override
	std::wstring	mMainWndCaption;

	D3D_DRIVER_TYPE	md3dDriverType;
	int				mClientHeight;
	int				mClientWidth;
	bool			mEnable4xMsaa;


};

//***********************************************************************************************
//***********************************  Questions  ***********************************************
//1. 
//2.
//3.
//***********************************************************************************************
//***********************************************************************************************