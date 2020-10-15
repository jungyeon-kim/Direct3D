#pragma once

class CVertex
{
protected:
	XMFLOAT3 Position{};
public:
	CVertex() = default;
	CVertex(XMFLOAT3 NewPosition) { Position = NewPosition; }
	~CVertex() = default;
};

class CDiffusedVertex : public CVertex
{
protected:
	XMFLOAT4 Color{};
public:
	CDiffusedVertex() = default;
	CDiffusedVertex(float x, float y, float z, XMFLOAT4 NewDiffuse)
	{
		Position = XMFLOAT3{ x, y, z };
		Color = NewDiffuse;
	}
	CDiffusedVertex(XMFLOAT3 NewPosition, XMFLOAT4 NewDiffuse)
	{
		Position = NewPosition;
		Color = NewDiffuse;
	}
	~CDiffusedVertex() = default;
};

class CMesh
{
private:
	int RefCount{};
protected:
	ID3D12Resource* VertexBuffer{};
	ID3D12Resource* VertexUploadBuffer{};

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView{};
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

	UINT StartSlot{};
	UINT NumOfVertex{};
	UINT VertexSize{};
	UINT Offset{};
public:
	CMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);
	virtual ~CMesh();

	void AddRef() { ++RefCount; }
	void Release() { if (--RefCount <= 0) delete this; }
	void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
};

class CTriangleMesh : public CMesh
{
public:
	CTriangleMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);
	virtual ~CTriangleMesh() = default;
};