#include "stdafx.h"

// 버퍼 리소스를 생성하는 함수. 버퍼의 힙 유형에 따라 버퍼 리소스를 생성하고 초기화 데이터가 있으면 초기화
ID3D12Resource* CreateBufferResource(ID3D12Device* Device, ID3D12GraphicsCommandList
	* CommandList, void* Data, UINT NumOfByte, D3D12_HEAP_TYPE HeapType,
	D3D12_RESOURCE_STATES ResourceStates, ID3D12Resource** UploadBuffer)
{
	// 실제 리소스를 저장할 버퍼
	ID3D12Resource* Buffer{};

	// 힙 속성을 설명하는 구조체 설정
	D3D12_HEAP_PROPERTIES HeapPropertiesDesc{};
	HeapPropertiesDesc.Type = HeapType;
	HeapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	HeapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	HeapPropertiesDesc.CreationNodeMask = 1;
	HeapPropertiesDesc.VisibleNodeMask = 1;

	// 리소스를 설명하는 구조체 설정
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

	// 리소스 초기 상태를 설정 (조건문은 업로드 힙과 리드백 힙에 대하여 각각 초기 리소스 상태로 설정)
	D3D12_RESOURCE_STATES ResourceInitialStates{ D3D12_RESOURCE_STATE_COPY_DEST };
	if (HeapType == D3D12_HEAP_TYPE_UPLOAD) ResourceInitialStates = D3D12_RESOURCE_STATE_GENERIC_READ;
	else if (HeapType == D3D12_HEAP_TYPE_READBACK) ResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;

	// 빈 리소스를 힙에 생성
	Device->CreateCommittedResource(&HeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &ResourceDesc, 
		ResourceInitialStates, NULL, __uuidof(ID3D12Resource), (void**)&Buffer);

	// 리소스를 저장할 힙의 종류에 따라 처리
	if (Data)
		switch (HeapType)
		{
		case D3D12_HEAP_TYPE_DEFAULT:
		{
			if (UploadBuffer)
			{
				// 업로드 버퍼에 리소스를 생성
				HeapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
				Device->CreateCommittedResource(&HeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &ResourceDesc, 
					D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)UploadBuffer);
				// 업로드 버퍼를 매핑하여 데이터를 업로드 버퍼에 복사
				D3D12_RANGE ReadRange{ 0, 0 };
				UINT8* BufferDataBegin{};
				(*UploadBuffer)->Map(0, &ReadRange, (void**)&BufferDataBegin);
				memcpy(BufferDataBegin, Data, NumOfByte);
				(*UploadBuffer)->Unmap(0, NULL);
				// 업로드 버퍼의 내용을 디폴트 버퍼에 복사
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
			// 업로드 버퍼를 매핑하여 초기화 데이터를 업로드 버퍼에 복사
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