﻿/*
	기저 벡터:			축에 대한 단위벡터
	선형 변환:			벡터의 성질을 만족시키는 변환 -> f(x + y) = f(x) + f(y) / f(ax) = af(x)
	위치 변환:			위치를 이동시키는 변환	(비선형적. 왜?)
						? -> ? + 1의 변환 일 때, x + y + 1 != x + y + 2이기 때문.
						x -> ax + by 
	아핀 공간:			점과 벡터로 이루어진 공간
	아핀 변환:			선형 변환 + 위치 변환
	동차좌표(w):			아핀 변환을 쓰기위해 사용 (일반적으로 0은 벡터, 1은 점이라 가정)
	AABB:				축 정렬 바운딩 박스		(객체가 회전해도 그대로)
	OOBB:				객체 정렬 바운딩 박스		(객체가 회전하면 같이 회전)
	Aspect Ratio:		가로 / 세로 (640x480의 종횡비는 1.3333~)
	NDC:				정규화된 투영평면
	BackFace Culling:	보이지 않는 면을 그리지 않는 것 (면들에 대해 계산)
						-> 외적으로 각 면의 노멀벡터를 구하고 카메라의 방향벡터와 내적하면 사잇각을 구할 수 있음
						-> 사잇각이 예각이다? 보이지 않는 면이다.
	Clipping:			뷰 프로스텀에 포함되지 않으면 그리지 않는 것 (점 즉, 픽셀들에 대해 계산)
	Linear Color:		선형적으로 변하는 색
						화면에 보이는 색은 선형적으로 변하지 않고 실제보다 약간 어둡게 보임.
						-> 감마를 사용해 해결 (감마보정, Gamma Correction라고 함)
	표준 RGB:			감마보정이 되어있는 색
	Frame Buffer:		영상정보를 일시적으로 저장하는 기억장치
						-> 렌더 타겟(Render Target)이라고도 함
	Swap Chain:			프레임 버퍼들의 집합
	Presentation:		후면 버퍼의 내용을 전면 버퍼로 옮기는 것
	Refresh:			전면버퍼를 화면에 출력하는 것	(하드웨어가 담당)
	앨리어싱(계단현상):	화면의 픽셀크기가 무한히 작지 않기때문에 선이 계단처럼 보여지는 것
	안티 앨리어싱:		후면 버퍼와 깊이 버퍼의 해상도를 n배(보통 4배) 더 크게 만든 후, 
						서브픽셀들의 색을 계산하고 평균을 구해 원래 픽셀의 색을 결정하는 것 (하드웨어가 담당)
	슈퍼 샘플링:			서브픽셀 전부의 색을 계산		(연산비용 ↑ 디테일 ↑)
	다중 샘플링:			서브픽셀 중 하나의 색만 계산	(연산비용 ↓ 디테일 ↓)
	Command Queue:		커맨드 리스트를 담고있는 큐 (FIFO)
	Command Allocator:	GPU 명령을 저장하기 위한 메모리 할당자 (이곳에서 커맨드 리스트 생성)
	Command List:		GPU에 요청할 명령 리스트
	Shader:				렌더링을 위한 연산을 하는 소프트웨어 명령집합
	Resource:			렌더링을 하기위해 GPU가 사용하는 데이터 (크게 텍스쳐와 버퍼로 구분) -> Committed, Placed, Reserved
	Resource View:		리소스에 대한 정보를 가지고 있는 객체
						-> 서술자(Descriptor)라고도 함
	Resource Barrier:	리소스의 상태(어떻게 사용할지)를 관리하는 객체
	Descriptor Heap:	서술자들을 저장하는 배열
	Default Heap:		GPU가 읽고 쓸 때 사용되는 힙 -> GPU에서 처리속도가 높음, CPU에서 접근불가
	Upload Heap:		CPU가 업로드할 때 사용되는 힙 -> GPU에서 처리속도가 낮음, CPU에서 접근하도록 최적화되있음
	ReadBack Heap:		CPU가 다시 읽어갈 때 사용되는 힙 -> GPU에서 처리속도가 낮음, CPU에서 접근하도록 최적화되있음
	Depth:				정규화된 Z값 (0.0 ~ 1.0)
	Depth Buffer:		시야에 겹치는 픽셀에 대해서 깊이 검사를 통해 어떤 것을 렌더링할지 판단하기위해 사용하는 버퍼
	Stencil Buffer:		일부 영역을 렌더링하지 않기 위해 사용하는 버퍼 (0 ~ 255)
	Viewport:			렌더링을 할 렌더 타겟(후면 버퍼)의 영역
	ScissorRect:		실제 렌더링할 영역
	Semantic:			다음 파이프라인에 넘길 정보, 태그 (전역 객체와 함수 인자만 시맨틱을 가질 수 있음)
	Primitive:			점, 선, 삼각형의 형태		
	선형보간:			Result = (1 - t) * A + t * B
	LOD:				카메라와의 거리에 따라서 객체의 디테일을 다르게하는 기법
	Instancing:			동일한 객체를 렌더링할 때, 한번의 드로우콜로 처리하는 기법
	정점 데이터:			모델에 따라 달라지는 데이터
	인스턴스 데이터:		객체에 따라 달라지는 데이터
	입력 레이아웃:		입력 조립기에 연결되는 입력 버퍼의 구조를 표현 (정점 데이터 or 인스턴스 데이터)
	Root Signature:		리소스들이 그래픽스 파이프라인의 셰이더들에서 어떻게 사용될 것인지 정의하는 배열 (셰이더 시그니처라고도 함)
						시그니처 하나당 64개의 32비트 원소(루트 파라미터)으로 구성
	Root Parameter:		루트 상수(int p), 루트 서술자(int* p), 서술자 테이블(int** p) -> 셰이더 전역변수에 대입
						루트 상수가 가장 빠르지만, 남용시 시그니처 공간이 낭비됨 (크기가 제한적이기에)
	Height Map:			각 픽셀의 높이를 나타내는 이미지
	퐁 모델 조명:		Ambient:	주변 조명(간접)		-> 모든 표면에 균일하게 반사되는 빛을 표현, 앰비언트만 존재하면 모든 색이 균일해짐
						Diffuse:	확산 조명(직접)		-> 난반사되는 빛을 표현(= 객체의 색상), 매끄럽지 않은 표면에서 일어남 (카메라 방향 무관)
						Specular:	스페큘러 조명(직접)	-> 정반사되는 빛을 표현, 매끄러운 표면에서 일어남 (카메라 방향 유관)
						Emissive:	발광 조명 (스스로 빛을 발산)
	램버트 법칙:			반사광의 양은 입사광과 표면 노멀벡터의 사이각이 클수록 작아짐
	Ray Tracing:		빛의 반사를 계산(추적)하는 기법 (래스터라이제이션보다 비용 ↑)
*/

/*
	DirectX:	왼손 좌표계, 행우선 행렬		(좌 -> 우 연산)
	OpenGL:		오른손 좌표계, 열우선 행렬	(우 -> 좌 연산)

	행렬 A가 직교행렬이다. -> 행렬 A의 각 행들이 직교한다. -> A의 역행렬 == A의 전치행렬
	외적은 교환법칙이 성립하지 않는다. -> 스칼라는 같지만 방향이 반대다.
	행 우선행렬은 행마다 메모리에 들어가고, 열 우선행렬은 열마다 메모리에 들어간다.
	-> 실제로 2차원 배열로 표현할 때 행 우선행렬은 가로가 행이고, 열 우선행렬은 가로가 열이기 떄문

	Device의 버전은 달라질 수 있기에 기본적이고 공통적인 프레임워크는 DXGI에 저장
	COM 객체에는 가비지 콜렉터가 존재 -> 명시적으로 메모리를 해제해주면 안됨
	실제로는 비디오 메모리에 존재하는 후면 버퍼에 영상 정보를 넣고 프레젠테이션한 뒤, 리프레쉬
	전면 버퍼는 무조건 1개, 후면 버퍼는 0개 ~ n개가 될 수 있음 (전면 버퍼에는 접근불가)
	프레임 버퍼 개수만 늘린다고 FPS가 향상되진 않음 -> 멀티 스레드를 이용
	GPU도 CPU와 마찬가지로 물리적 제한없이 사용할 수 있도록 가상 메모리를 지원
	GPU에 명령을 요청한다고 곧바로 실행되는 것이 아님 -> 커맨드 큐에서 순차적으로 실행
	CPU에서 리소스를 생성하고 GPU가 사용할 때 동기화 필요 (CPU가 다음 프레임의 리소스를 생성하기 전에 GPU가 이전 리소스 사용을 끝내야하기 때문) -> ID3D12Fence를 이용
	D3D12에는 크게 두가지 파이프라인으로 구분 -> 그래픽스 파이프라인, 컴퓨트 파이프라인
	리소스는 파이프라인에 직접 연결되지 않고 리소스 뷰(서술자)를 통해 연결됨
	리소스를 만들 때는 기본적으로 업로드 힙(CPU 접근 가능)에 CPU 리소스 데이터를 옮기고 디폴트 힙(CPU 접근 불가능)에 복사
	렌더링 파이프라인은 고정 프로그램 단계와 프로그램 가능 단계로 나뉨, 셰이더는 프로그램 가능 단계이고 이 외는 고정 프로그램 단계 
	-> 고정 프로그램은 Direct3D가 하드웨어에서 알아서 처리, 셰이더는 사용자가 소프트웨어에서 직접 정의
	CPU와 GPU간에 서로 메모리에 접근하려는 것은 느림. 따라서 주의할 것
	트라이앵글 스트립 토폴로지는 입력조립기가 와인딩 오더 순서가 맞지 않는다면 순서에 맞게 변경해준다. (인덱스 버퍼 사용)
	인덱스 버퍼의 사용은 정점 버퍼의 개수를 크게 줄여줄 수 있다. -> 인덱스 버퍼는 int, 정점 버퍼는 vector임 즉, 메모리를 줄일 수 있음
	정점 버퍼 뷰, 인덱스 버퍼 뷰는 내부적으로 서술자, 서술자 힙이 필요없음 -> Create*View()가 없고 IASet*()으로 뷰를 입력조립기에 직접연결
	GPU의 코어는 CPU 대비 아주 많음
	루트 시그니처는 여러개 생성할 수 있지만, Set은 하나만 가능 -> 생성 전에 직렬화 시켜주어야 한다.
	루트 파라미터의 개수는 제한적이지만, 셰이더 변수는 무한하다.
	D3D와 HLSL에서 Constant의 의미는 모든 정점, 픽셀의 처리를 완료할 때까지 값이 변하지않는 것을 의미한다.
	HLSL에서 전역변수는 디폴트가 extern, uniform이다. 메모리가 없는 변수기 때문에, 메모리를 차지하려면 static 키워드로 선언해야한다.
	D3D 행우선 행렬을 셰이더에 넘길때는 전치행렬로 넘기는게 좋다.
	-> HLSL 행렬의 기본은 열우선이기 때문. 행우선으로 바꿔주면 되지만 연산이 연속적이지 못하기 때문에 비효율적이다.
	패킹은 상수 레지스터(c)에만 적용된다.
	상수 버퍼는 업로드 힙에 만들어야하고 256Byte의 배수여야한다.
	dx는 왼손좌표계이기 때문에 하이트 맵의 정점들을 가져올때는 뒤집어서 가져와야한다.
	특정 지형의 높이를 구하려면 선형보간을 총 세번해야한다. -> 다른 삼각형의 점은 같은 평면에 있다는걸 보장할 수 없기때문에 따로 구해야한다.
	반사되는 빛의 색상 = 빛의 색상벡터 * 재질의 색상벡터
*/

/*
	★ 렌더링 파이프라인:	입력 조립기 -> 정점 셰이더 -> 헐 셰이더 -> 테셀레이터 -> 도메인 셰이더 -> 기하 셰이더 -> 
						(스트림 출력) -> 래스터라이저 -> 픽셀 셰이더 -> 출력 병합기

	입력 조립기:		정점 데이터들을 프리미티브로 변환 -> 시스템 값 시맨틱 생성
	정점 셰이더:		월드 변환 -> 카메라 변환 -> 투영 변환 -> 스키닝 -> 모핑 -> 조명 처리(정점 조명, 빠름) (정점마다 호출)
	래스터라이저:		원근 투영 나누기 -> 컬링 -> 클리핑 -> 프리미티브를 픽셀로 변환 (이 때, 선형보간 수행)-> 뷰포트 맵핑(스크린 변환을 대체)
	픽셀 셰이더:		조명 처리(픽셀 조명, 느림) -> 픽셀 색상 결정 (픽셀마다 호출)
	출력 병합기:		깊이-스텐실 검사 -> 블렌딩 (투명한 오브젝트가 있을 수 있어서. ex) 물, 유리)

	★ 변환 파이프라인:	월드 변환 -> 카메라(뷰) 변환 -> 투영 변환 -> 스크린 변환

	카메라(뷰) 변환)
	카메라는 연산이 자주 일어나고 방향 혹은 위치만 필요할 수 있음. 따라서, 하나의 행렬이 아닌 네개의 벡터로 선언.
	-> 전부 월드좌표의 벡터임. 왜? 카메라좌표로 표현하는 것은 항상 고정되어있으니 의미가없음.
	월드변환으로 카메라가 회전하면 카메라변환으로 오브젝트가 그 반대방향으로 회전.
	부동소수점 문제때문에 회전시 축이 어긋날 수 있음. -> 외적을 이용해 다시 세 축이 직교하도록 만들어야함.
	투영 변환)
	투영 변환후 원근투영 나누기를 하면 -1 <= x, y <= 1 / 0 <= z <= 1
	투영 변환을 쓰는 이유는 NDC를 구하기위해, 화면 종횡비를 맞춰기 위해서임
	FOV를 90도로 안쓰고 60이나 45를 쓰는 이유는 FOV가 클수록 보이는 정점들이 많아 렌더링 오버헤드가 발생하기 떄문
	또한, 절두체의 원평면까지의 거리가 클수록 렌더링 오버헤드 발생
*/

/*
	TIP)

	DirectMath를 효율적으로 사용하려면:
	C/C++ -> 코드 생성 -> 고급 명령집합 사용 -> 고급 벡터 확장(AVX)
	C/C++ -> 고급 -> 호출규칙 -> __vectorcall
	※	SIMD 연산을 하려면 변수가 16Byte로 정렬되는 것이 보장되어야한다.
		클래스 or 구조체의 멤버변수는 이 정렬을 보장받지 못하므로, XMLoad와 XMStore로 16Byte 정렬을 보장받는 로컬변수를 만들어 연산해야한다.

	디바이스 초기화 시 예외처리는 HRESULT형 변수에 dx함수의 반환값을 저장하고 전용 매크로로 성공, 실패여부를 체크
	-> 실패했다면 HRESULT형 변수에 담긴 값을 통해 원인을 알 수 있음

	SetName():	COM 객체에 이름을 부여 (주로 디버깅 시 사용)
*/