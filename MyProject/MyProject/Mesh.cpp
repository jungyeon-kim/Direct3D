#include "stdafx.h"
#include "Mesh.h"

CMesh::CMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList)
{
}

CMesh::~CMesh()
{
	if (VertexBuffer) VertexBuffer->Release();
	if (VertexUploadBuffer) VertexUploadBuffer->Release();
}

void CMesh::ReleaseUploadBuffers()
{
	// 정점 버퍼를 위한 업로드 버퍼를 소멸
	if (VertexUploadBuffer)
	{
		VertexUploadBuffer->Release();
		VertexUploadBuffer = nullptr;
	}
};

void CMesh::Render(ID3D12GraphicsCommandList* CommandList)
{
	// 메쉬의 프리미티브 유형 설정
	CommandList->IASetPrimitiveTopology(PrimitiveTopology);
	// 메쉬의 정점 버퍼 뷰 설정
	CommandList->IASetVertexBuffers(StartSlot, 1, &VertexBufferView);
	// 메쉬의 정점 버퍼 뷰를 렌더링 (파이프라인(입력 조립기) 작동)
	CommandList->DrawInstanced(NumOfVertex, 1, Offset, 0);
}

CTriangleMesh::CTriangleMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList) : CMesh(Device, CommandList)
{
	NumOfVertex = 3;
	VertexSize = sizeof(CDiffusedVertex);
	PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CDiffusedVertex Vertex[3]{};
	Vertex[0] = CDiffusedVertex(XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	Vertex[1] = CDiffusedVertex(XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
	Vertex[2] = CDiffusedVertex(XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(Colors::Blue));

	// 삼각형 메쉬를 리소스(정점 버퍼)로 생성
	VertexBuffer = CreateBufferResource(Device, CommandList, Vertex, VertexSize * NumOfVertex,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &VertexUploadBuffer);

	// 정점 버퍼 뷰 설정
	VertexBufferView.BufferLocation = VertexBuffer->GetGPUVirtualAddress();
	VertexBufferView.StrideInBytes = VertexSize;
	VertexBufferView.SizeInBytes = VertexSize * NumOfVertex;
}