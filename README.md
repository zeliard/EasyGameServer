EasyGameServer
==============

NHN NEXT의 첫번째 게임 프로젝트를 위한 교육용 게임 서버 기반 코드 입니다.

Windows의 Overlapped I/O를 이용한 single-thread 기반의 asynchronous non-blocking 방식의 TCP 서버입니다.


1. 당신이 EasyGameServer에서 참고하거나 배울 수 있는 것

A. Win32 API의 Overlapped I/O를 이용한 비동기 네트워크 입출력 및 다수의 클라이언트 세션 처리

B. TCP 스트림을 다루는 방법 및 TCP 스트림을 통한 패킷 시리얼라이징
   Circular Byte Buffer를 구성하고 다루는 법
   
C. 게임 클라이언트에서의 네트워크 처리 방법
   Asyncselect를 이용한 asynchronous non-blocking 방식의 클라이언트 구조

D. SQLite3을 이용한 Database 처리 방법
   C++상에서의 데이터베이스 제어
   Half-sync Half-async 패턴을 이용한 데이터베이스 처리 전용 스레드의 사용법
   Wait-free Single Producer Single Consumer Queue를 구현하는 방법

E. Waitable Timer를 이용한 게임 타이머 구성 방법

F. 서버에서의 예외 상황 발생시 minidump 생성 방법

G. Object Pooling을 통한 메모리 할당/해제 성능 향상 방법

H. 그 이외 C++ 11의 몇몇 특징 사용 방법


2. 당신이 여기에서 배울 수 없는 것

A. multi-thread 기반의 IOCP을 이용한 고성능 서버 만들기 방법
B. 스마트 포인터를 통한 객체의 생명 관리
