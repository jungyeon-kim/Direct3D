#pragma once

//������ ǥ���ϱ� ���� Ŭ������ �����Ѵ�. 
class CVertex
{
protected:
	//������ ��ġ �����̴�(��� ������ �ּ��� ��ġ ���͸� ������ �Ѵ�). 
	XMFLOAT3 m_xmf3Position{};
public:
	CVertex() = default;
	CVertex(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	~CVertex() = default;
};

class CDiffusedVertex : public CVertex
{
protected:
	//������ �����̴�. 
	XMFLOAT4 m_xmf4Diffuse{};
public:
	CDiffusedVertex() = default;
	CDiffusedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse)
	{
		m_xmf3Position = XMFLOAT3(x, y, z); 
		m_xmf4Diffuse = xmf4Diffuse;
	}
	CDiffusedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse) 
	{
		m_xmf3Position = xmf3Position; 
		m_xmf4Diffuse = xmf4Diffuse;
	}
	~CDiffusedVertex() = default;
};

class CMesh
{
private:
	int m_nReferences{};
protected:
	ID3D12Resource* m_pd3dVertexBuffer{};
	ID3D12Resource* m_pd3dVertexUploadBuffer{};
	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView{};
	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT m_nSlot{};
	UINT m_nVertices{};
	UINT m_nStride{};
	UINT m_nOffset{};

protected:
	/*�ε��� ����(�ε����� �迭)�� �ε��� ���۸� ���� ���ε� ���ۿ� ���� �������̽� �������̴�. �ε��� ���۴� ����
	����(�迭)�� ���� �ε����� ������.*/
	ID3D12Resource* m_pd3dIndexBuffer{};
	ID3D12Resource* m_pd3dIndexUploadBuffer{};
	D3D12_INDEX_BUFFER_VIEW m_d3dIndexBufferView{};
	//�ε��� ���ۿ� ���ԵǴ� �ε����� �����̴�. 
	UINT m_nIndices{};
	//�ε��� ���ۿ��� �޽��� �׸��� ���� ���Ǵ� ���� �ε����̴�. 
	UINT m_nStartIndex{};
	//�ε��� ������ �ε����� ������ �ε����̴�
	int m_nBaseVertex{};
public:
	CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CMesh();

	void AddRef() { ++m_nReferences; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nInstances);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nInstances, D3D12_VERTEX_BUFFER_VIEW d3dInstancingBufferView);
};

class CTriangleMesh : public CMesh
{
public:
	CTriangleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CTriangleMesh() = default;
};

class CCubeMeshDiffused : public CMesh
{
public:
	//������ü�� ����, ����, ������ ���̸� �����Ͽ� ������ü �޽��� �����Ѵ�. 
	CCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
		float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshDiffused() = default;
};

class CAirplaneMeshDiffused : public CMesh
{
public:
	CAirplaneMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 4.0f,
		XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f));
	virtual ~CAirplaneMeshDiffused();
};