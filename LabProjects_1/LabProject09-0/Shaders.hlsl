///////////////////////////////////////////////////////////////////////////////
//
Texture2D gtxtInput : register(t0);

RWTexture2D<float4> gtxtRWOutput : register(u0);
Texture2D gtxtOutput : register(t1);

SamplerState gSamplerState : register(s0);

static float3 gf3ToLuminance = float3(0.3f, 0.59f, 0.11f);

#define _WITH_BY_LUMINANCE

//////////////////////////////////////////////////////////////////////////////////
// Sobel Edge Detection
//
void SobelEdge(int3 nDispatchID : SV_DispatchThreadID)
{
	if ((nDispatchID.x < 1) || (nDispatchID.x >= int(gtxtInput.Length.x - 1)) || (nDispatchID.y < 1) || (nDispatchID.y >= int(gtxtInput.Length.y - 1))) 
	{
		gtxtRWOutput[nDispatchID.xy] = gtxtInput[nDispatchID.xy];
	}
	else
	{
#ifdef _WITH_BY_LUMINANCE
		float fHorizontalEdge = (-1.0f * dot(gf3ToLuminance, gtxtInput[int2(nDispatchID.x - 1, nDispatchID.y)].rgb)) + (2.0f * dot(gf3ToLuminance, gtxtInput[int2(nDispatchID.x, nDispatchID.y)].rgb)) + (-1.0f * dot(gf3ToLuminance, gtxtInput[int2(nDispatchID.x + 1, nDispatchID.y)].rgb));	
		float fVerticalEdge = (-1.0f * dot(gf3ToLuminance, gtxtInput[int2(nDispatchID.x, nDispatchID.y - 1)].rgb)) + (2.0f * dot(gf3ToLuminance, gtxtInput[int2(nDispatchID.x, nDispatchID.y)].rgb)) + (-1.0f * dot(gf3ToLuminance, gtxtInput[int2(nDispatchID.x, nDispatchID.y + 1)].rgb));	
#else
		float3 f3HorizontalEdge = (-1.0f * gtxtInput[int2(nDispatchID.x - 1, nDispatchID.y)].rgb) + (2.0f * gtxtInput[int2(nDispatchID.x, nDispatchID.y)].rgb) + (-1.0f * gtxtInput[int2(nDispatchID.x + 1, nDispatchID.y)].rgb);	
		float3 f3VerticalEdge = (-1.0f * gtxtInput[int2(nDispatchID.x, nDispatchID.y - 1)].rgb) + (2.0f * gtxtInput[int2(nDispatchID.x, nDispatchID.y)].rgb) + (-1.0f * gtxtInput[int2(nDispatchID.x, nDispatchID.y + 1)].rgb);	
#endif
#ifdef _WITH_BY_LUMINANCE
		float3 f3Edge = sqrt(fHorizontalEdge * fHorizontalEdge + fVerticalEdge * fVerticalEdge);
		gtxtRWOutput[nDispatchID.xy] = float4(f3Edge, 1.0f);
//		gtxtRWOutput[nDispatchID.xy] = gtxtInput[nDispatchID.xy] * float4(f3Edge, 1.0f);
#else
		gtxtRWOutput[nDispatchID.xy] = gtxtInput[nDispatchID.xy] * float4(sqrt(f3HorizontalEdge*f3HorizontalEdge + f3VerticalEdge+f3VerticalEdge), 1.0f);
#endif
	}
}

//////////////////////////////////////////////////////////////////////////////////
// Laplacian Edge Detection
//
static float gfLaplacians[9] = { -1.0f, -1.0f, -1.0f, -1.0f, 8.0f, -1.0f, -1.0f, -1.0f, -1.0f };
static int2 gnOffsets[9] = { { -1,-1 }, { 0,-1 }, { 1,-1 }, { -1,0 }, { 0,0 }, { 1,0 }, { -1,1 }, { 0,1 }, { 1,1 } };

void LaplacianEdge(int3 nDispatchID : SV_DispatchThreadID)
{
	float3 cEdgeness = float3(0.0f, 0.0f, 0.0f);
	if ((uint(nDispatchID.x) >= 1) || (uint(nDispatchID.y) >= 1) || (uint(nDispatchID.x) <= gtxtInput.Length.x - 2) || (uint(nDispatchID.y) <= gtxtInput.Length.y - 2))
	{
		for (int i = 0; i < 9; i++)
		{
			cEdgeness += gfLaplacians[i] * dot(gf3ToLuminance, gtxtInput[int2(nDispatchID.xy) + gnOffsets[i]].xyz);
		}
	}
	float3 cColor = lerp(gtxtInput[int2(nDispatchID.xy)].rgb, cEdgeness, 0.95f);

	gtxtRWOutput[nDispatchID.xy] = float4(cColor, 1.0f);
}

#define _WITH_SOBEL_EDGE

[numthreads(32, 32, 1)]
void CSEdgeDetection(int3 nDispatchID : SV_DispatchThreadID)
{
#ifdef _WITH_SOBEL_EDGE
	SobelEdge(nDispatchID);
#else
	LaplacianEdge(nDispatchID);
#endif
}

///////////////////////////////////////////////////////////////////////////////
//
struct VS_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VS_TEXTURED_OUTPUT VSTextureToFullScreen(uint nVertexID : SV_VertexID)
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

float4 PSTextureToFullScreen(VS_TEXTURED_OUTPUT input) : SV_Target
{
	float4 cColor = gtxtOutput.Sample(gSamplerState, input.uv);

	return(cColor);
}
