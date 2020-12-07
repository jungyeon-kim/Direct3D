cbuffer cbPlayerInfo : register(b0)
{
	matrix		gmtxPlayerWorld : packoffset(c0);
};

cbuffer cbCameraInfo : register(b1)
{
	matrix		gmtxView : packoffset(c0);
	matrix		gmtxProjection : packoffset(c4);
	matrix		gmtxViewProjection : packoffset(c8);
	float3		gvCameraPosition : packoffset(c12);
};

cbuffer cbGameObjectInfo : register(b2)
{
	matrix		gmtxWorld : packoffset(c0);
	uint		gnMaterialID : packoffset(c8);
};

#include "Light.hlsl"

cbuffer cbFrameworkInfo : register(b5)
{
	float		gfCurrentTime : packoffset(c0.x);
	float		gfElapsedTime : packoffset(c0.y);
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

VS_DIFFUSED_OUTPUT VSDiffused(VS_DIFFUSED_INPUT input)
{
	VS_DIFFUSED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProjection);
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
Texture2D gtxtTexture : register(t0);

SamplerState gWrapSamplerState : register(s0);
SamplerState gClampSamplerState : register(s1);

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

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

float4 PSTextured(VS_TEXTURED_OUTPUT input, uint primitiveID : SV_PrimitiveID) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(gWrapSamplerState, input.uv);

	return(cColor);
}

//--------------------------------------------------------------------------------------
// Tessellation
//--------------------------------------------------------------------------------------

struct VS_QUAD_PATCH_INPUT
{
	float3	position    : POSITION;
};

struct VS_QUAD_PATCH_OUTPUT
{
	float3	position    : POSITION;
};

struct HS_QUAD_PATCH_CONSTANT
{
	float fTessEdges[4] : SV_TessFactor;
	float fTessInsides[2] : SV_InsideTessFactor;
};

struct HS_QUAD_PATCH_OUTPUT
{
	float3	position    : POSITION;
};

struct DS_QUAD_PATCH_OUTPUT
{
	float4	position    : SV_POSITION;
};

float4 BernsteinCoeffcient4x4(float t)
{
	float tInv = 1.0f - t;
	float4 f4Bernstein = float4(tInv * tInv * tInv, 3.0f * t * tInv * tInv, 3.0f * t * t * tInv, t * t * t);
	return(f4Bernstein);
}

void BernsteinCoeffcient5x5(float t, out float fBernstein[5])
{
	float tInv = 1.0f - t;
	fBernstein[0] = tInv * tInv * tInv * tInv;
	fBernstein[1] = 4.0f * t * tInv * tInv * tInv;
	fBernstein[2] = 6.0f * t * t * tInv * tInv;
	fBernstein[3] = 4.0f * t * t * t * tInv;
	fBernstein[4] = t * t * t * t;
}

float3 CubicBezierSum4x4(OutputPatch<HS_QUAD_PATCH_OUTPUT, 16> patch, float4 uB, float4 vB)
{
	float3 sum = float3(0.0f, 0.0f, 0.0f);
	sum = vB.x * (uB.x * patch[0].position + uB.y * patch[1].position + uB.z * patch[2].position + uB.w * patch[3].position);
	sum += vB.y * (uB.x * patch[4].position + uB.y * patch[5].position + uB.z * patch[6].position + uB.w * patch[7].position);
	sum += vB.z * (uB.x * patch[8].position + uB.y * patch[9].position + uB.z * patch[10].position + uB.w * patch[11].position);
	sum += vB.w * (uB.x * patch[12].position + uB.y * patch[13].position + uB.z * patch[14].position + uB.w * patch[15].position);
	return(sum);
}

float3 CubicBezierSum5x5(OutputPatch<HS_QUAD_PATCH_OUTPUT, 25> patch, float uB[5], float vB[5])
{
	float3 sum = float3(0.0f, 0.0f, 0.0f);
	sum = vB[0] * (uB[0] * patch[0].position + uB[1] * patch[1].position + uB[2] * patch[2].position + uB[3] * patch[3].position + uB[4] * patch[4].position);
	sum += vB[1] * (uB[0] * patch[5].position + uB[1] * patch[6].position + uB[2] * patch[7].position + uB[3] * patch[8].position + uB[4] * patch[9].position);
	sum += vB[2] * (uB[0] * patch[10].position + uB[1] * patch[11].position + uB[2] * patch[12].position + uB[3] * patch[13].position + uB[4] * patch[14].position);
	sum += vB[3] * (uB[0] * patch[15].position + uB[1] * patch[16].position + uB[2] * patch[17].position + uB[3] * patch[18].position + uB[4] * patch[19].position);
	sum += vB[4] * (uB[0] * patch[20].position + uB[1] * patch[21].position + uB[2] * patch[22].position + uB[3] * patch[23].position + uB[4] * patch[24].position);
	return(sum);
}

VS_QUAD_PATCH_OUTPUT VSQuadPatch(VS_QUAD_PATCH_INPUT input)
{
	VS_QUAD_PATCH_OUTPUT output = (VS_QUAD_PATCH_OUTPUT)0;
	output.position = input.position;

	return(output);
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(16)]
[patchconstantfunc("HSQuadPatchConstant")]
[maxtessfactor(64.0f)]
HS_QUAD_PATCH_OUTPUT HSQuadPatch(InputPatch<VS_QUAD_PATCH_OUTPUT, 16> input, uint i : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
	HS_QUAD_PATCH_OUTPUT output;

	output.position = input[i].position;

	return(output);
}

HS_QUAD_PATCH_CONSTANT HSQuadPatchConstant(InputPatch<VS_QUAD_PATCH_OUTPUT, 16> input, uint nPatchID : SV_PrimitiveID)
{
	HS_QUAD_PATCH_CONSTANT output;

	output.fTessEdges[0] = 20.0f;
	output.fTessEdges[1] = 20.0f;
	output.fTessEdges[2] = 20.0f;
	output.fTessEdges[3] = 20.0f;

	output.fTessInsides[0] = 20.0f;
	output.fTessInsides[1] = 20.0f;

	return(output);
}

[domain("quad")]
DS_QUAD_PATCH_OUTPUT DSQuadPatch(HS_QUAD_PATCH_CONSTANT input, float2 uv : SV_DomainLocation, OutputPatch<HS_QUAD_PATCH_OUTPUT, 16> patch)
{
	DS_QUAD_PATCH_OUTPUT output = (DS_QUAD_PATCH_OUTPUT)0;

	float4 uB = BernsteinCoeffcient4x4(uv.x);
	float4 vB = BernsteinCoeffcient4x4(uv.y);
	float3 position = CubicBezierSum4x4(patch, uB, vB);

	matrix mtxWorldViewProjection = mul(mul(gmtxWorld, gmtxView), gmtxProjection);
	output.position = mul(float4(position, 1.0f), mtxWorldViewProjection);

	return(output);
}

float4 PSQuadPatch(DS_QUAD_PATCH_OUTPUT input) : SV_Target
{
	return(float4(1.0f, 0.0f, 0.0f, 1.0f));
}
