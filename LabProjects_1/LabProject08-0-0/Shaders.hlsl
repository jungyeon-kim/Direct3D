cbuffer cbGameObjectInfo : register(b0)
{
	matrix		gmtxGameObject : packoffset(c0);
};

cbuffer cbCameraInfo : register(b1)
{
	matrix		gmtxView : packoffset(c0);
	matrix		gmtxProjection : packoffset(c4);
};

cbuffer cbFrameworkInfo : register(b2)
{
	float 		gfCurrentTime;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_DIFFUSED_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct VS_DIFFUSED_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VS_DIFFUSED_OUTPUT VSPlayer(VS_DIFFUSED_INPUT input)
{
	VS_DIFFUSED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.color = input.color;

	return(output);
}

float4 PSPlayer(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
	return(input.color);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
Texture2D gtxtTexture : register(t0);
SamplerState gSamplerState : register(s0);

struct VS_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VS_TEXTURED_OUTPUT VSTextureToScreen(uint nVertexID : SV_VertexID)
{
	VS_TEXTURED_OUTPUT output;
	if (nVertexID == 0) { output.position = float4(-1.0f, +1.0f, 0.0f, 1.0f); output.uv = float2(0.0f, 0.0f); }
	if (nVertexID == 1) { output.position = float4(+1.0f, +1.0f, 0.0f, 1.0f); output.uv = float2(1.0f, 0.0f); }
	if (nVertexID == 2) { output.position = float4(+1.0f, -1.0f, 0.0f, 1.0f); output.uv = float2(1.0f, 1.0f); }
	if (nVertexID == 3) { output.position = float4(-1.0f, +1.0f, 0.0f, 1.0f); output.uv = float2(0.0f, 0.0f); }
	if (nVertexID == 4) { output.position = float4(+1.0f, -1.0f, 0.0f, 1.0f); output.uv = float2(1.0f, 1.0f); }
	if (nVertexID == 5) { output.position = float4(-1.0f, -1.0f, 0.0f, 1.0f); output.uv = float2(0.0f, 1.0f); }

	return(output);
}

float4 PSTunnelEffect(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float2 p = (2.0f * input.uv.xy) - 1.0f;
//	float2 p = ((2.0f * input.position.xy) - float2(640.0f, 480.0f)) / 640.0f; //x: (-1, +1)
//	float2 p = ((2.0f * input.position.xy) - float2(640.0f, 480.0f)) / 480.0f; //y: (-1, +1)

	float a = atan(p.y / p.x); //-0.5 pi ~ +0.5 pi

//	float r = pow(p.x * p.x + p.y * p.y, 1.0f / 2.0f);
//	float r = pow(pow(p.x * p.x, 2.0f) + pow(p.y * p.y, 2.0f), 1.0f / 4.0f);
//	float r = pow(pow(p.x * p.x, 2.0f) + pow(p.y * p.y, 2.0f), 1.0f / 4.0f);
	float r = pow(pow(p.x * p.x, 8.0f) + pow(p.y * p.y, 8.0f), 1.0f / 16.0f);
	float2 uv = float2((0.3f / r) + (0.2f * gfCurrentTime), (a / 3.1415927f));
//	float2 uv = float2((0.4f / r) + (0.1f * gfCurrentTime), 4.0f * (a / 3.1415927f));
//	float4 cColor = gtxtTexture.Sample(gSamplerState, uv);

    float2 uv2 = float2(uv.x, atan(p.y / abs(p.x)) / 3.1415927f);
	float4 cColor = gtxtTexture.SampleGrad(gSamplerState, uv, ddx(uv2), ddy(uv2));

	cColor = cColor * r;
    
	return(cColor);
}
