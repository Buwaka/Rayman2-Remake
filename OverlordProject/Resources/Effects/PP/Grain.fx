//=============================================================================
//// Shader uses position and texture
//=============================================================================
SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

Texture2D gTexture;
float gRandom;
float gStrength;
float gScale;

/// Create Depth Stencil State (ENABLE DEPTH WRITING)
DepthStencilState gDSS_NoDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
};

/// Create Rasterizer State (Backface culling) 
RasterizerState gRS_NoBackfaceCulling
{
    CullMode = BACK;
};


//IN/OUT STRUCTS
//--------------
struct VS_INPUT
{
    float3 Position : POSITION;
	float2 TexCoord : TEXCOORD0;

};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD1;
};


//VERTEX SHADER
//-------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	// Set the Position
	output.Position = float4(input.Position, 1.0f);
	// Set the TexCoord
	output.TexCoord = input.TexCoord;
	   
	return output;
}


float random( float2 p )
{
     return frac(sin(dot(p.xy ,float2( gRandom,7.226)))*42.7745);
}

//PIXEL SHADER
//------------
float4 PS(PS_INPUT input): SV_Target
{
	//grain coordinate
	float2 normUV = input.TexCoord.xy;
	normUV *= gScale;

	//to int
	float2 ipos = floor(normUV);
	//random
	float rand = random(ipos);

	float4 grain = float4(rand,rand,rand,1.0);
	grain = lerp( gTexture.Sample(samPoint, input.TexCoord), grain, gStrength);
	grain.a = 1.0;
	
    return grain;
}


//TECHNIQUE
//---------
technique11 Grayscale
{
    pass P0
    {          
        SetRasterizerState(gRS_NoBackfaceCulling);
        SetDepthStencilState(gDSS_NoDepth, 0);
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

