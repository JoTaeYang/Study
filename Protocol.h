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
8���� ��Ŷ
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
Ŭ���̾�Ʈ �ڽ��� ĳ���͸� ����

���� ���� �� ���ʷ� �޴� ��Ŷ
�Ҵ���� ID�� �ڽ��� ���� ��ġ, HP�� ����.

4 - ID
1 - Direction
2 - X
2 - Y
1 - HP

*/
#define dfPACKET_SC_CREATE_MY_CHARACTER			0



/*
�ٸ� Ŭ���̾�Ʈ ĳ���� ���� ��Ŷ

4 - ID
1 - Direction
2 - X
2 - Y
1 - HP
*/
#define	dfPACKET_SC_OTHER_CREATE_CHARACTER		1



/*
ĳ���� ���� ��Ŷ

ĳ������ ���� ���� �Ǵ� ĳ���Ͱ� �׾��� �� ����
4 - ID
*/
#define	dfPACKET_SC_DELETE_CHARACTER			2




/*
ĳ���� �̵� ���� ��Ŷ 

��� ������ ĳ���� �̵����� ��Ŷ.
�޽����� ���� ĳ���͸� ã�� �̵� ����.

������ �޾� �ش� Ű�� ��� ���� ������ �Ǵ�. ��� �̵� ������
�̵� ���� ���� ������ �ʰ� ������ �ٲ�ų� �� �� ����

	1	-	Direction	( ���� ������ �� 8���� ��� )
	2	-	X
	2	-	Y
*/
#define dfPACKET_CS_MOVE_START						10



/*
ĳ���� �̵� ���� ��Ŷ

�ٸ� ������ ĳ���� �̵� �� Ŭ�� �����ϴ� ��Ŷ.
�޽����� ���� ĳ���͸� ã�� �̵� ����.

������ �޾� �ش� Ű�� ��� ���� ������ �Ǵ�. ��� �̵� ������
�̵� ���� ���� ������ �ʰ� ������ �ٲ�ų� �� �� ����

4 - ID(KEY)
1 - Direction (8���� �� �ϳ�)
2 - X
2 - X
*/
#define dfPACKET_SC_MOVE_START						11



/*
ĳ���� �̵� ���� ��Ŷ

Ŭ�󿡼� ĳ������ �̵��� ���߸� ������ ��Ŷ

1 - Direction (8���⿡�� �¿츸 ���)
2 - X
2 - Y
*/
#define dfPACKET_CS_MOVE_STOP						12



/*
ĳ���� �̵����� ��Ŷ

ID(KEY)�� �ش��ϴ� ĳ���Ͱ� �̵��� ����

4 - ID
1 - Direction
2 - X
2 - Y

*/
#define dfPACKET_SC_MOVE_STOP						13



/*
ĳ���� ���� ��Ŷ

���� ���� �� �� ���� ��Ŷ�� ����.
������ �浹 üũ �� �浹 �ߴٸ� ������ ��Ŷ�� ����

��ǥ�� ����ִ� ���� : ������ ������ ��ǥ�� ������ �ٸ��ٸ� ��ũ �޽��� ������ ����
1 - Direction (8���⿡�� �¿츸 ���)
2 - X
2 - Y
*/
#define dfPACKET_CS_ATTACK1						20


/*
ĳ���� ���� ��Ŷ

Ŭ�󿡼� ID�� �ش��ϴ� ĳ���͸� ã�� ���� 1�� �������� ����
������ �ٸ��� ���⵵ ����

4 - ID
1 - Direction
2 - X
2 - Y
*/
#define dfPACKET_SC_ATTACK1						21



/*
ĳ���� ���� ��Ŷ

���� ���� �� �� ���� ��Ŷ�� ����.
������ �浹 üũ �� �浹 �ߴٸ� ������ ��Ŷ�� ����

1 - Direction (8���⿡�� �¿츸 ���)
2 - X
2 - Y
*/
#define dfPACKET_CS_ATTACK2						22


/*
ĳ���� ���� ��Ŷ

Ŭ�󿡼� ID�� �ش��ϴ� ĳ���͸� ã�� ���� 2�� �������� ����
������ �ٸ��� ���⵵ ����

4 - ID
1 - Direction (8���⿡�� �¿츸 ���)
2 - X
2 - Y
*/
#define dfPACKET_SC_ATTACK2						23




/*
ĳ���� ���� ��Ŷ

���� ���� �� �� ���� ��Ŷ�� ����.
������ �浹 üũ �� �浹 �ߴٸ� ������ ��Ŷ�� ����

1 - Direction (8���⿡�� �¿츸 ���)
2 - X
2 - Y
*/
#define dfPACKET_CS_ATTACK3						24


/*
ĳ���� ���� ��Ŷ

Ŭ�󿡼� ID�� �ش��ϴ� ĳ���͸� ã�� ���� 2�� �������� ����
������ �ٸ��� ���⵵ ����

1 - Direction (8���⿡�� �¿츸 ���)
2 - X
2 - Y
*/
#define dfPACKET_SC_ATTACK3						25


/*
������ ��Ŷ

���� ĳ������ ������ ����

4 - ATTACKID(������ ID)
4 - DAMAGEID(������ ID)
1 - DAMAGE_HP (������ HP)
*/
#define dfPACKET_SC_DAMAGE						30



/*
��ġ ��ǥ ����ȭ�� ���� ��Ŷ

�ش� ĳ���͸� ã�� ��ǥ ����ȭ
4 - ID
2 - X
2 - Y
*/
#define dfPACKET_SC_SYNC						251


#define	dfPACKET_CS_ECHO						252
//---------------------------------------------------------------
// Echo �� ��Ŷ					Client -> Server
//
//	4	-	Time
//
//---------------------------------------------------------------

#define	dfPACKET_SC_ECHO						253
//---------------------------------------------------------------
// Echo ���� ��Ŷ				Server -> Client
//
//	4	-	Time
//
//---------------------------------------------------------------


#endif