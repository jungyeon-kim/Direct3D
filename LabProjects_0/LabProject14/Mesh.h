#pragma once

#define _WITH_APPROXIMATE_OPPOSITE_CORNER

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

class CHeightMapImage
{
private:
	//���� �� �̹��� �ȼ�(8-��Ʈ)���� ������ �迭�̴�. �� �ȼ��� 0~255�� ���� ���´�. 
	BYTE* m_pHeightMapPixels{};
	//���� �� �̹����� ���ο� ���� ũ���̴�. 
	int m_nWidth{};
	int m_nLength{};
	//���� �� �̹����� ������ �� �� Ȯ���Ͽ� ����� ���ΰ��� ��Ÿ���� ������ �����̴�. 
	XMFLOAT3 m_xmf3Scale{};
public:
	CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale);
	~CHeightMapImage(void);

	//���� �� �̹������� (x, z) ��ġ�� �ȼ� ���� ����� ������ ���̸� ��ȯ�Ѵ�. 
	float GetHeight(float x, float z);
	//���� �� �̹������� (x, z) ��ġ�� ���� ���͸� ��ȯ�Ѵ�. 
	XMFLOAT3 GetHeightMapNormal(int x, int z);
	XMFLOAT3 GetScale() { return m_xmf3Scale; }
	BYTE* GetHeightMapPixels() { return m_pHeightMapPixels; }
	int GetHeightMapWidth() { return m_nWidth; }
	int GetHeightMapLength() { return m_nLength; }
};

class CHeightMapGridMesh : public CMesh
{
protected:
	//������ ũ��(����: x-����, ����: z-����)�̴�. 
	int m_nWidth;
	int m_nLength;
	/*������ ������(����: x-����, ����: z-����, ����: y-����) �����̴�.
	���� ���� �޽��� �� ������ x-��ǥ, y-��ǥ, z-��ǥ�� ������ ������ x-��ǥ, y-��ǥ, z-��ǥ�� ���� ���� ���´�.
	��, ���� ������ x-�� ������ ������ 1�� �ƴ϶� ������ ������ x-��ǥ�� �ȴ�.
	�̷��� �ϸ� ���� ����(���� ����)�� ����ϴ��� ū ũ���� ����(����)�� ������ �� �ִ�.*/
	XMFLOAT3 m_xmf3Scale;
public:
	CHeightMapGridMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
		int xStart, int zStart, int nWidth, int nLength, XMFLOAT3 xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f), 
		XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f), void* pContext = nullptr);
	virtual ~CHeightMapGridMesh();

	XMFLOAT3 GetScale() { return m_xmf3Scale; }
	int GetWidth() { return m_nWidth; }
	int GetLength() { return m_nLength; }
	//������ ��ǥ�� (x, z)�� �� ����(����)�� ���̸� ��ȯ�ϴ� �Լ��̴�. 
	virtual float OnGetHeight(int x, int z, void *pContext);
	//������ ��ǥ�� (x, z)�� �� ����(����)�� ������ ��ȯ�ϴ� �Լ��̴�. 
	virtual XMFLOAT4 OnGetColor(int x, int z, void *pContext);
};