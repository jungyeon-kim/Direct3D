cbuffer cbPlayerInfo : register(b0)
{
	matrix		gmtxPlayerWorld : packoffset(c0);
};

cbuffer cbCameraInfo : register(b1)
{
	matrix		gmtxView : packoffset(c0);
	matrix		gmtxProjection : packoffset(c4);
	float3		gvCameraPosition : packoffset(c8);
};

cbuffer cbGameObjectInfo : register(b2)
{
	matrix		gmtxGameObject : packoffset(c0);
	uint		gnMaterial : packoffset(c4);
};

#include "Light.hlsl"

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

VS_DIFFUSED_OUTPUT VSDiffused(VS_DIFFUSED_INPUT input)
{
	VS_DIFFUSED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.color = input.color;

	return(output);
}

float4 PSDiffused(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
	return(input.color);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
VS_DIFFUSED_OUTPUT VSPlayer(VS_DIFFUSED_INPUT input)
{
	VS_DIFFUSED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxPlayerWorld), gmtxView), gmtxProjection);
	output.color = input.color;

	return(output);
}

float4 PSPlayer(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
	return(input.color);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_TEXTURED_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

struct VS_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VS_TEXTURED_OUTPUT VSTextured(VS_TEXTURED_INPUT input)
{
	VS_TEXTURED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

Texture2D gtxtDiffuse : register(t0);

SamplerState gSamplerState : register(s0);

float4 PSTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtDiffuse.Sample(gSamplerState, input.uv);

	return(cColor);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//#define _WITH_VERTEX_LIGHTING

struct VS_TEXTURED_LIGHTING_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct VS_TEXTURED_LIGHTING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	//	nointerpolation float3 normalW : NORMAL;
	float2 uv : TEXCOORD;
#ifdef _WITH_VERTEX_LIGHTING
	float4 color : COLOR;
#endif
};

VS_TEXTURED_LIGHTING_OUTPUT VSTexturedLighting(VS_TEXTURED_LIGHTING_INPUT input)
{
	VS_TEXTURED_LIGHTING_OUTPUT output;

	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;
#ifdef _WITH_VERTEX_LIGHTING
	output.normalW = normalize(output.normalW);
	output.color = Lighting(output.positionW, output.normalW);
#endif
	return(output);
}

float4 PSTexturedLighting(VS_TEXTURED_LIGHTING_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID) : SV_TARGET
{
	float4 cTexture = gtxtDiffuse.SampleLevel(gSamplerState, input.uv, 0);
#ifdef _WITH_VERTEX_LIGHTING
	float4 cIllumination = input.color;
#else
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);
#endif
//	return(cTexture * cIllumination);
	return(lerp(cTexture, cIllumination, 0.35f));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_NORMALMAP_TEXTURED_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
};

struct VS_NORMALMAP_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float3 tangentW : TANGENT;
	float3 bitangentW : BITANGENT;
	float2 uv : TEXCOORD;
#ifdef _WITH_VERTEX_LIGHTING
	float4 color : COLOR;
#endif
};

VS_NORMALMAP_TEXTURED_OUTPUT VSTexturedNormalMapLighting(VS_NORMALMAP_TEXTURED_INPUT input)
{
	VS_NORMALMAP_TEXTURED_OUTPUT output;

	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.tangentW = (float3)mul(float4(input.tangent, 1.0f), gmtxGameObject);
	output.bitangentW = (float3)mul(float4(input.bitangent, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

#ifdef _WITH_VERTEX_LIGHTING
	float3x3 TBN = float3x3(normalize(output.tangentW), normalize(output.bitangentW), normalize(output.normalW));

	float4 cNormal = gtxtNormal.Sample(gSamplerState, input.uv);
	float3 vNormal = normalize(cNormal.rgb * 2.0f - 1.0f); //[0, 1] ¡æ [-1, 1]

	output.normalW = normalize(mul(vNormal, TBN));
	output.color = Lighting(input.positionW, output.normalW);
#endif

	return(output);
}

Texture2D gtxtNormal : register(t1);

float4 PSTexturedNormalMapLighting(VS_NORMALMAP_TEXTURED_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID) : SV_TARGET
{
	float4 cTexture = gtxtDiffuse.SampleLevel(gSamplerState, input.uv, 0);

#ifdef _WITH_VERTEX_LIGHTING
	float4 cIllumination = input.color;
#else
	float3x3 TBN = float3x3(normalize(input.tangentW), normalize(input.bitangentW), normalize(input.normalW));

	float4 cNormal = gtxtNormal.SampleLevel(gSamplerState, input.uv, 0);
	float3 vNormal = normalize(cNormal.rgb * 2.0f - 1.0f); //[0, 1] ¡æ [-1, 1]
	float3 normalW = normalize(mul(vNormal, TBN));

	float4 cIllumination = Lighting(input.positionW, normalW);
#endif

//	return(cTexture * cIllumination);
	return(lerp(cTexture, cIllumination, 0.35f));
}
