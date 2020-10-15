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
	// ���� ���۸� ���� ���ε� ���۸� �Ҹ�
	if (VertexUploadBuffer)
	{
		VertexUploadBuffer->Release();
		VertexUploadBuffer = nullptr;
	}
};

void CMesh::Render(ID3D12GraphicsCommandList* CommandList)
{
	// �޽��� ������Ƽ�� ���� ����
	CommandList->IASetPrimitiveTopology(PrimitiveTopology);
	// �޽��� ���� ���� �� ����
	CommandList->IASetVertexBuffers(StartSlot, 1, &VertexBufferView);
	// �޽��� ���� ���� �並 ������ (����������(�Է� ������) �۵�)
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

	// �ﰢ�� �޽��� ���ҽ�(���� ����)�� ����
	VertexBuffer = CreateBufferResource(Device, CommandList, Vertex, VertexSize * NumOfVertex,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &VertexUploadBuffer);

	// ���� ���� �� ����
	VertexBufferView.BufferLocation = VertexBuffer->GetGPUVirtualAddress();
	VertexBufferView.StrideInBytes = VertexSize;
	VertexBufferView.SizeInBytes = VertexSize * NumOfVertex;
}