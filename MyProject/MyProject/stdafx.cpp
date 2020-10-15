#include "stdafx.h"

// ���� ���ҽ��� �����ϴ� �Լ�. ������ �� ������ ���� ���� ���ҽ��� �����ϰ� �ʱ�ȭ �����Ͱ� ������ �ʱ�ȭ
ID3D12Resource* CreateBufferResource(ID3D12Device* Device, ID3D12GraphicsCommandList
	* CommandList, void* Data, UINT NumOfByte, D3D12_HEAP_TYPE HeapType,
	D3D12_RESOURCE_STATES ResourceStates, ID3D12Resource** UploadBuffer)
{
	// ���� ���ҽ��� ������ ����
	ID3D12Resource* Buffer{};

	// �� �Ӽ��� �����ϴ� ����ü ����
	D3D12_HEAP_PROPERTIES HeapPropertiesDesc{};
	HeapPropertiesDesc.Type = HeapType;
	HeapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	HeapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	HeapPropertiesDesc.CreationNodeMask = 1;
	HeapPropertiesDesc.VisibleNodeMask = 1;

	// ���ҽ��� �����ϴ� ����ü ����
	D3D12_RESOURCE_DESC ResourceDesc{};
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Alignment = 0;
	ResourceDesc.Width = NumOfByte;
	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	ResourceDesc.SampleDesc.Count = 1;
	ResourceDesc.SampleDesc.Quality = 0;
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// ���ҽ� �ʱ� ���¸� ���� (���ǹ��� ���ε� ���� ����� ���� ���Ͽ� ���� �ʱ� ���ҽ� ���·� ����)
	D3D12_RESOURCE_STATES ResourceInitialStates{ D3D12_RESOURCE_STATE_COPY_DEST };
	if (HeapType == D3D12_HEAP_TYPE_UPLOAD) ResourceInitialStates = D3D12_RESOURCE_STATE_GENERIC_READ;
	else if (HeapType == D3D12_HEAP_TYPE_READBACK) ResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;

	// �� ���ҽ��� ���� ����
	Device->CreateCommittedResource(&HeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &ResourceDesc, 
		ResourceInitialStates, NULL, __uuidof(ID3D12Resource), (void**)&Buffer);

	// ���ҽ��� ������ ���� ������ ���� ó��
	if (Data)
		switch (HeapType)
		{
		case D3D12_HEAP_TYPE_DEFAULT:
		{
			if (UploadBuffer)
			{
				// ���ε� ���ۿ� ���ҽ��� ����
				HeapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
				Device->CreateCommittedResource(&HeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &ResourceDesc, 
					D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)UploadBuffer);
				// ���ε� ���۸� �����Ͽ� �����͸� ���ε� ���ۿ� ����
				D3D12_RANGE ReadRange{ 0, 0 };
				UINT8* BufferDataBegin{};
				(*UploadBuffer)->Map(0, &ReadRange, (void**)&BufferDataBegin);
				memcpy(BufferDataBegin, Data, NumOfByte);
				(*UploadBuffer)->Unmap(0, NULL);
				// ���ε� ������ ������ ����Ʈ ���ۿ� ����
				CommandList->CopyResource(Buffer, *UploadBuffer);
				D3D12_RESOURCE_BARRIER ResourceBarrier{};
				ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				ResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				ResourceBarrier.Transition.pResource = Buffer;
				ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
				ResourceBarrier.Transition.StateAfter = ResourceStates;
				ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				CommandList->ResourceBarrier(1, &ResourceBarrier);
			}
			break;
		}
		case D3D12_HEAP_TYPE_UPLOAD:
		{
			// ���ε� ���۸� �����Ͽ� �ʱ�ȭ �����͸� ���ε� ���ۿ� ����
			D3D12_RANGE ReadRange{ 0, 0 };
			UINT8* BufferDataBegin{};
			Buffer->Map(0, &ReadRange, (void**)&BufferDataBegin);
			memcpy(BufferDataBegin, Data, NumOfByte);
			Buffer->Unmap(0, nullptr);
			break;
		}
		case D3D12_HEAP_TYPE_READBACK:
			break;
		}

	return Buffer;
}