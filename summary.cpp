﻿/*
	기저 벡터:		축에 대한 단위벡터
	선형 변환:		벡터의 성질을 만족시키는 변환 -> f(x + y) = f(x) + f(y) / f(ax) = af(x)
	위치 변환:		위치를 이동시키는 변환	(비선형적. 왜?)
					? -> ? + 1의 변환 일 때, x + y + 1 != x + y + 2이기 때문.
					x -> ax + by 
	아핀 공간:		점과 벡터로 이루어진 공간
	아핀 변환:		선형 변환 + 위치 변환
	동차좌표(w):		아핀 변환을 쓰기위해 사용 (일반적으로 0은 벡터, 1은 점이라 가정)

	AABB:			축 정렬 바운딩 박스		(객체가 회전해도 그대로)
	OOBB:			객체 정렬 바운딩 박스		(객체가 회전하면 같이 회전)

	Aspect Ratio:	가로 / 세로 (640x480의 종횡비는 1.3333~)
	NDC:			정규화된 투영평면
	Clipping:		뷰 프로스텀에 포함되지 않으면 렌더링하지 않는 것

	Linear Color:	선형적으로 변하는 색
					화면에 보이는 색은 선형적으로 변하지 않고 실제보다 약간 어둡게 보임.
					-> 감마를 사용해 해결 (감마보정, Gamma Correction라고 함)
	표준 RGB:		감마보정이 되어있는 색

	Frame Buffer:	영상정보를 일시적으로 저장하는 기억장치
	Swap Chain:		프레임 버퍼들의 집합
	Presentation:	후면 버퍼의 내용을 전면 버퍼로 옮기는 것
	Refresh:		전면버퍼를 화면에 출력하는 것	(하드웨어가 담당)
	앨리어싱(계단현상):	화면의 픽셀크기가 무한히 작지 않기때문에 선이 계단처럼 보여지는 것
	안티 앨리어싱:		원래 그리려는 크기보다 n배(보통 4배) 더 크게 후면버퍼에 그린 후, 서브픽셀들의 색을 계산하고 평균을 구해
						원래 픽셀의 색을 결정한 뒤, 원래 크기로 축소시켜서 프레젠트하는 것 (하드웨어가 담당)
	슈퍼 샘플링:		서브픽셀 전부의 색을 계산		(연산비용 ↑ 디테일 ↑)
	다중 샘플링:		서브픽셀 중 하나의 색만 계산	(연산비용 ↓ 디테일 ↓)
*/

/*
	DirectX:	왼손 좌표계, 행우선 행렬		(좌 -> 우 연산)
	OpenGL:		오른손 좌표계, 열우선 행렬	(우 -> 좌 연산)

	행렬 A가 직교행렬이다. -> 행렬 A의 각 행들이 직교한다. -> A의 역행렬 == A의 전치행렬

	Device의 버전은 달라질 수 있기에 기본적이고 공통적인 프레임워크는 DXGI에 저장
	COM 객체에는 가비지 콜렉터가 존재 -> 명시적으로 메모리를 해제해주면 안됨
	실제로는 비디오 메모리에 존재하는 후면 버퍼에 영상 정보를 넣고 프레젠테이션한 뒤, 리프레쉬
	전면 버퍼는 무조건 1개, 후면 버퍼는 0개 ~ n개가 될 수 있음
	프레임 버퍼 개수만 늘린다고 FPS가 향상되진 않음 -> 멀티 스레드를 이용
*/

/*
	변환 파이프라인:	월드 변환 -> 카메라(뷰) 변환 -> 투영 변환 -> 스크린 변환

	카메라(뷰) 변환:
	카메라는 연산이 자주 일어나고 방향 혹은 위치만 필요할 수 있음. 따라서, 하나의 행렬이 아닌 네개의 벡터로 선언.
	-> 전부 월드좌표의 벡터임. 왜? 카메라좌표로 표현하는 것은 항상 고정되어있으니 의미가없음.
	월드변환으로 카메라가 회전하면 카메라변환으로 오브젝트가 그 반대방향으로 회전.
	부동소수점 문제때문에 회전시 축이 어긋날 수 있음. -> 외적을 이용해 다시 세 축이 직교하도록 만들어야함.

	투영 변환:
	투영 변환후 원근투영 나누기를 하면 -1 <= x, y <= 1 / 0 <= z <= 1
	투영 변환을 쓰는 이유는 NDC를 구하기위해, 화면 종횡비를 맞춰기 위해서임
	FOV를 90도로 안쓰고 60이나 45를 쓰는 이유는 FOV가 클수록 보이는 정점들이 많아 렌더링 오버헤드가 발생하기 떄문
	또한, 절두체의 원평면까지의 거리가 클수록 렌더링 오버헤드 발생
*/

/*
	TIP)

	디바이스 초기화 시 예외처리는 HRESULT형 변수에 dx함수의 반환값을 저장하고 전용 매크로로 성공, 실패여부를 체크
	-> 실패했다면 HRESULT형 변수에 담긴 값을 통해 원인을 알 수 있음
	SetName():	COM 객체에 이름을 부여 (주로 디버깅 시 사용)
*/