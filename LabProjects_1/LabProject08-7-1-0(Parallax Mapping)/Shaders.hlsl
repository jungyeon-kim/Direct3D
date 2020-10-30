cbuffer cbPlayerInfo : register(b0)
{
	matrix		gmtxPlayerWorld : packoffset(c0);
};

cbuffer cbCameraInfo : register(b1)
{
	matrix		gmtxView : packoffset(c0);
	matrix		gmtxProjection : packoffset(c4);
	float3		gf3CameraPosition : packoffset(c8);
};

cbuffer cbGameObjectInfo : register(b2)
{
	matrix		gmtxGameObject : packoffset(c0);
	matrix		gmtxInverseGameObject : packoffset(c4);
	uint		gnMaterial : packoffset(c8);
};

static float4 gf4TextureMapping = { 2.0f, 2.0f, 0.0f, 0.0f }; //xy: uv Scale, zw: uv Offset

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
};

VS_TEXTURED_LIGHTING_OUTPUT VSTexturedLighting(VS_TEXTURED_LIGHTING_INPUT input)
{
	VS_TEXTURED_LIGHTING_OUTPUT output;

	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

float4 PSTexturedLighting(VS_TEXTURED_LIGHTING_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID) : SV_TARGET
{
	float4 cTexture = gtxtDiffuse.SampleLevel(gSamplerState, input.uv * gf4TextureMapping.xy + gf4TextureMapping.zw, 0);
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);

	return(cTexture * cIllumination);
	//return(lerp(cTexture, cIllumination, 0.5f));
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
};

VS_NORMALMAP_TEXTURED_OUTPUT VSTexturedNormalMapLighting(VS_NORMALMAP_TEXTURED_INPUT input)
{
	VS_NORMALMAP_TEXTURED_OUTPUT output;

	output.positionW = mul(float4(input.position, 1.0f), gmtxGameObject).xyz;
	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.tangentW = mul(input.tangent, (float3x3)gmtxGameObject);
	output.bitangentW = mul(input.bitangent, (float3x3)gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

Texture2D gtxtNormal : register(t1);

float4 PSTexturedNormalMapLighting(VS_NORMALMAP_TEXTURED_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID) : SV_TARGET
{
	float2 uv = input.uv * gf4TextureMapping.xy + gf4TextureMapping.zw;
	float4 cTexture = gtxtDiffuse.SampleLevel(gSamplerState, uv, 0);

	float3x3 TBN = float3x3(normalize(input.tangentW), normalize(input.bitangentW), normalize(input.normalW));

	float4 cNormal = gtxtNormal.SampleLevel(gSamplerState, uv, 0);
	float3 vNormal = cNormal.rgb * 2.0f - 1.0f; //[0, 1] ¡æ [-1, 1]
	float3 normalW = normalize(mul(vNormal, TBN));

	float4 cIllumination = Lighting(input.positionW, normalW);

	return(cTexture * cIllumination);
}

struct VS_PARALLAXMAP_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float2 uv : TEXCOORD0;
	float3 normalW : NORMAL;
	float3 tangentW : TANGENT;
	float3 bitangentW : BITANGENT;
	float3 toCameraW : TEXCOORD1; 
	float3 toCamera : TEXCOORD2;
};

VS_PARALLAXMAP_TEXTURED_OUTPUT VSTexturedNormalParallaxMapLighting(VS_NORMALMAP_TEXTURED_INPUT input)
{
	VS_PARALLAXMAP_TEXTURED_OUTPUT output;

	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject).xyz;
	output.normalW = normalize(mul(gmtxInverseGameObject, float4(input.normal, 0.0f)).xyz);
//	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.tangentW = mul(input.tangent, (float3x3)gmtxGameObject);
	output.bitangentW = cross(output.normalW, output.tangentW);
//	output.bitangentW = mul(input.bitangent, (float3x3)gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

	float3 f3ToCameraModelSpace = mul(float4(gf3CameraPosition, 1.0), gmtxInverseGameObject).xyz - input.position.xyz;
	float3x3 TBN = float3x3(input.tangent, input.bitangent, input.normal); 
	output.toCamera = mul(TBN, f3ToCameraModelSpace); 

	output.toCameraW = normalize(gf3CameraPosition - output.positionW);

	return(output);
}

Texture2D gtxtHeight : register(t2);

static float gfParallaxScale = 0.004f;
static float gfParallaxBias = -0.5f;
static float gfOcclusion = 0.3f;

float4 PSTexturedNormalParallaxMapLighting(VS_PARALLAXMAP_TEXTURED_OUTPUT input) : SV_TARGET
{
	float fHeight = (gtxtHeight.Sample(gSamplerState, input.uv * gf4TextureMapping.xy + gf4TextureMapping.zw).r + gfParallaxBias) * gfParallaxScale;
	//float fHeight = (gtxtHeight.SampleLevel(gSamplerState, input.uv * gf4TextureMapping.xy + gf4TextureMapping.zw, 0).r + gfParallaxBias) * gfParallaxScale;
//	float2 f2Offset = fHeight * (input.toCamera.xy / input.toCamera.zz); 
	float2 f2Offset = clamp(fHeight * (input.toCamera.xy / input.toCamera.zz), -0.02f, +0.02f); 

	float2 uv = (input.uv + f2Offset) * gf4TextureMapping.xy + gf4TextureMapping.zw;

	//float4 cDiffuse = gtxtDiffuse.SampleLevel(gSamplerState, uv, 0);
	//float4 cNormal = gtxtNormal.SampleLevel(gSamplerState, uv, 0);

	float4 cDiffuse = gtxtDiffuse.Sample(gSamplerState, uv);
	float4 cNormal = gtxtNormal.Sample(gSamplerState, uv);

	float3x3 TBN = float3x3(normalize(input.tangentW), normalize(input.bitangentW), normalize(input.normalW));

//	float3 f3Normal = float3(2.0f * cNormal.r - 1.0f, 2.0f * cNormal.g - 1.0f, 0.0f);
//	f3Normal.z = sqrt(1.0f - dot(f3Normal, f3Normal));
//	float3 normalW = normalize(mul(f3Normal, TBN));

	float3 vNormal = cNormal.rgb * 2.0f - 1.0f; //[0, 1] ¡æ [-1, 1]
	float3 normalW = normalize(mul(vNormal, TBN));

	float4 cIllumination = Lighting(input.positionW, normalW);

	float4 cOcclusion = gtxtHeight.SampleLevel(gSamplerState, uv, 0);
	float4 cColor = cDiffuse * cIllumination;

	return(lerp(cColor, cColor * cOcclusion, gfOcclusion));
//	return(cDiffuse * cIllumination);
}
