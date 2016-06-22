//***************************************************************************************
// color.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************

cbuffer cbPerObject//------------------------------create random colors????
{
	float4x4 gWorldViewProj; //test its globalness---------------------------
	float4 gColorBase;
	float4 gColorSpecial;
	float4 gColorRandom;
};

/*RasterizerState WF
{
	FillMode = Wireframe;
	CullMode = None;
	FrontCounterClockwise = false;
};*/

struct VertexIn
{
	float3 PosL  : POSITION;
    float4 Color : COLOR;
};

float Random( float2 p )
{
  const float2 r = float2( 23.1406926327792690, 2.6651441426902251);
  return cos( dot(p,r) );  
}




struct VertexOut// can these be joined???------------------------------------
{
	float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	//vin.PosL.xy += 0.5f*sin(vin.PosL.x)*sin(3.0f*gTime);
	//vin.PosL.z *= 0.6f + 0.4f*sin(2.0f*gTime);

	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	// Just pass vertex color into the pixel shader.
    vout.Color = vin.Color;
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target//------------------return required????
{
	if (pin.Color[2] == 0.0f || pin.Color[1] == 0.0f)
	{
//		return pin.Color;
	}
	float2 f2 = float2(pin.PosH[1], pin.PosH[2]);
	float rand = Random(f2);
	float4 colorRandom = float4(0.01f * rand, 0.1f * rand, 0.1f * rand, 0.0f);
	pin.Color = gColorBase + gColorSpecial + gColorRandom;
    return pin.Color;
}

technique11 ColorTech
{
    pass P0
    {
		//SetRasterizerState(WF);
        SetVertexShader( CompileShader( vs_4_0, VS() ) );//changed from 5_0
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );//changed from 5_0

    }
}
