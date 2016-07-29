


#include "GameView.h"
#include <float.h>
#include <ctime>
#include <cstdlib>



//*************************************
//		vertex definitions
//*************************************

Vertex::Vertex()
{

};

Vertex::Vertex(XMFLOAT3 pos, XMFLOAT3 normal)
{
	Pos = pos;
	Normal = normal;
};

Vertex::Vertex(const GeometryGenerator::Vertex& other)
{
	this->Pos = other.Position;
	this->Normal = other.Normal;
};

//*************************************
//		RenderTarget Definitions
//*************************************


RenderTarget::RenderTarget(HINSTANCE hInstance, GameLogic& gameLogic)
	:D3DAppControl(hInstance),  mMenued(false), mTestVertexBuffer(nullptr), mTestIndexBuffer(nullptr), mMenuListCount(0), mMenuList(nullptr), 
	mWireFrame(false), mMenuCloseable(true), mSelectedMenu(MenuDef::None), mGameLogic(nullptr), mRankingPlayer(QuickDef::NoPlayer),
	mFXWorld(nullptr), mFXWorldInvTranspose(nullptr), mFXEyePosW(nullptr), mFXDirLight(nullptr), mFXPointLight(nullptr), mFXSpotLight(nullptr), 
	mFXMaterial(nullptr)
{
		mGameLogic = &gameLogic;
		XMMATRIX I = XMMatrixIdentity();
		XMStoreFloat4x4 (&mView, I);
		XMStoreFloat4x4 (&mProj, I);
		
		XMStoreFloat4(&mColorHighlight, XMVectorSet(0.0f, 0.2f, 0.2f, 0.0f));
		XMStoreFloat4(&mEyePosW, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		SetMenuLevel(MenuDef::Main);

		XMFLOAT4 EmptyColor(0.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR lPos = XMVectorSet(9.0f, 5.0f, 3.5f, 1.0f);
		XMVECTOR l2Pos = XMVectorSet(-9.0f, 6.0f, 3.5f, 1.0f);
		XMVECTOR bMid = XMVectorSet(3.5f, 0.0f, 3.5f, 1.0f);

		mDirLight.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		mDirLight.Diffuse = XMFLOAT4(1.5f, 1.5f, 1.5f, 1.0f);
		mDirLight.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
		XMStoreFloat3(&mDirLight.Direction, XMVector3Normalize(bMid - l2Pos));

		XMStoreFloat3(&mPointLight.Position, lPos);
		mPointLight.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		mPointLight.Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		mPointLight.Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
		mPointLight.Att = XMFLOAT3(1.0f, 0.0f, 0.0f);
		mPointLight.Range = 1000.0f;

		mSpotLight.Ambient = EmptyColor;
		mSpotLight.Diffuse = EmptyColor;
		mSpotLight.Specular = EmptyColor;
		mSpotLight.Att = XMFLOAT3(1.0f, 1.0f, 0.0f);
		mSpotLight.SpotFactor = 96.0f;
		mSpotLight.Range = 10000.0f;

		mPieceMat.Ambient = XMFLOAT4(0.45f, 0.45f, 0.45f, 1.0f);
		mPieceMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		mPieceMat.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 64.0f);

		mBoardMat.Ambient = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);
		mBoardMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		mBoardMat.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 16.0f);

		mMenuMat.Ambient = XMFLOAT4(1000.0f, 1000.0f, 1000.0f, 1.0f);
		mMenuMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		mMenuMat.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// TODO: Reflect values?


		// XMStoreFloat4(&mViewDisplace, XMVectorSet(3.5f, 0.0f, 3.5f, 0.0f));
};

RenderTarget::~RenderTarget()
{
	md3dImmediateContext->ClearState();
	ReleaseCOM(mObjectsVertexBuffer);
	ReleaseCOM(mObjectsIndexBuffer);
	ReleaseCOM(mMenuVertexBuffer);
	ReleaseCOM(mMenuIndexBuffer);
	//debug
	ReleaseCOM(mDebugVertexBuffer);
	ReleaseCOM(mDebugIndexBuffer);

	ReleaseCOM(mInputLayout);
	ReleaseCOM(mWireFrameRS);
	ReleaseCOM(mFX);

	if (mMenuList)
	{
		for (UINT i = 0; i < mMenuListCount; ++i)
			delete mMenuList[i];

		delete[] mMenuList;
	}
};

LRESULT RenderTarget::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			if (! mMenued)
			{
				mMenued = true;
				SetMenuLevel(MenuDef::Main);
			}
			else
			{
				if (mMenuCloseable)
					mMenued = false;
			}

			return 0;

		case VK_F1:
			if (mWireFrame)
				mWireFrame = false;
			else
				mWireFrame = true;
			return 0;
		}
		break;

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
		return MAKELRESULT(0, MNC_CLOSE);// TODO: Learn: curious about actual purpose

	case WM_GETMINMAXINFO:
		(reinterpret_cast<MINMAXINFO*>(lParam))->ptMinTrackSize.x	= 800;
		(reinterpret_cast<MINMAXINFO*>(lParam))->ptMinTrackSize.y	= 600;
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	if (mGameLogic)
		return mGameLogic->MsgProc(hWnd, msg, wParam, lParam);
	else
		return DefWindowProc(hWnd, msg, wParam, lParam);// TODO: Determine: issue??????????????
};

bool RenderTarget::Init()
{
	if (! D3DAppControl::Init())
		return false;

	BuildGeometryBuffers();
	BuildMenuBuffers();
	BuildFX();
	BuildVertexLayout();

	D3D11_RASTERIZER_DESC wireframeDesc;
	ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_BACK;
	wireframeDesc.FrontCounterClockwise = false;
	wireframeDesc.DepthClipEnable = true;

	HR(md3dDevice->CreateRasterizerState(&wireframeDesc, &mWireFrameRS));
	
	return true;
};

void RenderTarget::SetViewProjection(CXMMATRIX viewProj)
{

};

Object* RenderTarget::ObjectPick(int sx, int sy, CXMMATRIX view, ObjectInfo& objects)
{
	float vx = (+2.0f*sx/mClientWidth  - 1.0f)/mProj(0,0);
	float vy = (-2.0f*sy/mClientHeight + 1.0f)/mProj(1,1);

	XMVECTOR rayOrigin	= XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR rayDir		= XMVectorSet(vx, vy, 1.0f, 0.0f);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);

	if (mMenued)
	{
		vx = (+2.0f*sx/mClientWidth  - 1.0f);
		vy = (-2.0f*sy/mClientHeight + 1.0f);
		rayOrigin	= XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f);
		rayDir		= XMVectorSet(vx, vy, 1.0f, 0.0f);

		//world transform, get/set location, getaction, color base/highlight, ID, 

		for (UINT i = 0; i < mMenuListCount; ++i)
		{
			XMMATRIX world = mMenuList[i]->GetWorldTransform();
			world._42 += 0.75f;

			XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(world), world);

			XMMATRIX toLocal = XMMatrixMultiply(XMMatrixIdentity(), invWorld);

			XMVECTOR tRayOrigin = rayOrigin;
			tRayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);

			XMVECTOR tRayDir = rayDir;
			tRayDir = XMVector3TransformNormal(rayDir, toLocal);
			tRayDir = XMVector3Normalize(tRayDir);

			float f = 0.0f;
			if (XNA::IntersectRayAxisAlignedBox(tRayOrigin, tRayDir, &mMenuBox, &f))
			{
				MenuProcess(mMenuList[i]);
				return 0;
			}
		}
		return 0;
	}

	UINT numObjects = (objects.numBlack + objects.numBoards + objects.numRed);
	Object** fullObjects = new Object*[numObjects];
	int k = 0;
	for (UINT i = 0; i < objects.numBoards; ++i, ++k)
	{
		fullObjects[k] = objects.BoardObjects[i];
	}
	for (UINT i = 0; i < objects.numBlack; ++i, ++k)
	{
		fullObjects[k] = objects.BlackObjects[i];
	}
	for (UINT i = 0; i < objects.numRed; ++i, ++k)
	{
		fullObjects[k] = objects.RedObjects[i];
	}

	Object* picked = 0;
	float minDistance = FLT_MAX;

	//main picking loop
	for (UINT o = 0; o < numObjects; ++o)
	{
		XMMATRIX world = XMLoadFloat4x4(&fullObjects[o]->GetWorldTransform());
		XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(world), world);

		XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

		XMVECTOR tRayOrigin = rayOrigin;
		tRayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);

		XMVECTOR tRayDir = rayDir;
		tRayDir = XMVector3TransformNormal(rayDir, toLocal);
		tRayDir = XMVector3Normalize(tRayDir);

		XNA::AxisAlignedBox* currBox = 0;
		Mesh& currMesh = GetMeshInfo(fullObjects[o]->GetMoveType(), &currBox);

		float fake = 0.0f;
		if (XNA::IntersectRayAxisAlignedBox(tRayOrigin, tRayDir, currBox, &fake))
		{
			for (UINT i = 0; i < currMesh.Indices.size()/3; ++i)
			{
				UINT I0 = currMesh.Indices[i*3 + 0];
				UINT I1 = currMesh.Indices[i*3 + 1];
				UINT I2 = currMesh.Indices[i*3 + 2];

				XMVECTOR V0 = XMLoadFloat3(&currMesh.Vertices[I0].Pos);
				XMVECTOR V1 = XMLoadFloat3(&currMesh.Vertices[I1].Pos);
				XMVECTOR V2 = XMLoadFloat3(&currMesh.Vertices[I2].Pos);

				float currDistance = 0.0f;
				if (XNA::IntersectRayTriangle(tRayOrigin, tRayDir, V0, V1, V2, &currDistance))
				{
					if (currDistance < minDistance)
					{
						minDistance = currDistance;
						picked = fullObjects[o];
					}
				}
			}
		}

	}
	delete fullObjects;
	return picked;
};

void RenderTarget::OnResize()
{
	D3DAppControl::OnResize();

	XMMATRIX p = XMMatrixPerspectiveFovLH(0.25f * XM_PI, getAspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, p);
};

MenuDef::MenuType RenderTarget::GetMenuItem()
{
	if (mSelectedMenu != MenuDef::None)
	{
		MenuDef::MenuType temp = mSelectedMenu;
		mSelectedMenu = MenuDef::None;
		return temp;
	}
	else
		return MenuDef::None;
}

void RenderTarget::MenuOpen(QuickDef::Player player, MenuDef::MenuLevel menuLevel)
{
	mRankingPlayer = player;
	mMenued = true;
	mMenuCloseable = false;
	if (mMenuLevel != MenuDef::Won)
		SetMenuLevel(menuLevel);
};

void RenderTarget::MenuClose()
{
	if (mMenuLevel == MenuDef::Won)
		return;

	mMenued = false;
	mMenuCloseable = true;
};

void RenderTarget::UpdateScene(float deltaTime, CXMVECTOR eyePos, CXMMATRIX viewProj)
{
	XMStoreFloat4(&mEyePosW, eyePos);
	XMStoreFloat4x4(&mView, viewProj);
};

void RenderTarget::DrawScene(ObjectInfo& objects)
{
	assert(mSwapChain);
	assert(md3dImmediateContext);
	
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Black));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	// TODO: Determine: must it be set each call?
	md3dImmediateContext->IASetInputLayout(mInputLayout);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (mWireFrame)
		md3dImmediateContext->RSSetState(mWireFrameRS);

	else
		md3dImmediateContext->RSSetState(0);


	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mObjectsVertexBuffer, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mObjectsIndexBuffer, DXGI_FORMAT_R32_UINT, offset);

	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX viewProj = view*proj;

	mFXDirLight->SetRawValue(&mDirLight, 0, sizeof(mDirLight));
	mFXPointLight->SetRawValue(&mPointLight, 0, sizeof(mPointLight));
	mFXSpotLight->SetRawValue(&mSpotLight, 0, sizeof(mSpotLight));
	mFXEyePosW->SetFloatVector(reinterpret_cast<float*>(&mEyePosW));

	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)// TODO: Learn: can a float 4x4 be used to fill worldviewproj->setmatrix
	{
		XMMATRIX world = XMMatrixIdentity();

		if (mMenued)
		{
			md3dImmediateContext->IASetVertexBuffers(0, 1, &mMenuVertexBuffer, &stride, &offset);
			md3dImmediateContext->IASetIndexBuffer(mMenuIndexBuffer, DXGI_FORMAT_R32_UINT, offset);
			md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

			mFXMaterial->SetRawValue(&mMenuMat, 0, sizeof(mMenuMat));

			for (UINT i = 0; i < mMenuListCount; ++i)
			{
				UINT currVertexOffset;
				UINT currIndexOffset;
				UINT currIndexCount;
				GetDrawInfo(currVertexOffset, currIndexOffset, currIndexCount, 0, mMenuList[i]);

				world = mMenuList[i]->GetWorldTransform();
				mFXWorld->SetMatrix(reinterpret_cast<float*>(&world));
				mFXWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&MathHelper::InverseTranspose(world)));
				mFXWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world)));

				mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
				md3dImmediateContext->DrawIndexed(currIndexCount, currIndexOffset, currVertexOffset);

				world._42 += 0.75f;
				mFXWorld->SetMatrix(reinterpret_cast<float*>(&world));
				mFXWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&MathHelper::InverseTranspose(world)));
				mFXWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world)));
				mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
				md3dImmediateContext->DrawIndexed(mMenuBoxIndexCount, mMenuBoxIndexOffset, mMenuBoxVertexOffset);
			}	
		}

		//***********************
		//	debug drawing
		//***********************
		md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mDebugVertexBuffer, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mDebugIndexBuffer, DXGI_FORMAT_R32_UINT, offset);

		world = XMMatrixTranslation(9.0f, 5.0f, 3.5f);

		mFXWorld->SetMatrix(reinterpret_cast<float*>(&world));
		mFXWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&MathHelper::InverseTranspose(world)));
		mFXWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world * viewProj)));

		Material debugMat;
		debugMat.Ambient = XMFLOAT4(1000.0f, 0.0f, 0.0f, 1.0f);
		debugMat.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		debugMat.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		debugMat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

		mFXMaterial->SetRawValue(&debugMat, 0, sizeof(debugMat));

		mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(2, 0, 0);

		debugMat.Ambient = XMFLOAT4(0.0f, 1000.0f, 0.0f, 0.0f);
		mFXMaterial->SetRawValue(&debugMat, 0, sizeof(debugMat));

		mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(2, 2, 0);

		debugMat.Ambient = XMFLOAT4(0.0f, 0.0f, 1000.0f, 0.0f);
		mFXMaterial->SetRawValue(&debugMat, 0, sizeof(debugMat));

		mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(2, 4, 0);
		//***********************
		//	end debug
		//***********************

		md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mObjectsVertexBuffer, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mObjectsIndexBuffer, DXGI_FORMAT_R32_UINT, offset);

		for (UINT i = 0; i < objects.numBoards; ++i)
		{
			world = XMLoadFloat4x4(& objects.BoardObjects[i]->GetWorldTransform()); //possible issue using a return
			Material mat = mBoardMat;
			XMVECTOR color = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
			color += XMLoadFloat4(&objects.BoardObjects[i]->mColorBase);
			color += XMLoadFloat4(&objects.BoardObjects[i]->mColorHighlight);
			XMStoreFloat4(&mat.Ambient, color * XMLoadFloat4(&mat.Ambient));
			XMStoreFloat4(&mat.Diffuse, color * XMLoadFloat4(&mat.Diffuse));
			//XMStoreFloat4(&mat.Specular, color * XMLoadFloat4(&mat.Specular));


			mFXWorld->SetMatrix(reinterpret_cast<float*>(&world));
			mFXWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&MathHelper::InverseTranspose(world)));
			mFXWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world * viewProj)));
			mFXMaterial->SetRawValue(&mat, 0, sizeof(mat));

			mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(mBoardIndexCount, mBoardIndexOffset, mBoardVertexOffset);
		}

		for (UINT i = 0; i < objects.numBlack; ++i)
		{
			UINT currVertexOffset;
			UINT currIndexOffset;
			UINT currIndexCount;

			GetDrawInfo(currVertexOffset, currIndexOffset, currIndexCount, objects.BlackObjects[i], 0);//possible issue
			
			world = XMLoadFloat4x4(& objects.BlackObjects[i]->GetWorldTransform());
			Material mat = mPieceMat;
			XMVECTOR color = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
			color += XMLoadFloat4(&objects.BlackObjects[i]->mColorBase);
			color += XMLoadFloat4(&objects.BlackObjects[i]->mColorHighlight);
			XMStoreFloat4(&mat.Ambient, color * XMLoadFloat4(&mat.Ambient));
			XMStoreFloat4(&mat.Diffuse, color * XMLoadFloat4(&mat.Diffuse));
			//XMStoreFloat4(&mat.Specular, color * XMLoadFloat4(&mat.Specular));

			mFXWorld->SetMatrix(reinterpret_cast<float*>(&world));
			mFXWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&MathHelper::InverseTranspose(world)));
			mFXWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world * viewProj)));
			mFXMaterial->SetRawValue(&mat, 0, sizeof(mat));

			mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(currIndexCount, currIndexOffset, currVertexOffset);
		}

		for (UINT i = 0; i < objects.numRed; ++i)
		{
			UINT currVertexOffset;
			UINT currIndexOffset;
			UINT currIndexCount;

			GetDrawInfo(currVertexOffset, currIndexOffset, currIndexCount, objects.RedObjects[i], 0);

			world = XMLoadFloat4x4(&objects.RedObjects[i]->GetWorldTransform());
			Material mat = mPieceMat;
			XMVECTOR color = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
			color += XMLoadFloat4(&objects.RedObjects[i]->mColorBase);
			color += XMLoadFloat4(&objects.RedObjects[i]->mColorHighlight);
			XMStoreFloat4(&mat.Ambient, color * XMLoadFloat4(&mat.Ambient));
			XMStoreFloat4(&mat.Diffuse, color * XMLoadFloat4(&mat.Diffuse));
			//XMStoreFloat4(&mat.Specular, color * XMLoadFloat4(&mat.Specular));

			mFXWorld->SetMatrix(reinterpret_cast<float*>(&world));
			mFXWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&MathHelper::InverseTranspose(world)));
			mFXWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world * viewProj)));
			mFXMaterial->SetRawValue(&mat, 0, sizeof(mat));

			mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(currIndexCount, currIndexOffset, currVertexOffset);
		}
	}

	HR(mSwapChain->Present(1, 0));
};

void RenderTarget::GetDrawInfo(UINT& VertexOffset, UINT& IndexOffset, UINT& IndexCount, Object* object, Menu* menuItem)
{
	if (object)
	{
		switch (object->GetMoveType())
			{
			case QuickDef::King:
				VertexOffset = mKingVertexOffset;
				IndexOffset = mKingIndexOffset;
				IndexCount = mKingIndexCount;
				break;

			case QuickDef::Queen:
				VertexOffset = mQueenVertexOffset;
				IndexOffset = mQueenIndexOffset;
				IndexCount = mQueenIndexCount;
				break;

			case QuickDef::Knight:
				VertexOffset = mKnightVertexOffset;
				IndexOffset = mKnightIndexOffset;
				IndexCount = mKnightIndexCount;
				break;

			case QuickDef::Bishop:
				VertexOffset = mBishopVertexOffset;
				IndexOffset = mBishopIndexOffset;
				IndexCount = mBishopIndexCount;
				break;

			case QuickDef::Rook:
				VertexOffset = mRookVertexOffset;
				IndexOffset = mRookIndexOffset;
				IndexCount = mRookIndexCount;
				break;

			case QuickDef::Pawn:
				VertexOffset = mPawnVertexOffset;
				IndexOffset = mPawnIndexOffset;
				IndexCount = mPawnIndexCount;
				break;

			default:
				VertexOffset = 0;
				IndexOffset = 0;
				IndexCount = 0;
				break;
			}
	}
	else if (menuItem)
	{
		MenuDef::MenuType type = menuItem->GetMenuAction();

		switch(type)
		{
		case MenuDef::New:
			VertexOffset = mMenuNewVertexOffset;
			IndexOffset = mMenuNewIndexOffset;
			IndexCount = mMenuNewIndexCount;
			break;

		case MenuDef::Load:
			VertexOffset = mMenuLoadVertexOffset;
			IndexOffset = mMenuLoadIndexOffset;
			IndexCount = mMenuLoadIndexCount;
			break;

		case MenuDef::Save:
			VertexOffset = mMenuSaveVertexOffset;
			IndexOffset = mMenuSaveIndexOffset;
			IndexCount = mMenuSaveIndexCount;
			break;

		case MenuDef::Exit:
			VertexOffset = mMenuExitVertexOffset;
			IndexOffset = mMenuExitIndexOffset;
			IndexCount = mMenuExitIndexCount;
			break;

		case MenuDef::NewPlayer:
		case MenuDef::LoadPlayer:
			VertexOffset = mMenuPlayerVertexOffset;
			IndexOffset = mMenuPlayerIndexOffset;
			IndexCount = mMenuPlayerIndexCount;
			break;

		case MenuDef::NewReplay:
		case MenuDef::LoadReplay:
			VertexOffset = mMenuReplayVertexOffset;
			IndexOffset = mMenuReplayIndexOffset;
			IndexCount = mMenuReplayIndexCount;
			break;

		case MenuDef::Queen:
			VertexOffset = mMenuQueenVertexOffset;
			IndexOffset = mMenuQueenIndexOffset;
			IndexCount = mMenuQueenIndexCount;
			break;

		case MenuDef::Knight:
			VertexOffset = mMenuKnightVertexOffset;
			IndexOffset = mMenuKnightIndexOffset;
			IndexCount = mMenuKnightIndexCount;
			break;

		case MenuDef::Bishop:
			VertexOffset = mMenuBishopVertexOffset;
			IndexOffset = mMenuBishopIndexOffset;
			IndexCount = mMenuBishopIndexCount;
			break;

		case MenuDef::Rook:
			VertexOffset = mMenuRookVertexOffset;
			IndexOffset = mMenuRookIndexOffset;
			IndexCount = mMenuRookIndexCount;
			break;

		case MenuDef::Red:
			VertexOffset = mMenuRedVertexOffset;
			IndexOffset = mMenuRedIndexOffset;
			IndexCount = mMenuRedIndexCount;
			break;

		case MenuDef::Black:
			VertexOffset = mMenuBlackVertexOffset;
			IndexOffset = mMenuBlackIndexOffset;
			IndexCount = mMenuBlackIndexCount;
			break;

		case MenuDef::Win:
			VertexOffset = mMenuWinVertexOffset;
			IndexOffset = mMenuWinIndexOffset;
			IndexCount = mMenuWinIndexCount;
			break;

		default:
			VertexOffset = 0;
			IndexOffset = 0;
			IndexCount = 0;
			break;
		}
	}
};

Mesh& RenderTarget::GetMeshInfo(QuickDef::MoveType type, XNA::AxisAlignedBox** box)
{
	switch (type)
	{
	case QuickDef::King:
		*box = &mKingBox;
		return mKingMesh;

	case QuickDef::Queen:
		*box = &mQueenBox;
		return mQueenMesh;

	case QuickDef::Knight:
		*box = &mKnightBox;
		return mKnightMesh;

	case QuickDef::Bishop:
		*box = &mBishopBox;
		return mBishopMesh;

	case QuickDef::Rook:
		*box = &mRookBox;
		return mRookMesh;

	case QuickDef::Pawn:
		*box = &mPawnBox;
		return mPawnMesh;

	default:
		*box = &mBoardBox;
		return mBoardMesh;
	}
};

void RenderTarget::MenuProcess(Menu* menuItem)
{
	MenuDef::MenuType type = menuItem->GetMenuAction();

	switch(type)
	{
	case MenuDef::New:
		SetMenuLevel(MenuDef::PlayNew);
		break;
		
	case MenuDef::Load:
		SetMenuLevel(MenuDef::PlayLoad);
		break;

	case MenuDef::Save:
		mMenued = false;
		mGameLogic->SaveGame();
		break;

	case MenuDef::Exit:
		mSelectedMenu = MenuDef::Exit;
		break;

	case MenuDef::LoadPlayer:
	case MenuDef::NewPlayer:
	case MenuDef::LoadReplay:
	case MenuDef::NewReplay:
		mSelectedMenu = menuItem->GetMenuAction();
		mMenued = false;
		break;

	case MenuDef::Queen:
	case MenuDef::Knight:
	case MenuDef::Bishop:
	case MenuDef::Rook:
		Event promotion;
		if (type == MenuDef::Queen)
			promotion.Picked = new Queen(mRankingPlayer, 0,  0);
		else if (type == MenuDef::Knight)
			promotion.Picked = new Knight(mRankingPlayer, 0, 0);
		else if (type == MenuDef::Bishop)
			promotion.Picked = new Bishop(mRankingPlayer, 0, 0);
		else
			promotion.Picked = new Rook(mRankingPlayer, 0, 0);
		
		promotion.CurrPlayer = mRankingPlayer;
		promotion.Move = false;
		bool fake = mGameLogic->EventProcess(promotion);
		mMenued = false;
		mMenuCloseable = true;
		mRankingPlayer = QuickDef::NoPlayer;
	}
};

void RenderTarget::SetMenuLevel(MenuDef::MenuLevel level)
{
	for (UINT i = 0; i < mMenuListCount; ++i)
	{
		delete mMenuList[i];
	}
	mMenuListCount = 0;
	if (mMenuList)
	{
		delete mMenuList;
		mMenuList = 0;
	}
	switch (level)
	{
	case MenuDef::Main:
		mMenuList = new Menu*[4];
		mMenuListCount = 4;
		mMenuList[0] = new Menu(0.0f, 0.8f, MenuDef::New);
		mMenuList[1] = new Menu(0.0f, 0.6f, MenuDef::Load);
		mMenuList[2] = new Menu(0.0f, 0.4f, MenuDef::Save);
		mMenuList[3] = new Menu(0.0f, 0.2f, MenuDef::Exit);
		mMenuLevel = MenuDef::Main;
		break;

	case MenuDef::PlayLoad:
		mMenuList = new Menu*[2];
		mMenuListCount = 2;
		mMenuList[0] = new Menu(0.0f, 0.4f, MenuDef::LoadPlayer);
		mMenuList[1] = new Menu(0.0f, -0.4f, MenuDef::LoadReplay);
		mMenuLevel = MenuDef::PlayLoad;
		break;

	case MenuDef::PlayNew:
		mMenuList = new Menu*[2];
		mMenuListCount = 2;
		mMenuList[0] = new Menu(0.0f, 0.4f, MenuDef::NewPlayer);
		mMenuList[1] = new Menu(0.0f, -0.4f, MenuDef::NewReplay);
		mMenuLevel = MenuDef::PlayNew;
		break;

	case MenuDef::Ranked:
		mMenuList = new Menu*[4];
		mMenuListCount = 4;
		mMenuList[0] = new Menu(0.0f, 0.8f, MenuDef::Queen);
		mMenuList[1] = new Menu(0.0f, 0.6f, MenuDef::Knight);
		mMenuList[2] = new Menu(0.0f, 0.4f, MenuDef::Bishop);
		mMenuList[3] = new Menu(0.0f, 0.2f, MenuDef::Rook);
		mMenuLevel = MenuDef::Ranked;
		break;

	case MenuDef::Won:
		mMenuList = new Menu*[5];
		mMenuListCount = 5;
		if (mRankingPlayer == QuickDef::Black)
			mMenuList[0] = new Menu(-0.2f, 0.8f, MenuDef::Black);
		else
			mMenuList[0] = new Menu(-0.2f, 0.8f, MenuDef::Red);

		mMenuList[1] = new Menu(0.2f, 0.8f, MenuDef::Win);
		mMenuList[2] = new Menu(0.0f, 0.6f, MenuDef::New);
		mMenuList[3] = new Menu(0.0f, 0.4f, MenuDef::Load);
		mMenuList[4] = new Menu(0.0f, 0.2f, MenuDef::Save);
		mMenuLevel = MenuDef::Won;
		break;

	}
};

void RenderTarget::BuildBlankBuffer()
{
	ReleaseCOM(mTestIndexBuffer);
	ReleaseCOM(mTestVertexBuffer);
	std::ifstream bufferFile("Buffers/test.txt");

	UINT vCount;

	bufferFile >> vCount;

	std::vector<Vertex> vertices(vCount);

	XMVECTOR white = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

	for (UINT i = 0; i < vCount; ++i)
	{
		bufferFile >> vertices[i].Pos.x >> vertices[i].Pos.y;
		vertices[i].Pos.z = 0.0f;
		vertices[i].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		//vertices[i].Color = reinterpret_cast<float*>(&white);
	}

	D3D11_BUFFER_DESC bd;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.ByteWidth = sizeof(Vertex) * vCount;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	bd.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &vertices[0];

	HR(md3dDevice->CreateBuffer(&bd, &initData, &mTestVertexBuffer));

	std::vector<UINT> indices(vCount);
	for (UINT i = 0; i < vCount; ++i)
	{
		indices[i] = i;
	}

	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.ByteWidth = sizeof(UINT) * vCount;
	
	initData.pSysMem = &indices[0];

	HR(md3dDevice->CreateBuffer(&bd, &initData, &mTestIndexBuffer));
	mTestIndexCount = vCount;
};

void RenderTarget::BuildGeometryBuffers()
{
	GeometryGenerator::MeshData King, Queen, Knight, Bishop, Rook, Pawn, Board;

	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 0.2f, 1.0f, Board);
	geoGen.CreateSphere(0.5f, 8, 2, King);
	geoGen.CreateSphere(0.5f, 20, 20, Queen);
	geoGen.CreateBox(0.5f, 0.5f, 0.5f, Knight);
	geoGen.CreateCylinder(0.3f, 0.1f, 1.0f, 20, 20, Bishop);
	geoGen.CreateCylinder(0.3f, 0.3f, 0.75f, 20, 20, Rook);
	geoGen.CreateBox(0.5f, 0.25f, 0.5, Pawn);

	BuildBoxExtents(Board, mBoardBox);
	BuildBoxExtents(King, mKingBox);
	BuildBoxExtents(Queen, mQueenBox);
	BuildBoxExtents(Knight, mKnightBox);
	BuildBoxExtents(Bishop, mBishopBox);
	BuildBoxExtents(Rook, mRookBox);
	BuildBoxExtents(Pawn, mPawnBox);


	mBoardVertexOffset	= 0;
	mKingVertexOffset	= Board.Vertices.size();
	mQueenVertexOffset	= King.Vertices.size() + mKingVertexOffset;
	mKnightVertexOffset	= Queen.Vertices.size() + mQueenVertexOffset;
	mBishopVertexOffset	= Knight.Vertices.size() + mKnightVertexOffset;
	mRookVertexOffset	= Bishop.Vertices.size() + mBishopVertexOffset;
	mPawnVertexOffset	= Rook.Vertices.size() + mRookVertexOffset;

	mBoardIndexCount	= Board.Indices.size();
	mKingIndexCount		= King.Indices.size();
	mQueenIndexCount	= Queen.Indices.size();
	mKnightIndexCount	= Knight.Indices.size();
	mBishopIndexCount	= Bishop.Indices.size();
	mRookIndexCount		= Rook.Indices.size();
	mPawnIndexCount		= Pawn.Indices.size();

	mBoardIndexOffset	= 0;
	mKingIndexOffset	= mBoardIndexCount;
	mQueenIndexOffset	= mKingIndexCount + mKingIndexOffset;
	mKnightIndexOffset	= mQueenIndexCount + mQueenIndexOffset;
	mBishopIndexOffset	= mKnightIndexCount + mKnightIndexOffset;
	mRookIndexOffset	= mBishopIndexCount + mBishopIndexOffset;
	mPawnIndexOffset	= mRookIndexCount + mRookIndexOffset;

	mBoardMesh.Vertices.insert(mBoardMesh.Vertices.begin(), Board.Vertices.begin(), Board.Vertices.end());
	mBoardMesh.Indices.insert (mBoardMesh.Indices.begin(), Board.Indices.begin(), Board.Indices.end());

	mKingMesh.Vertices.insert(mKingMesh.Vertices.begin(), King.Vertices.begin(), King.Vertices.end());
	mKingMesh.Indices.insert (mKingMesh.Indices.begin(), King.Indices.begin(), King.Indices.end());

	mQueenMesh.Vertices.insert(mQueenMesh.Vertices.begin(), Queen.Vertices.begin(), Queen.Vertices.end());
	mQueenMesh.Indices.insert (mQueenMesh.Indices.begin(), Queen.Indices.begin(), Queen.Indices.end());

	mKnightMesh.Vertices.insert(mKnightMesh.Vertices.begin(), Knight.Vertices.begin(), Knight.Vertices.end());
	mKnightMesh.Indices.insert (mKnightMesh.Indices.begin(), Knight.Indices.begin(), Knight.Indices.end());

	mBishopMesh.Vertices.insert(mBishopMesh.Vertices.begin(), Bishop.Vertices.begin(), Bishop.Vertices.end());
	mBishopMesh.Indices.insert (mBishopMesh.Indices.begin(), Bishop.Indices.begin(), Bishop.Indices.end());

	mRookMesh.Vertices.insert(mRookMesh.Vertices.begin(), Rook.Vertices.begin(), Rook.Vertices.end());
	mRookMesh.Indices.insert (mRookMesh.Indices.begin(), Rook.Indices.begin(), Rook.Indices.end());

	mPawnMesh.Vertices.insert(mPawnMesh.Vertices.begin(), Pawn.Vertices.begin(), Pawn.Vertices.end());
	mPawnMesh.Indices.insert (mPawnMesh.Indices.begin(), Pawn.Indices.begin(), Pawn.Indices.end());

	UINT totalVertexCount = (mPawnVertexOffset + Pawn.Vertices.size());
	UINT totalIndexCount = (mPawnIndexOffset + mPawnIndexCount);

	std::vector<Vertex> verticies(totalVertexCount);


	UINT k = 0;
	for(UINT i = 0; i < Board.Vertices.size(); ++i, ++k)
	{
		verticies[k].Pos = Board.Vertices[i].Position;
		verticies[k].Normal = Board.Vertices[i].Normal;
	}

	for(UINT i = 0; i < King.Vertices.size(); ++i, ++k)
	{
		verticies[k].Pos = King.Vertices[i].Position;
		verticies[k].Normal = King.Vertices[i].Normal;
	}

	for(UINT i = 0; i < Queen.Vertices.size(); ++i, ++k)
	{
		verticies[k].Pos = Queen.Vertices[i].Position;
		verticies[k].Normal = Queen.Vertices[i].Normal;
	}

	for (UINT i = 0; i < Knight.Vertices.size(); ++i, ++k)
	{
		verticies[k].Pos = Knight.Vertices[i].Position;
		verticies[k].Normal = Knight.Vertices[i].Normal;
	}

	for (UINT i = 0; i < Bishop.Vertices.size(); ++i, ++k)
	{
		verticies[k].Pos = Bishop.Vertices[i].Position;
		verticies[k].Normal = Bishop.Vertices[i].Normal;
	}

	for (UINT i = 0; i < Rook.Vertices.size(); ++i, ++k)
	{
		verticies[k].Pos = Rook.Vertices[i].Position;
		verticies[k].Normal = Rook.Vertices[i].Normal;
	}

	for (UINT i = 0; i < Pawn.Vertices.size(); ++i, ++k)
	{
		verticies[k].Pos = Pawn.Vertices[i].Position;
		verticies[k].Normal = Pawn.Vertices[i].Normal;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * totalVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vInitData;
	vInitData.pSysMem = &verticies[0];

	HR(md3dDevice->CreateBuffer(&vbd, &vInitData, &mObjectsVertexBuffer));

	std::vector<UINT> indices;
	indices.insert(indices.end(), Board.Indices.begin(), Board.Indices.end());
	indices.insert(indices.end(), King.Indices.begin(), King.Indices.end());
	indices.insert(indices.end(), Queen.Indices.begin(), Queen.Indices.end());
	indices.insert(indices.end(), Knight.Indices.begin(), Knight.Indices.end());
	indices.insert(indices.end(), Bishop.Indices.begin(), Bishop.Indices.end());
	indices.insert(indices.end(), Rook.Indices.begin(), Rook.Indices.end());
	indices.insert(indices.end(), Pawn.Indices.begin(), Pawn.Indices.end());

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iInitData;
	iInitData.pSysMem = &indices[0];

	HR(md3dDevice->CreateBuffer(&ibd, &iInitData, &mObjectsIndexBuffer));


	//************************
	//	degbug buffers
	//************************

	Vertex debugVertices[6];
	debugVertices[0] = Vertex(XMFLOAT3(-50.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
	debugVertices[1] = Vertex(XMFLOAT3(50.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
	debugVertices[2] = Vertex(XMFLOAT3(0.0f, -50.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
	debugVertices[3] = Vertex(XMFLOAT3(0.0f, 50.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
	debugVertices[4] = Vertex(XMFLOAT3(0.0f, 0.0f, -50.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
	debugVertices[5] = Vertex(XMFLOAT3(0.0f, 0.0f, 50.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));

	UINT debugIndices[] =
	{
		0, 1,
		2, 3,
		4, 5
	};

	D3D11_BUFFER_DESC dbd;
	dbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	dbd.ByteWidth = sizeof(Vertex) * 6;
	dbd.CPUAccessFlags = 0;
	dbd.MiscFlags = 0;
	dbd.StructureByteStride = 0;
	dbd.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA dInitData;
	dInitData.pSysMem = debugVertices;
	HR(md3dDevice->CreateBuffer(&dbd, &dInitData, &mDebugVertexBuffer));

	dbd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	dbd.ByteWidth = sizeof(UINT) * 6;

	dInitData.pSysMem = debugIndices;
	HR(md3dDevice->CreateBuffer(&dbd, &dInitData, &mDebugIndexBuffer));

};

void RenderTarget::BuildMenuBuffers()
{
	GeometryGenerator::MeshData menuBuffer;
	GeometryGenerator::Vertex  v0, v1, v2, v3, vert;

	//surrounding box construction
	XMStoreFloat3(&v0.Position, XMVectorSet(-0.2f, -0.05f, 0.0f, 1.0f));
	XMStoreFloat3(&v1.Position, XMVectorSet(-0.2f, +0.05f, 0.0f, 1.0f));
	XMStoreFloat3(&v2.Position, XMVectorSet(+0.2f, +0.05f, 0.0f, 1.0f));
	XMStoreFloat3(&v3.Position, XMVectorSet(+0.2f, -0.05f, 0.0f, 1.0f));

	menuBuffer.Vertices.push_back(v0);
	menuBuffer.Vertices.push_back(v1);
	menuBuffer.Vertices.push_back(v2);
	menuBuffer.Vertices.push_back(v3);

	menuBuffer.Indices.push_back(0);
	menuBuffer.Indices.push_back(1);

	menuBuffer.Indices.push_back(1);
	menuBuffer.Indices.push_back(2);

	menuBuffer.Indices.push_back(2);
	menuBuffer.Indices.push_back(3);

	menuBuffer.Indices.push_back(3);
	menuBuffer.Indices.push_back(0);

	mMenuBoxVertexOffset = 0;
	mMenuBoxIndexCount = 8;
	mMenuBoxIndexOffset = 0;

	BuildBoxExtents(menuBuffer, mMenuBox);
	
	vert.Position.z = 0.0f;
	std::ifstream bufferFile;

	//Text buffer NEW
	bufferFile.open("Buffers/MenuNew.txt");
	//files are created in a manner where there are equal vertex and index counts
	//files only contain vertices, index data is a zero-based count of each vertex
	bufferFile >> mMenuNewIndexCount;

	mMenuNewVertexOffset = menuBuffer.Vertices.size(); 
	mMenuNewIndexOffset = menuBuffer.Indices.size();
	for (UINT i = 0; i < mMenuNewIndexCount; ++i)
	{
		bufferFile >> vert.Position.x >> vert.Position.y;
		menuBuffer.Vertices.push_back(vert);
		menuBuffer.Indices.push_back(i);
	}
	bufferFile.close();

	bufferFile.open("Buffers/MenuLoad.txt");
	bufferFile >> mMenuLoadIndexCount;
	mMenuLoadVertexOffset = menuBuffer.Vertices.size();
	mMenuLoadIndexOffset = menuBuffer.Indices.size();
	for (UINT i = 0; i < mMenuLoadIndexCount; ++i)
	{
		bufferFile >> vert.Position.x >> vert.Position.y;
		menuBuffer.Vertices.push_back(vert);
		menuBuffer.Indices.push_back(i);
	}
	bufferFile.close();

	bufferFile.open("Buffers/MenuSave.txt");
	bufferFile >> mMenuSaveIndexCount;
	mMenuSaveVertexOffset = menuBuffer.Vertices.size();
	mMenuSaveIndexOffset = menuBuffer.Indices.size();
	for (UINT i = 0; i < mMenuSaveIndexCount; ++i)
	{
		bufferFile >> vert.Position.x >> vert.Position.y;
		menuBuffer.Vertices.push_back(vert);
		menuBuffer.Indices.push_back(i);
	}
	bufferFile.close();

	//////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////

	bufferFile.open("Buffers/MenuExit.txt");
	bufferFile >> mMenuExitIndexCount;
	mMenuExitVertexOffset = menuBuffer.Vertices.size();
	mMenuExitIndexOffset = menuBuffer.Indices.size();
	for (UINT i = 0; i < mMenuExitIndexCount; ++i)
	{
		bufferFile >> vert.Position.x >> vert.Position.y;
		menuBuffer.Vertices.push_back(vert);
		menuBuffer.Indices.push_back(i);
	}
	bufferFile.close();

	bufferFile.open("Buffers/MenuPlayer.txt");
	bufferFile >> mMenuPlayerIndexCount;
	mMenuPlayerVertexOffset = menuBuffer.Vertices.size();
	mMenuPlayerIndexOffset = menuBuffer.Indices.size();
	for (UINT i = 0; i < mMenuPlayerIndexCount; ++i)
	{
		bufferFile >> vert.Position.x >> vert.Position.y;
		menuBuffer.Vertices.push_back(vert);
		menuBuffer.Indices.push_back(i);
	}
	bufferFile.close();

	bufferFile.open("Buffers/MenuReplay.txt");
	bufferFile >> mMenuReplayIndexCount;
	mMenuReplayVertexOffset = menuBuffer.Vertices.size();
	mMenuReplayIndexOffset = menuBuffer.Indices.size();
	for (UINT i = 0; i < mMenuReplayIndexCount; ++i)
	{
		bufferFile >> vert.Position.x >> vert.Position.y;
		menuBuffer.Vertices.push_back(vert);
		menuBuffer.Indices.push_back(i);
	}
	bufferFile.close();

	bufferFile.open("Buffers/MenuQueen.txt");
	bufferFile >> mMenuQueenIndexCount;
	mMenuQueenVertexOffset = menuBuffer.Vertices.size();
	mMenuQueenIndexOffset = menuBuffer.Indices.size();
	for (UINT i = 0; i < mMenuQueenIndexCount; ++i)
	{
		bufferFile >> vert.Position.x >> vert.Position.y;
		menuBuffer.Vertices.push_back(vert);
		menuBuffer.Indices.push_back(i);
	}
	bufferFile.close();

	bufferFile.open("Buffers/MenuKnight.txt");
	bufferFile >> mMenuKnightIndexCount;
	mMenuKnightVertexOffset = menuBuffer.Vertices.size();
	mMenuKnightIndexOffset = menuBuffer.Indices.size();
	for (UINT i = 0; i < mMenuKnightIndexCount; ++i)
	{
		bufferFile >> vert.Position.x >> vert.Position.y;
		menuBuffer.Vertices.push_back(vert);
		menuBuffer.Indices.push_back(i);
	}
	bufferFile.close();

	bufferFile.open("Buffers/MenuBishop.txt");
	bufferFile >> mMenuBishopIndexCount;
	mMenuBishopVertexOffset = menuBuffer.Vertices.size();
	mMenuBishopIndexOffset = menuBuffer.Indices.size();
	for (UINT i = 0; i < mMenuBishopIndexCount; ++i)
	{
		bufferFile >> vert.Position.x >> vert.Position.y;
		menuBuffer.Vertices.push_back(vert);
		menuBuffer.Indices.push_back(i);
	}
	bufferFile.close();

	bufferFile.open("Buffers/MenuRook.txt");
	bufferFile >> mMenuRookIndexCount;
	mMenuRookVertexOffset = menuBuffer.Vertices.size();
	mMenuRookIndexOffset = menuBuffer.Indices.size();
	for (UINT i = 0; i < mMenuRookIndexCount; ++i)
	{
		bufferFile >> vert.Position.x >> vert.Position.y;
		menuBuffer.Vertices.push_back(vert);
		menuBuffer.Indices.push_back(i);
	}
	bufferFile.close();

	bufferFile.open("Buffers/MenuRed.txt");
	bufferFile >> mMenuRedIndexCount;
	mMenuRedVertexOffset = menuBuffer.Vertices.size();
	mMenuRedIndexOffset = menuBuffer.Indices.size();
	for (UINT i = 0; i < mMenuRedIndexCount; ++i)
	{
		bufferFile >> vert.Position.x >> vert.Position.y;
		menuBuffer.Vertices.push_back(vert);
		menuBuffer.Indices.push_back(i);
	}
	bufferFile.close();

	bufferFile.open("Buffers/MenuBlack.txt");
	bufferFile >> mMenuBlackIndexCount;
	mMenuBlackVertexOffset = menuBuffer.Vertices.size();
	mMenuBlackIndexOffset = menuBuffer.Indices.size();
	for (UINT i = 0; i < mMenuBlackIndexCount; ++i)
	{
		bufferFile >> vert.Position.x >> vert.Position.y;
		menuBuffer.Vertices.push_back(vert);
		menuBuffer.Indices.push_back(i);
	}
	bufferFile.close();

	bufferFile.open("Buffers/MenuWin.txt");
	bufferFile >> mMenuWinIndexCount;
	mMenuWinVertexOffset = menuBuffer.Vertices.size();
	mMenuWinIndexOffset = menuBuffer.Indices.size();
	for (UINT i = 0; i < mMenuWinIndexCount; ++i)
	{
		bufferFile >> vert.Position.x >> vert.Position.y;
		menuBuffer.Vertices.push_back(vert);
		menuBuffer.Indices.push_back(i);
	}
	bufferFile.close();



	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////




	mMenuMesh.Vertices.insert(mMenuMesh.Vertices.begin(), menuBuffer.Vertices.begin(), menuBuffer.Vertices.end());
	mMenuMesh.Indices.insert(mMenuMesh.Indices.begin(), menuBuffer.Indices.begin(), menuBuffer.Indices.end());

	UINT vertexCount = mMenuMesh.Vertices.size();
	UINT indexCount = mMenuMesh.Indices.size();
	mMenuBoxVertexOffset = 0;

	// XMVECTOR white = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	std::vector<Vertex> vertices(vertexCount);
	for (UINT i = 0; i < vertexCount; ++i)
	{
		vertices[i].Pos = mMenuMesh.Vertices[i].Pos;
		vertices[i].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		// vertices[i].Color = reinterpret_cast<float*>(&white);
	}


	D3D11_BUFFER_DESC mbd;
	mbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	mbd.ByteWidth = sizeof(Vertex) * vertexCount;
	mbd.CPUAccessFlags = 0;
	mbd.MiscFlags = 0;
	mbd.StructureByteStride = 0;
	mbd.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA vInitData;
	vInitData.pSysMem = &vertices[0];

	HR(md3dDevice->CreateBuffer(&mbd, &vInitData, &mMenuVertexBuffer));

	mbd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	mbd.ByteWidth = sizeof(UINT) * indexCount;
	vInitData.pSysMem = &mMenuMesh.Indices[0];

	HR(md3dDevice->CreateBuffer(&mbd, &vInitData, &mMenuIndexBuffer));
};

void RenderTarget::BuildFX()
{
	DWORD shaderFlags = 0;
#if defined (DEBUG) || defined (_DEBUG)
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationMsgs = 0;
	HRESULT hr = D3DX11CompileFromFile(L"FX/Lighting.fx", 0, 0, 0, "fx_5_0", shaderFlags, 0, 0, &compiledShader, &compilationMsgs, 0);

	if (compilationMsgs != 0)
	{
		MessageBoxA(0, reinterpret_cast<char*>(compilationMsgs->GetBufferPointer()), 0, 0);
		ReleaseCOM(compilationMsgs);
	}
	
	if (FAILED(hr))
	{
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX11compilefromfile", true);
	}

	SIZE_T m = compiledShader->GetBufferSize();
	HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), m, 0, md3dDevice, &mFX));

	ReleaseCOM(compiledShader);

	mTech				= mFX->GetTechniqueByName("LightTech");
	mFXWorldViewProj	= mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	mFXWorld			= mFX->GetVariableByName("gWorld")->AsMatrix();
	mFXWorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	mFXEyePosW			= mFX->GetVariableByName("gEyePosW")->AsVector();
	mFXDirLight			= mFX->GetVariableByName("gDirLight");
	mFXPointLight		= mFX->GetVariableByName("gPointLight");
	mFXSpotLight		= mFX->GetVariableByName("gSpotLight");
	mFXMaterial			= mFX->GetVariableByName("gMaterial");
	// mFXColorBase		= mFX->GetVariableByName("gColorBase")->AsVector();
	// mFXColorSpecial		= mFX->GetVariableByName("gColorSpecial")->AsVector();
	
};

void RenderTarget::BuildVertexLayout()
{
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0} // TODO: Try 12
	};	

	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mInputLayout));
};

void RenderTarget::BuildBoxExtents(GeometryGenerator::MeshData& meshData, XNA::AxisAlignedBox& boxData)
{
	float infinity = FLT_MAX;
	XMFLOAT3 vMinf3(+infinity, +infinity, +infinity);
	XMFLOAT3 vMaxf3(-infinity, -infinity, -infinity);

	XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	XMVECTOR vMax = XMLoadFloat3(&vMaxf3);

	for (UINT i = 0; i < meshData.Vertices.size(); ++i)
	{
		XMVECTOR currVert = XMLoadFloat3(&meshData.Vertices[i].Position);

		vMin = XMVectorMin(vMin, currVert);
		vMax = XMVectorMax(vMax, currVert);
	}

	XMStoreFloat3(&boxData.Center,  0.5f*(vMax + vMin));
	XMStoreFloat3(&boxData.Extents, 0.5f*(vMax - vMin));
};




//*********************************************
//		GameView Definitions
//*********************************************

GameView::GameView(QuickDef::Player faction)
	: mSelected(0), mPhi(0.25f * XM_PI), mTheta(1.5f * XM_PI + (XM_PI * faction)), mRadius(20.0f), mFaction(faction)//fix mPhi for player faction
{
	XMStoreFloat4(&mViewDisplace, XMVectorSet(3.5f, 0.0f, 3.5f, 0.0f));
	UpdateViewProjection();
};

GameView::~GameView()
{
	
};

LRESULT GameView::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (mRenderTarget != 0)
	{
		return mRenderTarget->MsgProc(hWnd, msg, wParam, lParam);
	}
	else
		return DefWindowProc(hWnd, msg, wParam, lParam);
};

void GameView::SetRenderTarget(RenderTarget* target)
{
	mRenderTarget = target;
};

void GameView::AttachGameLogic(GameLogic* gameLogic)
{
	mGameLogic = gameLogic;
};

void GameView::MouseInput(UINT msg, WPARAM btnState, int x, int y)
{
	switch (msg)
	{
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(btnState, x, y);
		break;

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(btnState, x, y);
		break;

	case WM_MOUSEMOVE:
		OnMouseMove(btnState, x, y);
		break;
	}
};

void GameView::Frame(ObjectInfo& objects)
{
	UpdateViewProjection();
	mRenderTarget->UpdateScene(0.0f, XMLoadFloat4(&mEyePosW), reinterpret_cast<float*>(&mViewProj));
	mRenderTarget->DrawScene(objects);
};

ObjectInfo GameView::GetObjects()
{
	return mGameLogic->ObjectRequest();
};

Object* GameView::ObjectPick(int x, int y, ObjectInfo& objects)
{
	return mRenderTarget->ObjectPick(x, y, reinterpret_cast<float*>(&mViewProj), objects);
};

void GameView::ProcessPicked(Object* picked)
{
	QuickDef::Player faction = picked->GetPlayer();
	QuickDef::Type objectType = picked->GetObjectType();


	if (objectType == QuickDef::Piece)
	{
		if (mFaction == faction)
		{
			if (mSelected != 0)
				mSelected->Deselect();

			mSelected = picked;
			mSelected->MakeSelected();
			ClearAssists();
			MoveAssist();
		}
		else
		{
			if (mSelected == 0)
			{
				return;
			}
			Location2D selectedLoc = mSelected->GetLocation();
			Location2D pickedLoc = picked->GetLocation();
			Location2D difference;
			difference.x = pickedLoc.x - selectedLoc.x;
			difference.z = pickedLoc.z - selectedLoc.z;

			Event move;
			move.CurrPlayer = mFaction;
			move.Selected = mSelected;
			move.Picked = picked;
			move.Move = true;
			move.Real = true;
			move.diffX = difference.x;
			move.diffZ = difference.z;
			if (SendEvent(move))
			{
				mSelected->Deselect();
				mSelected = 0;
				ClearAssists();

				if (move.Selected->GetMoveType() == QuickDef::Pawn && (pickedLoc.z == 7 || pickedLoc.z == 0))
				{
					if (mFaction == QuickDef::Black && pickedLoc.z == 7)
					{
						mRenderTarget->MenuOpen(mFaction, MenuDef::Ranked);
						LogMove(selectedLoc.x, selectedLoc.z, pickedLoc.x, pickedLoc.z, move, true);
						return;
					}
					else if (mFaction == QuickDef::Red && pickedLoc.z == 0)
					{
						mRenderTarget->MenuOpen(mFaction, MenuDef::Ranked);
						LogMove(selectedLoc.x, selectedLoc.z, pickedLoc.x, pickedLoc.z, move, true);
						return;
					}
				}

				LogMove(selectedLoc.x, selectedLoc.z, pickedLoc.x, pickedLoc.z, move, false);
			}

		}
		return;
	}
	else if (objectType == QuickDef::Board)
	{
		if (mSelected == 0)
		{
			return;
		}
		Location2D selectedLoc = mSelected->GetLocation();
		Location2D pickedLoc = picked->GetLocation();

		Event move;
		move.CurrPlayer = mFaction;
		move.Selected = mSelected;
		move.Picked = picked;
		move.Move = true;
		move.Real = true;
		move.diffX = pickedLoc.x - selectedLoc.x;
		move.diffZ = pickedLoc.z - selectedLoc.z;

		if(SendEvent(move))
		{
			mSelected->Deselect();
			mSelected = 0;
			ClearAssists();
			if (move.Selected->GetMoveType() == QuickDef::Pawn && (pickedLoc.z == 7 || pickedLoc.z == 0))
			{
				if (mFaction == QuickDef::Black && pickedLoc.z == 7)
				{
					mRenderTarget->MenuOpen(mFaction, MenuDef::Ranked);
					LogMove(selectedLoc.x, selectedLoc.z, pickedLoc.x, pickedLoc.z, move, true);
					return;
				}
				else if (mFaction == QuickDef::Red && pickedLoc.z == 0)
				{
					mRenderTarget->MenuOpen(mFaction, MenuDef::Ranked);
					LogMove(selectedLoc.x, selectedLoc.z, pickedLoc.x, pickedLoc.z, move, true);
					return;
				}
			}
			LogMove(selectedLoc.x, selectedLoc.z, pickedLoc.x, pickedLoc.z, move, false);
		}
	}
	else if (objectType == QuickDef::Menu)
	{
		return;
	}
	else
	{
		return;
	}
};

void GameView::UpdateViewProjection()
{
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float y = mRadius*cosf(mPhi);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);

	XMVECTOR displace = XMLoadFloat4(&mViewDisplace);
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	pos += displace;

	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR target = XMVectorSet(3.5f, 0.0f, 3.5f, 1.0f);
	// TODO: Fix viewpoj vs view?
	XMMATRIX v = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mViewProj, v);
	XMStoreFloat4(&mEyePosW, pos);
};

bool GameView::SendEvent(Event& data)
{
	return mGameLogic->EventProcess(data);
};

void GameView::MenuClose()
{
	mRenderTarget->MenuClose();
};

void GameView::SaveGame()
{

};

void GameView::Reset()
{
	mPhi = 0.25f * XM_PI;
	mTheta = 1.5f * XM_PI + (mFaction * XM_PI);
	mRadius = 20.0f;

	UpdateViewProjection();
};

void GameView::Win()
{
	mRenderTarget->MenuOpen(mFaction, MenuDef::Won);
};

void GameView::MoveAssist()
{
	if (mSelected == 0)
		return;

	ObjectInfo currObjects = GetObjects();
	Location2D selectedLoc = mSelected->GetLocation();

	for (UINT i = 0; i < currObjects.numBoards; ++i)
	{
		Location2D boardLoc = currObjects.BoardObjects[i]->GetLocation();

		Event data;
		data.CurrPlayer = mFaction;
		data.diffX = boardLoc.x - selectedLoc.x;
		data.diffZ = boardLoc.z - selectedLoc.z;
		data.Move = true;
		data.Real = false;
		data.Picked = currObjects.BoardObjects[i];
		data.Selected = mSelected;
		
		if (SendEvent(data))
		{
			currObjects.BoardObjects[i]->MakeSelected();
		}
	}
};

void GameView::ClearAssists()
{
	ObjectInfo currObjects = GetObjects();

	for (UINT i = 0; i < currObjects.numBoards; ++i)
	{
		currObjects.BoardObjects[i]->Deselect();
	}
};

//*******************************************
//			PlayerView definitons
//*******************************************
//*************   STATICS   *****************
UINT PlayerView::mReferences = 0;
std::fstream* PlayerView::mMoveFile = 0;
//*******************************************

PlayerView::PlayerView(QuickDef::Player faction)
	:GameView(faction), test(0), testVertices(0)
{
	if ( ! mMoveFile)
	{
		mMoveFile = new std::fstream;
		mMoveFile->open("Saves/TempSave.txt", std::ios::trunc | std::ios::in | std::ios::out);
	}
	++mReferences;
};

PlayerView::~PlayerView()
{
	if (mReferences == 1)
	{
		mMoveFile->close();
		delete mMoveFile;
		mMoveFile = 0;
	}
	--mReferences;
};

void PlayerView::LogMove(UINT x, UINT z, UINT nx, UINT nz, Event& data, bool promotion)
{
	if (promotion)
	{
		UINT p;
		if (data.CurrPlayer == QuickDef::Black)
			p = 3;
		else
			p = 4;
		*mMoveFile << p << '\t' << x << '\t' << z << "\t\t" << nx << '\t' << nz <<'\t';
	}
	else
		*mMoveFile << data.CurrPlayer << '\t' << x << '\t' << z << "\t\t" << nx << '\t' << nz << '\n';
};

void PlayerView::LogPromotion(QuickDef::MoveType piece)
{
	*mMoveFile << piece << '\n';
};

void PlayerView::Reset()
{
	if (! mMoveFile)
		return;

	mMoveFile->clear();
	mMoveFile->close();
	mMoveFile->open("Saves/TempSave.txt", std::ios::trunc | std::ios::in | std::ios::out);
	if (mMoveFile->is_open())
	{
		int t = 0;
		++t;
	}
	if (mMoveFile->good())
	{
		int h = 3;
		++h;
	}
	GameView::Reset();
};

void PlayerView::SaveGame()
{
	if (! mMoveFile)
		return;

	std::ofstream saveFile("Saves/LastSave.txt", std::ios::trunc);
	mMoveFile->seekg(std::ios::beg);
	saveFile << mMoveFile->rdbuf();
	saveFile.close();
};

void PlayerView::OnMouseDown(WPARAM btnState, int x, int y)
{
	if ( (btnState & MK_LBUTTON) )
	{
		ObjectInfo currObjects = GetObjects();
		Object* picked = ObjectPick(x, y, currObjects);
		if (picked != 0)
			ProcessPicked(picked);
	}

	else if ( (btnState & MK_RBUTTON) || (btnState & MK_MBUTTON) )
	{
		mLastMousePos.x = x;
		mLastMousePos.y = y;
	};
};

void PlayerView::OnMouseUp(WPARAM btnState, int x, int y)
{

};

void PlayerView::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ( (btnState & MK_RBUTTON) )
	{
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

		mTheta += dx;
		mPhi += dy;

		//possible clamp and or limiting value growth
	}

	else if ( (btnState & MK_MBUTTON) )
	{
		float dx = 0.01f * static_cast<float>(x - mLastMousePos.x);
		float dy = 0.01f * static_cast<float>(y - mLastMousePos.y);

		mRadius += dx - dy;

		//clamp?
	}
	// TODO: Remove code and related code
	else if ( false && (btnState & MK_LBUTTON) )
	{
		int dx = x - mLastDrawPos.x;
		int dy = y - mLastDrawPos.y;

		if (((dx < 5 || dx > 5) || (dy < 5 || dy > 5)) || (dx - dy < 5 || dx - dy > 5))
		{
			CreateBuffer(x, y);
		}
		++test;
		if (test == 100)
		{
			test = 0;
		}
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
};

void PlayerView::CreateBuffer(int sx, int sy)
{
	float vx = (+2.0f*sx/1680  - 1.0f);
	float vy = (-2.0f*sy/1050 + 1.0f);
	Vertex temp;
	temp.Pos.x = vx;
	temp.Pos.y = vy;

	testVertices.push_back(temp);
	testSize = testVertices.size();

	std::ofstream bufferFile("Buffers/test.txt", std::ios::trunc);

	bufferFile << testSize << '\n';
	for (UINT i = 0; i < testSize; ++i)
	{
		bufferFile << testVertices[i].Pos.x << "   " << testVertices[i].Pos.y << '\n';
	}
};

//*******************************************
//			ReplayView definitions
//*******************************************
//********* STATICS *************************
std::fstream* ReplayView::mReplayFile = 0;
//*******************************************
ReplayView::ReplayView(bool temp)
	:GameView(QuickDef::Black)
{
	if (! mReplayFile)
	{
		mReplayFile = new std::fstream;
		mReplayFile->open("Saves/Replay.txt", std::ios::in);
		mReplayFile->seekg(std::ios::beg);
		int t;
		if (mReplayFile->is_open())
			t = 5;
		if (mReplayFile->good())
			t = 4;
	}
};

ReplayView::~ReplayView()
{
	if (mReplayFile)
	{
		mReplayFile->clear();
		mReplayFile->close();
		delete mReplayFile;
		mReplayFile = 0;
	}
};

void ReplayView::LogMove(UINT x, UINT z, UINT nx, UINT nz, Event& data, bool promotion)
{
	
};

void ReplayView::Reset()
{
	mFaction = QuickDef::Black;
	mReplayFile->clear();
	mReplayFile->seekg(std::ios::beg);
	GameView::Reset();
};



void ReplayView::QuickRun()
{
	mReplayFile->seekg(std::ios::beg);
	UINT p, x, z;

	while ((*mReplayFile >> p >> x >> z))
	{
		bool promotion = false;
		ObjectInfo currObjects = GetObjects();
		QuickDef::Player currPlayer;
		
		if (p == 1)
			currPlayer = QuickDef::Red;
		else if (p == 0)
			currPlayer = QuickDef::Black;
		else if (p == 3)
		{
			currPlayer = QuickDef::Black;
			promotion = true;
		}
		else if (p == 4)
		{
			currPlayer = QuickDef::Red;
			promotion = true;
		}
		

		mFaction = currPlayer;
		mSelected = FindObject(x, z, currObjects);

		if ( ! (*mReplayFile >> x >> z))
		{
			return;//---------error
		}

		Object* picked = FindObject(x, z, currObjects);

		if (picked != 0)
		{
			ProcessPicked(picked);
		}
		else
			//error

		if (promotion)
		{
			MenuClose();
			UINT piece;

			if (! (*mReplayFile >> piece))
			{
				//error
			}

			Object* pickedPiece = 0;
			switch (piece)
			{
			case QuickDef::Queen:
				picked = new Queen(currPlayer, 0, 0);
				break;
			
			case QuickDef::Knight:
				picked = new Knight(currPlayer, 0, 0);
				break;

			case QuickDef::Bishop:
				picked = new Bishop(currPlayer, 0, 0);
				break;

			case QuickDef::Rook:
				picked = new Rook(currPlayer, 0, 0);
				break;

			default:
				picked = 0;
			}

			if (picked == 0)
			{
				return;
				//error
			}

			Event data;
			data.Move = false;
			data.CurrPlayer = currPlayer;
			data.Picked = pickedPiece;
			
			if (! SendEvent(data))
			{
				//error
			}
		}
	}
	mReplayFile->clear();
	mReplayFile->seekg(std::ios::beg);
};

void ReplayView::OnMouseDown(WPARAM btnState, int sx, int sy)
{
	if ( (btnState & MK_LBUTTON) )
	{

		bool promotion = false;
		ObjectInfo currObjects = GetObjects();
		QuickDef::Player currPlayer;
		UINT p, x, z;
		if (false)//for menu
		{
			Object* fake = ObjectPick(sx, sy, currObjects);
			return;
		}

		if (! (*mReplayFile >> p >> x >> z))
		{
			Object* fakes = ObjectPick(sx, sy, currObjects);
			return;
		}
		
		if (p == 1)
			currPlayer = QuickDef::Red;
		else if (p == 0)
			currPlayer = QuickDef::Black;
		else if (p == 3)
		{
			currPlayer = QuickDef::Black;
			promotion = true;
		}
		else if (p == 4)
		{
			currPlayer = QuickDef::Red;
			promotion = true;
		}
		

		mFaction = currPlayer;

		mSelected = FindObject(x, z, currObjects);
		*mReplayFile >> x >> z;
		Object* picked = FindObject(x, z, currObjects);
		if (picked != 0)
			ProcessPicked(picked);

		if (promotion)
		{
			MenuClose();
			UINT piece;
			*mReplayFile >> piece;

			Object* picked;
			switch (piece)
			{
			case QuickDef::Queen:
				picked = new Queen(currPlayer, 0, 0);
				break;
			
			case QuickDef::Knight:
				picked = new Knight(currPlayer, 0, 0);
				break;

			case QuickDef::Bishop:
				picked = new Bishop(currPlayer, 0, 0);
				break;

			case QuickDef::Rook:
				picked = new Rook(currPlayer, 0, 0);
				break;

			default:
				picked = 0;
			}

			if (picked == 0)
				return;

			Event data;
			data.Move = false;
			data.CurrPlayer = currPlayer;
			data.Picked = picked;
			
			SendEvent(data);
		}
		Object* fake = ObjectPick(sx, sy, currObjects);
	}

	else if ( (btnState & MK_RBUTTON) || (btnState & MK_MBUTTON) )
	{
		mLastMousePos.x = sx;
		mLastMousePos.y = sy;
	};
};

void ReplayView::OnMouseUp(WPARAM btnState, int x, int y)
{

};

void ReplayView::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ( (btnState & MK_RBUTTON) )
	{
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

		mTheta += dx;
		mPhi += dy;

		//possible clamp and or limiting value growth
	}

	else if ( (btnState & MK_MBUTTON) )
	{
		float dx = 0.01f * static_cast<float>(x - mLastMousePos.x);
		float dy = 0.01f * static_cast<float>(y - mLastMousePos.y);

		mRadius += dx - dy;

		//clamp?
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
};

Object* ReplayView::FindObject(UINT x, UINT z, ObjectInfo& objects)
{
	for (UINT i = 0; i < objects.numBlack; ++i)
	{
		Location2D loc = objects.BlackObjects[i]->GetLocation();
		
		if (loc.x == x && loc.z == z)
			return objects.BlackObjects[i];
	}
	for (UINT i = 0; i < objects.numRed; ++i)
	{
		Location2D loc = objects.RedObjects[i]->GetLocation();

		if (loc.x == x && loc.z == z)
			return objects.RedObjects[i];
	}
	//since board objects are checked last, mSelected wont be a board unless bad data
	for (UINT i = 0; i < objects.numBoards; ++i)
	{
		Location2D loc = objects.BoardObjects[i]->GetLocation();

		if (loc.x == x && loc.z == z)
			return objects.BoardObjects[i];
	}
	return 0;
};