//***********************************************************************************************
//***********************************************************************************************
//							Componet Framework of Engine 702
//
//							Initilization of D3D and Win32
//
//*******************************  All Rights Reserved  *****************************************
//***********************************************************************************************

#include "702_Create.h"
#include <WindowsX.h>
#include <sstream>// look for .str() and other string swap items
//#include <string>


namespace//perhaps a way to do this without namespace, though it may allow it to be used outside the file
{
	D3DAppControl* h_d3dAppCtrl = 0;
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)//still sketchy on mutiple wind procs/msg procs?
{
	return h_d3dAppCtrl->MsgProc(hWnd, Msg, wParam, lParam);
}

D3DAppControl::D3DAppControl(HINSTANCE hInstance)
:	mhAppInstance(hInstance),
	mhMainWnd(0),
	mAppPaused(false),
	mInactive(false),
	mResizing(false),
	mMinimized(false),
	mMaximized(false),
	m4xMsaaQuality(0),

	md3dDevice(0),
	md3dImmediateContext(0),
	mSwapChain(0),
	mDepthStencilBuffer(0),
	mDepthStencilView(0),
	mRenderTargetView(0),

	mMainWndCaption(L"Chess"),
	md3dDriverType(D3D_DRIVER_TYPE_HARDWARE),
	mClientHeight(600),
	mClientWidth(800),
	mEnable4xMsaa(false)
{
	ZeroMemory(&mScreenViewport, sizeof(D3D11_VIEWPORT));
	h_d3dAppCtrl = this;
}

D3DAppControl::~D3DAppControl()
{
	ReleaseCOM(md3dDevice);

	if (md3dImmediateContext)//----------------------------------why do we restore default settings
		md3dImmediateContext->ClearState();

	ReleaseCOM(md3dImmediateContext);
	ReleaseCOM(mSwapChain);
	ReleaseCOM(mDepthStencilBuffer);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mRenderTargetView);
}

HINSTANCE D3DAppControl::getInstance() const
{
	return mhAppInstance;
}

HWND D3DAppControl::getMainWndHandle() const
{
	return mhMainWnd;
}

float D3DAppControl::getAspectRatio() const
{
	return static_cast<float>(mClientWidth) / mClientHeight;
}

int D3DAppControl::Run()
{
	MSG Msg = {0}; // same as ZeroMemory() macro??-------

	mTimer.Reset();

	while (Msg.message != WM_QUIT)
	{
		if (PeekMessage(&Msg, 0, 0, 0, PM_REMOVE))//second param allows checks for messages of a specific window --- thread dependent?
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		else
		{
			mTimer.Tick();

			if (!mAppPaused)
			{
				CalculateFrameStats();
//				UpdateScene(mTimer.DeltaTime());
//				DrawScene();
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return (int)Msg.wParam;
}

bool D3DAppControl::Init()
{
	if (! InitMainWind())
		return false;

	if (! InitDirect3D())
		return false;

	return true;
}

void D3DAppControl::OnResize()
{
	assert(md3dDevice);
	assert(md3dImmediateContext);
	assert(mSwapChain);

	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mDepthStencilBuffer);
	ReleaseCOM(mDepthStencilView);

	HR(mSwapChain->ResizeBuffers(1, mClientWidth, mClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));

	ID3D11Texture2D* backBuffer;
	HR(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));

	HR(md3dDevice->CreateRenderTargetView(backBuffer, 0, &mRenderTargetView));
	ReleaseCOM(backBuffer);


	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width		= mClientWidth;
	depthStencilDesc.Height		= mClientHeight;
	depthStencilDesc.MipLevels	= 1;
	depthStencilDesc.ArraySize	= 1;
	depthStencilDesc.Format		= DXGI_FORMAT_D24_UNORM_S8_UINT;

	if ( mEnable4xMsaa )
	{
		depthStencilDesc.SampleDesc.Count	= 4;
		depthStencilDesc.SampleDesc.Quality	= m4xMsaaQuality - 1;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count	= 1;
		depthStencilDesc.SampleDesc.Quality	= 0;
	}

	depthStencilDesc.Usage			= D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags		= D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags	= 0;
	depthStencilDesc.MiscFlags		= 0;

	HR(md3dDevice->CreateTexture2D(&depthStencilDesc, 0, &mDepthStencilBuffer));
	HR(md3dDevice->CreateDepthStencilView(mDepthStencilBuffer, 0, &mDepthStencilView));


	md3dImmediateContext->OMSetRenderTargets( 1, &mRenderTargetView, mDepthStencilView);


	mScreenViewport.TopLeftX	= 0;
	mScreenViewport.TopLeftY	= 0;
	mScreenViewport.Width		= static_cast<float>(mClientWidth);
	mScreenViewport.Height		= static_cast<float>(mClientHeight);
	mScreenViewport.MinDepth	= 0.0f;
	mScreenViewport.MaxDepth	= 1.0f;

	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);
}

/*LRESULT D3DAppControl::MsgProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch ( Msg )
	{
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	// end user input processes

	case WM_ACTIVATE:
		if ( LOWORD(wParam) == WA_INACTIVE )
		{
			mInactive = true;
			mTimer.Stop();
		}
		else
		{
			mInactive = false;
			mTimer.Start();
		}
		return 0;

	case WM_SIZE:
		mClientWidth	= LOWORD(lParam);
		mClientHeight	= HIWORD(lParam);

		if ( md3dDevice && mSwapChain )
		{
			if ( wParam == SIZE_MINIMIZED )
			{
				mInactive	= true;
				mMinimized	= true;
				mMaximized	= false;
			}
			else if ( wParam == SIZE_MAXIMIZED )
			{
				mInactive	= false;
				mMinimized	= false;
				mMaximized	= true;
				OnResize();
			}
			else if ( wParam == SIZE_RESTORED )
			{
				if ( mMinimized )
				{
					mInactive	= false;
					mMinimized	= false;
					OnResize();
				}
				else if ( mMaximized )
				{
					mInactive	= false;
					mMaximized	= false;
					OnResize();
				}
				else if ( mResizing )
				{
					mInactive = true;
				}
				else
				{
					OnResize();
				}
			}
		}
		return 0;

	case WM_ENTERSIZEMOVE:
		mInactive = true;
		mResizing = true;
		return 0;

	case WM_EXITSIZEMOVE:
		mInactive = false;
		mResizing = false;
		OnResize();
		return 0;

	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);// --------------------------------------------------curious about actual purpose

	case WM_GETMINMAXINFO:
		(reinterpret_cast<MINMAXINFO*>(lParam))->ptMinTrackSize.x	= 800;//--------------------------changed casts (minmaxinfo*)
		(reinterpret_cast<MINMAXINFO*>(lParam))->ptMinTrackSize.y	= 600;
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, Msg, wParam, lParam);
}*/

bool D3DAppControl::InitMainWind()
{

	//HINSTANCE instance = GetModuleHandle(NULL);
	WNDCLASS wc;
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= MainWndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= mhAppInstance;
	wc.hIcon			= LoadIcon(0, IDI_WINLOGO);
	wc.hCursor			= LoadCursor (0, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName		= 0;
	wc.lpszClassName	= L"myclass";

	if ( !RegisterClass(&wc) )
	{
		MessageBox(0, L"RegisterClass FAILED.", 0, 0);
		return false;
	}

	/*RECT R = {0, 0, mClientWidth, mClientHeight};
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);*///remove from final build?
	int width = mClientWidth; //R.right - R.left;
	int height = mClientHeight; //R.bottom - R.top;

	mhMainWnd = CreateWindow(L"myclass", mMainWndCaption.c_str(),
		WS_OVERLAPPEDWINDOW, 500, 500, width, height, 0, 0, mhAppInstance, 0);//----compress these for ch limit reasons?

	if ( !mhMainWnd )
	{
		MessageBox(0, L"CreateWindow FAILED", 0, 0);

		/*DWORD Error = GetLastError();
		char ErrorBuffer[ 1024 ];
		wsprintf( reinterpret_cast<LPWSTR>(ErrorBuffer), L"Error creating window. Error code, decimal %d, hexadecimal %X.", Error, Error );
		MessageBox( NULL, reinterpret_cast<LPWSTR>(ErrorBuffer), L"Error", MB_ICONHAND );*///-----------------------------------------------------interesting code, review
	
		return false;
	}

	ShowWindow(mhMainWnd, SW_SHOW);//----they seem slightly required or proper messages arent handled
	UpdateWindow(mhMainWnd);//---------------------are these really necessary, does createWindow not send out the proper messages for thiis?

	return true;
}

bool D3DAppControl::InitDirect3D()
{
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;
	D3D_FEATURE_LEVEL input = D3D_FEATURE_LEVEL_10_1;
	HRESULT hr = D3D11CreateDevice(0, md3dDriverType, 0, createDeviceFlags, &input, 1, D3D11_SDK_VERSION, &md3dDevice, &featureLevel, &md3dImmediateContext);

	if (FAILED(hr))
	{
		MessageBox(0, L"CreateDevice FAILED", 0, 0);
		//return false;
	}
	if ( featureLevel != D3D_FEATURE_LEVEL_10_1)
	{
		MessageBox(0, L"D3D11 Not Supported", 0, 0);
		//return false;
	}

	HR(md3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality));
	assert(m4xMsaaQuality > 0);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferDesc.Width						= mClientWidth;
	sd.BufferDesc.Height					= mClientHeight;
	sd.BufferDesc.RefreshRate.Numerator		= 60;
	sd.BufferDesc.RefreshRate.Denominator	= 1;
	sd.BufferDesc.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;

	if ( 0 )
	{
		sd.SampleDesc.Count		= 4;
		sd.SampleDesc.Quality	= m4xMsaaQuality - 1;
	}
	else
	{
		sd.SampleDesc.Count		= 1;
		sd.SampleDesc.Quality	= 0;
	}

	sd.BufferUsage	= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount	= 1;
	sd.OutputWindow	= mhMainWnd;
	sd.Windowed		= false;//-----------------------------------------may cause issues with relation to client area vs window area line 130 no swap chain
	sd.SwapEffect	= DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags		= 0;

	IDXGIDevice* dxgiDevice = 0;
	HR(md3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice)));

	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgiAdapter)));// proper cast replaces (void**)

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&dxgiFactory)));

	dxgiFactory->CreateSwapChain(md3dDevice, &sd, &mSwapChain);


	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);

	OnResize();

	return true;
}

void D3DAppControl::CalculateFrameStats()
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	++frameCnt;

	float currTime = (mTimer.TotalTime() - timeElapsed);

	if ( currTime >= 1.0f )
	{
		float fps = static_cast<float>(frameCnt);//------------------------return/remove cast
		float mspf = 1000.0f / fps;

		std::wostringstream ws;
		ws.precision(6);
		ws << mMainWndCaption   << L"    "
		   << L"FPS: "  << fps  << L"    "
		   << L"MSPF: " << mspf << L" (ms)";

		SetWindowText(mhMainWnd, ws.str().c_str());



		frameCnt = 0;
		timeElapsed += currTime;
	}
}

//***********************************************************************************************
//***********************************  Questions  ***********************************************
//1. DONT FORGET OTHER NEEDED MACROS
//2. float / int results in float? check relations of such assosiations--see getapsectratio()
//3. look up mSwapChain->getbuffer() why the uuidof and void**
//4. pointers to functions??
//5. check create device function and its relation to feature levels.
//6. REMOVE C STYLE CASTS, EDIT FPS?
//***********************************************************************************************
//***********************************************************************************************