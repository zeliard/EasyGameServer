EasyGameServer (for cocos2dx) 
==============

This game server supports real-time/stateful games using TCP

NHN NEXT의 첫번째 게임 프로젝트를 위한 교육용 게임 서버 기반 코드 입니다.

Windows의 Overlapped I/O를 이용한 single I/O thread 기반의 asynchronous non-blocking 방식의 TCP 서버입니다.
(동시 접속 10000세션 무한 echo-back 테스트에서도 별 무리 없이 가동되었습니다.)

EasyGameServer는 기본적으로 3개의 thread로 구성 됩니다. 

1. 프로그램의 초기화 및 진입을 담당하고 ACCEPT를 처리하는 main thread
2. 클라이언트 세션 처리, 클라이언트 타이머 및 I/O를 담당하는 client handling thread 
3. 데이터베이스 처리를 담당하는 database handling thread

## 당신이 EasyGameServer에서 참고하거나 배울 수 있는 것

1. Win32 API의 Overlapped I/O를 이용한 비동기 네트워크 입출력 및 다수의 클라이언트 세션 처리

2. TCP 스트림을 다루는 방법 및 TCP 스트림을 통한 패킷 시리얼라이징,
   Circular Byte Buffer를 구성하고 다루는 법,
   NAGLE (TCP_NODELAY) 컨트롤 
   
3. cocos2d-x 게임 클라이언트에서의 네트워킹 방법 (send/recv 및 TCP 스트림 핸들링)

4. SQLite3을 이용한 Database 처리 방법,
   C++상에서의 데이터베이스 제어,
   Half-sync Half-async 패턴을 이용한 데이터베이스 처리 전용 스레드의 사용법
   
5. Windows APC(Async Procedure Call) 큐의 개념 이해와 더불어 Windows의 각종 동기화 객체들 사용법
   - Waitable Timer를 이용한 게임 타이머 구성 방법
   - Producer Consumer Queue를 condition variable을 이용하여 구현하는 방법
   - Event 및 SRWLock 등의 사용법

6. 서버에서의 예외 상황 발생시 minidump 생성 방법

7. Object Pooling을 통한 메모리 할당/해제 성능 향상 방법

8. Thread local storage의 개념

9. std::bind를 응용한 Task 스케줄링 방법 

10. 참조 카운팅 기법을 활용한 객체의 생명 주기 관리 방법

11. Dummy 클라이언트(Boost.Asio 사용)를 이용한 시나리오 기반 서버 부하 테스트 방법

## 기대했을 수 있지만, 여기에서 배울 수 없는 것

1. multi-thread 기반의 IOCP를 이용한 서버 만들기 방법
2. 고성능 서버 제작을 위한 각종 튜닝 기법들
3. 암호화, 해킹방어 등과 같은 각종 보안 기법들
4. 게임 콘텐츠에 관련된 각종 구현 방법들

