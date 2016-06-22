/*#ifndef MAIN_H
#define MAIN_H
#include "702/702_Create.h"

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

class MyApp : public D3DAppControl
{
public:
	MyApp(HINSTANCE hInstance);
	~MyApp();

	bool Init();
	void OnResize();
	void UpdateScene(float deltaTime);
	void DrawScene();
	void NoFullscreen();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	void BuildGeometryBuffers();
	void BuildFX();
	void BuildVertexLayout();

private:
	ID3D11Buffer* mVR;
	ID3D11Buffer* mVG;
	ID3D11Buffer* mIB;

	ID3D11Buffer* mXB;
	ID3D11Buffer* mYB;
	ID3D11Buffer* mZB;

	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mFxWorldViewProj;

	ID3D11InputLayout* mInputLayout;

	ID3D11RasterizerState* mWireFrameRS;

	UINT mIndexCount;
	UINT mIndexOffset;

	XMFLOAT4X4 mWorld[65];
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};

void MyApp::NoFullscreen()
{
	IDXGIDevice* dxgiDevice = 0;
	HR(md3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void **>(&dxgiDevice)));
	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void **>(&dxgiAdapter)));
	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void **>(&dxgiFactory)));

	//HR(dxgiFactory->MakeWindowAssociation(mhMainWnd, DXGI_MWA_NO_WINDOW_CHANGES));

	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);
}

MyApp::MyApp(HINSTANCE hInstance)
	: D3DAppControl(hInstance), mIB(0), mVR(0), mVG(0), mTheta(1.5f * XM_PI), mPhi(0.000001f), mRadius(20.0f), mFX(0), mWireFrameRS(0), mInputLayout(0)
{
	mClientWidth  = 1680;
	mClientHeight = 1050;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);

	Init();



	XMStoreFloat4x4(&mWorld[64], XMMatrixTranslation(-0.5f, 0.0f,-0.5f));

	UINT k = 0;
	for (UINT i = 0; i < 8; ++i)
	{
		for (UINT j = 0; j < 8; ++j, ++k)
		{
			XMStoreFloat4x4(&mWorld[k], XMMatrixTranslation(static_cast<float>(j), 0.0f, static_cast<float>(i)));
		}
	}

	Vertex vertRed[] =
	{
		{XMFLOAT3(-0.5f, 0.0f, 0.5f), XMFLOAT4(0.8f, 0.6f, 0.4f, 1.0f)},
		{XMFLOAT3( 0.5F, 0.0f, 0.5f), XMFLOAT4(0.8f, 0.6f, 0.4f, 1.0f)},
		{XMFLOAT3(-0.5f, 0.0f,-0.5f), XMFLOAT4(0.8f, 0.6f, 0.4f, 1.0f)},
		{XMFLOAT3( 0.5f, 0.0f,-0.5f), XMFLOAT4(0.8f, 0.6f, 0.4f, 1.0f)}
	};

	Vertex vertGreen[] =
	{
		{XMFLOAT3(-0.5f, 0.0f, 0.5f), XMFLOAT4(0.33f, 0.21f, 0.08f, 1.0f)},
		{XMFLOAT3( 0.5f, 0.0f, 0.5f), XMFLOAT4(0.33f, 0.21f, 0.08f, 1.0f)},
		{XMFLOAT3(-0.5f, 0.0f,-0.5f), XMFLOAT4(0.33f, 0.21f, 0.08f, 1.0f)},
		{XMFLOAT3( 0.5f, 0.0f,-0.5f), XMFLOAT4(0.33f, 0.21f, 0.08f, 1.0f)}
	};

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * 4;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertRed;
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVR));

	vinitData.pSysMem = vertGreen;
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVG));

	UINT indicies[] = //fix
	{
		0, 1, 2,
		2, 1, 3
	};

	vbd.ByteWidth = sizeof(UINT) * 6;
	vbd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	vinitData.pSysMem = indicies;
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mIB));

	mIndexCount = 6;
	mIndexOffset = 0;

	Vertex x[] =
	{
		{XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
		{XMFLOAT3(50.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)}
	};

	Vertex y[] =
	{
		{XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
		{XMFLOAT3(0.0f, 50.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)}
	};

	Vertex z[] =
	{
		{XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f)},
		{XMFLOAT3(0.0f, 0.0f, 50.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f)}
	};

	vbd.ByteWidth = sizeof(Vertex) * 2;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	vinitData.pSysMem = x;

	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mXB));

	vinitData.pSysMem = y;

	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mYB));

	vinitData.pSysMem = z;

	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mZB));



	//FX---------------------------------------

	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
 
	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationMsgs = 0;
	HRESULT hr = D3DX11CompileFromFile(L"FX/Basic.fx", 0, 0, 0, "fx_5_0", shaderFlags, 
		0, 0, &compiledShader, &compilationMsgs, 0);

	// compilationMsgs can store errors or warnings.
	if( compilationMsgs != 0 )
	{
		MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
		ReleaseCOM(compilationMsgs);
	}

	// Even if there are no compilationMsgs, check to make sure there were no other errors.
	if(FAILED(hr))
	{
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX11CompileFromFile", true);
	}
	SIZE_T m = compiledShader->GetBufferSize();
	HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), 
		0, md3dDevice, &mFX));

	// Done with compiled shader.
	ReleaseCOM(compiledShader);

	mTech    = mFX->GetTechniqueByName("ColorTech");
	mFxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	//mFX->GetVariableByIndex("gWorldViewProj")->asv
	//mfxTime = mFX->GetVariableByName("gTime")->AsScalar();
	ID3DX11EffectType* h = mFxWorldViewProj->GetType();
	D3DX11_EFFECT_VARIABLE_DESC* g = 0;
	mFxWorldViewProj->GetDesc(g);

	BuildVertexLayout();

}
MyApp::~MyApp()
{
	ReleaseCOM(mVR);
	ReleaseCOM(mVG);
	ReleaseCOM(mIB);
	ReleaseCOM(mFX);
	ReleaseCOM(mInputLayout);
	ReleaseCOM(mWireFrameRS);
}
bool MyApp::Init()
{
	return D3DAppControl::Init();
}
void MyApp::OnResize()
{
	D3DAppControl::OnResize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * XM_PI, getAspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
	
}
void MyApp::UpdateScene(float deltaTime)
{

	float x = mRadius*sinf(mPhi)*cosf(mTheta);//1.5707964  4.7123890
	float z = mRadius*sinf(mPhi)*sinf(mTheta);//----------review----------------------------------
	float y = mRadius*cosf(mPhi);
	XMVECTOR displace = XMVectorSet(3.5f, 0.0f, 3.5f, 0.0f);

	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	pos += displace;
	XMVECTOR target = XMVectorSet(3.5f, 0.0f, 3.5f, 1.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX v = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, v);
}
void MyApp::DrawScene()
{
	assert(mSwapChain);
	assert(md3dImmediateContext);

	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Black));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(mInputLayout);


	//md3dImmediateContext->RSSetState(mWireFrameRS);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX viewProj = view * proj;



	md3dImmediateContext->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

	




	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		XMMATRIX world = XMLoadFloat4x4(&mWorld[64]);
		mFxWorldViewProj->SetMatrix(reinterpret_cast<float*>(& (world * viewProj)));
		mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mXB, &stride, &offset);
		md3dImmediateContext->DrawIndexed(2, 0, 0);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mYB, &stride, &offset);
		md3dImmediateContext->DrawIndexed(2, 0, 0);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mZB, &stride, &offset);
		md3dImmediateContext->DrawIndexed(2, 0, 0);




		md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mVR, &stride, &offset);
		int k = 0;
		for (UINT i = 0; i < 64; ++i, ++k)
		{
			if (i % 8 == 0 && k != 0)
			{
				--k;
			}
			if (k % 2 == 0)
			{
				world = XMLoadFloat4x4(&mWorld[i]);
				mFxWorldViewProj->SetMatrix(reinterpret_cast<float*>(& (world * viewProj)));
				mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
				md3dImmediateContext->DrawIndexed(mIndexCount, mIndexOffset, 0);
			}

		}
		k = 0;
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mVG, &stride, &offset);

		for (UINT i = 0; i < 64; ++i, ++k)
		{
			if (i % 8 == 0 && k != 0)
			{
				--k;
			}
			if (k % 2 == 1)
			{
				world = XMLoadFloat4x4(&mWorld[i]);
				mFxWorldViewProj->SetMatrix(reinterpret_cast<float*>(& (world * viewProj)));
				mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
				md3dImmediateContext->DrawIndexed(mIndexCount, mIndexOffset, 0);
			}

		}
	}

	HR(mSwapChain->Present(1,0));
}

void MyApp::BuildVertexLayout()
{
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mInputLayout));

	D3D11_RASTERIZER_DESC wireframeDesc;
	ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_BACK;
	wireframeDesc.FrontCounterClockwise = false;
	wireframeDesc.DepthClipEnable = true;
	//wireframeDesc.ScissorEnable = true;
	HR(md3dDevice->CreateRasterizerState(&wireframeDesc, &mWireFrameRS));
}

void MyApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void MyApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void MyApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if( (btnState & MK_LBUTTON) != 0 )
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		// Update angles based on input to orbit camera around box.
		mTheta += dx;
		mPhi   += dy;

		// Restrict the angle mPhi.
		//mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi-0.1f);
	}
	else if( (btnState & MK_RBUTTON) != 0 )
	{
		// Make each pixel correspond to 0.005 unit in the scene.
		float dx = 0.005f*static_cast<float>(x - mLastMousePos.x);//--------------------------use just the x?
		float dy = 0.005f*static_cast<float>(y - mLastMousePos.y);

		// Update the camera radius based on input.
		mRadius += dx - dy;

		// Restrict the radius.
		//mRadius = MathHelper::Clamp(mRadius, 3.0f, 15.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

#endif // MAIN_H*/