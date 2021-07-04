#ifndef __PROTOCOL__
#define __PROTOCOL__


/*
CS => Client -> Server

SC => Server -> Client
*/
#define dfERROR_RANGE 50

#define dfSPEED_PLAYER_X						6
#define dfSPEED_PLAYER_Y						4


#define dfACTION_MOVE_LL						0 
#define dfACTION_MOVE_LU						1
#define dfACTION_MOVE_UU						2
#define dfACTION_MOVE_RU						3
#define dfACTION_MOVE_RR						4 
#define dfACTION_MOVE_RD						5
#define dfACTION_MOVE_DD						6
#define dfACTION_MOVE_LD						7

#define dfACTION_ATTACK1						8
#define dfACTION_ATTACK2						9
#define dfACTION_ATTACK3						10

#define dfACTION_STAND							11

/*
8방향 패킷
*/
#define dfPACKET_MOVE_DIR_LL					0
#define dfPACKET_MOVE_DIR_LU					1
#define dfPACKET_MOVE_DIR_UU					2
#define dfPACKET_MOVE_DIR_RU					3
#define dfPACKET_MOVE_DIR_RR					4
#define dfPACKET_MOVE_DIR_RD					5
#define dfPACKET_MOVE_DIR_DD					6
#define dfPACKET_MOVE_DIR_LD					7


#define dfPACKET_CODE 0x89



/*
클라이언트 자신의 캐릭터를 생성

서버 접속 시 최초로 받는 패킷
할당받은 ID와 자신의 최초 위치, HP를 받음.

4 - ID
1 - Direction
2 - X
2 - Y
1 - HP

*/
#define dfPACKET_SC_CREATE_MY_CHARACTER			0



/*
다른 클라이언트 캐릭터 생성 패킷

4 - ID
1 - Direction
2 - X
2 - Y
1 - HP
*/
#define	dfPACKET_SC_OTHER_CREATE_CHARACTER		1



/*
캐릭터 삭제 패킷

캐릭터의 접속 해제 또는 캐릭터가 죽었을 때 전송
4 - ID
*/
#define	dfPACKET_SC_DELETE_CHARACTER			2




/*
캐릭터 이동 시작 패킷 

모든 유저의 캐릭터 이동시작 패킷.
메시지를 보낸 캐릭터를 찾아 이동 시작.

방향을 받아 해당 키가 계속 눌린 것으로 판단. 계속 이동 시켜줌
이동 중일 때는 보내지 않고 방향이 바뀌거나 할 때 전송

	1	-	Direction	( 방향 디파인 값 8방향 사용 )
	2	-	X
	2	-	Y
*/
#define dfPACKET_CS_MOVE_START						10



/*
캐릭터 이동 시작 패킷

다른 유저의 캐릭터 이동 시 클라가 수신하는 패킷.
메시지를 보낸 캐릭터를 찾아 이동 시작.

방향을 받아 해당 키가 계속 눌린 것으로 판단. 계속 이동 시켜줌
이동 중일 때는 보내지 않고 방향이 바뀌거나 할 때 전송

4 - ID(KEY)
1 - Direction (8방향 중 하나)
2 - X
2 - X
*/
#define dfPACKET_SC_MOVE_START						11



/*
캐릭터 이동 중지 패킷

클라에서 캐릭터의 이동을 멈추면 보내는 패킷

1 - Direction (8방향에서 좌우만 사용)
2 - X
2 - Y
*/
#define dfPACKET_CS_MOVE_STOP						12



/*
캐릭터 이동중지 패킷

ID(KEY)에 해당하는 캐릭터가 이동을 멈춤

4 - ID
1 - Direction
2 - X
2 - Y

*/
#define dfPACKET_SC_MOVE_STOP						13



/*
캐릭터 공격 패킷

공격 동작 시 한 번만 패킷을 전송.
서버는 충돌 체크 후 충돌 했다면 데미지 패킷도 전송

좌표가 들어있는 이유 : 공격을 시작한 좌표가 서버와 다르다면 싱크 메시지 전송을 위함
1 - Direction (8방향에서 좌우만 사용)
2 - X
2 - Y
*/
#define dfPACKET_CS_ATTACK1						20


/*
캐릭터 공격 패킷

클라에서 ID에 해당하는 캐릭터를 찾아 공격 1번 동작으로 변경
방향이 다르면 방향도 변경

4 - ID
1 - Direction
2 - X
2 - Y
*/
#define dfPACKET_SC_ATTACK1						21



/*
캐릭터 공격 패킷

공격 동작 시 한 번만 패킷을 전송.
서버는 충돌 체크 후 충돌 했다면 데미지 패킷도 전송

1 - Direction (8방향에서 좌우만 사용)
2 - X
2 - Y
*/
#define dfPACKET_CS_ATTACK2						22


/*
캐릭터 공격 패킷

클라에서 ID에 해당하는 캐릭터를 찾아 공격 2번 동작으로 변경
방향이 다르면 방향도 변경

4 - ID
1 - Direction (8방향에서 좌우만 사용)
2 - X
2 - Y
*/
#define dfPACKET_SC_ATTACK2						23




/*
캐릭터 공격 패킷

공격 동작 시 한 번만 패킷을 전송.
서버는 충돌 체크 후 충돌 했다면 데미지 패킷도 전송

1 - Direction (8방향에서 좌우만 사용)
2 - X
2 - Y
*/
#define dfPACKET_CS_ATTACK3						24


/*
캐릭터 공격 패킷

클라에서 ID에 해당하는 캐릭터를 찾아 공격 2번 동작으로 변경
방향이 다르면 방향도 변경

1 - Direction (8방향에서 좌우만 사용)
2 - X
2 - Y
*/
#define dfPACKET_SC_ATTACK3						25


/*
데미지 패킷

맞은 캐릭터의 정보를 전송

4 - ATTACKID(공격자 ID)
4 - DAMAGEID(피해자 ID)
1 - DAMAGE_HP (피해자 HP)
*/
#define dfPACKET_SC_DAMAGE						30



/*
위치 좌표 동기화를 위한 패킷

해당 캐릭터를 찾아 좌표 동기화
4 - ID
2 - X
2 - Y
*/
#define dfPACKET_SC_SYNC						251


#define	dfPACKET_CS_ECHO						252
//---------------------------------------------------------------
// Echo 용 패킷					Client -> Server
//
//	4	-	Time
//
//---------------------------------------------------------------

#define	dfPACKET_SC_ECHO						253
//---------------------------------------------------------------
// Echo 응답 패킷				Server -> Client
//
//	4	-	Time
//
//---------------------------------------------------------------


#endif