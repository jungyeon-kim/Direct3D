#include "stdafx.h"

using namespace std;
using namespace chrono;

// ���� ���ҽ��� �����ϴ� �Լ��̴�. ������ �� ������ ���� ���� ���ҽ��� �����ϰ� �ʱ�ȭ �����Ͱ� ������ �ʱ�ȭ �Ѵ�.
ID3D12Resource* CreateBufferResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
	* pd3dCommandList, void* pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType,
	D3D12_RESOURCE_STATES d3dResourceStates, ID3D12Resource** ppd3dUploadBuffer)
{
	ID3D12Resource* pd3dBuffer{};

	// �� �Ӽ��� �����ϴ� ����ü ����
	D3D12_HEAP_PROPERTIES d3dHeapPropertiesDesc;
	::ZeroMemory(&d3dHeapPropertiesDesc, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapPropertiesDesc.Type = d3dHeapType;
	d3dHeapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapPropertiesDesc.CreationNodeMask = 1;
	d3dHeapPropertiesDesc.VisibleNodeMask = 1;

	// ���ҽ��� �����ϴ� ����ü ����
	D3D12_RESOURCE_DESC d3dResourceDesc;
	::ZeroMemory(&d3dResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = nBytes;
	d3dResourceDesc.Height = 1;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	d3dResourceDesc.SampleDesc.Count = 1;
	d3dResourceDesc.SampleDesc.Quality = 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// ���ҽ� �ʱ� ���¸� ���� (���ǹ��� ���ε� ���� ����� ���� ���Ͽ� ���� �ʱ� ���ҽ� ���·� ����)
	D3D12_RESOURCE_STATES d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;
	if (d3dHeapType == D3D12_HEAP_TYPE_UPLOAD) d3dResourceInitialStates = D3D12_RESOURCE_STATE_GENERIC_READ;
	else if (d3dHeapType == D3D12_HEAP_TYPE_READBACK) d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;

	// ���� ���ҽ� ����
	HRESULT hResult = pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc,
		D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, d3dResourceInitialStates, NULL,
		__uuidof(ID3D12Resource), (void**)&pd3dBuffer);

	// ���� ������ ���� ó��
	if (pData)
		switch (d3dHeapType)
		{
		case D3D12_HEAP_TYPE_DEFAULT:
		{
			if (ppd3dUploadBuffer)
			{
				// ���ε� ���ۿ� ���ҽ��� �����Ѵ�. 
				d3dHeapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
				pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc,
					D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL,
					__uuidof(ID3D12Resource), (void**)ppd3dUploadBuffer);
				// ���ε� ���۸� �����Ͽ� �ʱ�ȭ �����͸� ���ε� ���ۿ� �����Ѵ�. 
				D3D12_RANGE d3dReadRange = { 0, 0 };
				UINT8* pBufferDataBegin{};
				(*ppd3dUploadBuffer)->Map(0, &d3dReadRange, (void**)&pBufferDataBegin);
				memcpy(pBufferDataBegin, pData, nBytes);
				(*ppd3dUploadBuffer)->Unmap(0, NULL);
				// ���ε� ������ ������ ����Ʈ ���ۿ� �����Ѵ�. 
				pd3dCommandList->CopyResource(pd3dBuffer, *ppd3dUploadBuffer);
				D3D12_RESOURCE_BARRIER d3dResourceBarrier;
				::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
				d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				d3dResourceBarrier.Transition.pResource = pd3dBuffer;
				d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
				d3dResourceBarrier.Transition.StateAfter = d3dResourceStates;
				d3dResourceBarrier.Transition.Subresource =
					D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
			}
			break;
		}
		case D3D12_HEAP_TYPE_UPLOAD:
		{
			// ���ε� ���۸� �����Ͽ� �ʱ�ȭ �����͸� ���ε� ���ۿ� �����Ѵ�.
			D3D12_RANGE d3dReadRange = { 0, 0 };
			UINT8* pBufferDataBegin = NULL;
			pd3dBuffer->Map(0, &d3dReadRange, (void**)&pBufferDataBegin);
			memcpy(pBufferDataBegin, pData, nBytes);
			pd3dBuffer->Unmap(0, NULL);
			break;
		}
		case D3D12_HEAP_TYPE_READBACK:
			break;
		}

	return pd3dBuffer;
}

///////////////////////////////////////////////////////////////////////////////////////////////

//�Լ������Ϳ� ȣ��ð��� �������ִ� ����ü�̴�.
struct Event
{
	function<void()> CallBack{};
	high_resolution_clock::time_point WakeUpTime{};

	constexpr bool operator>(const Event& Rhs) const
	{
		return WakeUpTime > Rhs.WakeUpTime;
	}
};

//Event ����ü�� ������ �켱���� ť �ڷᱸ���̴�. WakeUpTime�� �������� �켱������ ����.
priority_queue<Event, vector<Event>, greater<Event>> TimerQueue;

//Ÿ�̸� ť�� �̺�Ʈ�� �߰��Ѵ�.
void AddTimerQueue(function<void()> CallBack, int Duration)
{
	Event Event{ CallBack, high_resolution_clock::now() + milliseconds{ Duration } };
	TimerQueue.push(Event);
}

//Ÿ�̸� �������̴�.
void TimerThread()
{
	while (true)
	{
		this_thread::sleep_for(1ms);

		while (true)
		{
			if (TimerQueue.empty() || TimerQueue.top().WakeUpTime > high_resolution_clock::now()) break;

			Event Event{ TimerQueue.top() };
			TimerQueue.pop();

			//��ϵ� �Լ��� ȣ���Ѵ�.
			Event.CallBack();
		}
	}
}
