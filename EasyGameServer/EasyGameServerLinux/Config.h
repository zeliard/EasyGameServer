#pragma once


/// DB 연결 스트링 
#define DB_CONN_STR "./database/user_example.db3"
#define LISTEN_PORT	9001

/// 타이머 실행 간격 (ms)
#define POLL_INTERVAL	100

/// 플레이어의 HeartBeat 주기
#define PLAYER_HEART_BEAT	1000

/// 1분마다 플레이어 상태 저장
#define PLAYER_DB_UPDATE_INTERVAL	(60*1000)

/// 세션별 send/recv 버퍼 크기
#define BUFSIZE	(1024*10)

/// 최대 동접 제한
#define MAX_CONNECTION	1024