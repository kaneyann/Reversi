//==========================================================
// �l��vs�l�Ԃ̃��o�[�V�Q�[��
//==========================================================

#include <stdio.h>

//--------------------------------------------------------------------
//  �}�N����`
//--------------------------------------------------------------------

//--- �I�u�W�F�N�g�`���}�N��
#define SIZE 10    // �Ղ̃T�C�Y(8�~8�Ə㉺���E���[�̔ԕ��e1��)
#define FIRST 1    // 1�s�ځA1��ڂ�\��
#define LAST 8     // 8�s�ځA8��ڂ�\��
#define TRUE  1    // �^��1
#define FALSE 0    // �U��0

//--- �֐��`���}�N��
#define partner(player) ((3) - (player))    // 1�̑����2�A2�̑����1

//--------------------------------------------------------------------
//  ���錾
//--------------------------------------------------------------------
    
//--------------------------------------------------------------------
//  ����`
//--------------------------------------------------------------------

enum { EMP = 0, BLK = 1, WHT = 2, OUT = -1 };    // �}�X�ڂ̏��
enum { DEC_RAW = -1, INC_RAW = 1 };    // �s�̑���
enum { DEC_COL = -1, INC_COL = 1 };    // ��̑���

//--------------------------------------------------------------------
//  �֐����^�錾
//--------------------------------------------------------------------

void init_board(int board[][SIZE]);     // �Ֆʂ̏�����
void print_board(int board[][SIZE]);    // �Ֆʂ̕\��
int search_legal_move(int board[][SIZE], int player);                              // ���@�I�Ȏ肪���邩���ׂ�(���ׂẴ}�X�𑖍�)
void get_input_human(int board[][SIZE], int player, int *p, int *q);               // �ł���̓���(�l��)
int is_legal_move(int board[][SIZE], int player, int p, int q);                    // �ł���̍��@�����m���߂�
int count_turn_over(int board[][SIZE], int player, int p, int q, int d, int e);    // ���񂾐΂̐���Ԃ�(����������1�����̂�)
void set_and_turn_over(int board[][SIZE], int player, int p, int q);               // �΂�u�� �� ���񂾐΂𗠕Ԃ�
void count_stone(int board[][SIZE], int *c1, int *c2);                             // ���v���C���[�̐΂̌��𐔂���
void game(void);           // �I�Z���Q�[����1��s��

//====================================================================
//  �{�̏���
//====================================================================

//--------------------------------------------------------------------
//  main�֐�
//--------------------------------------------------------------------

int main(void)
{
    puts("1�l���o�[�V�X�^�[�g�I");
    puts("Player 1 : ����   Player 2 : ����");
    puts("���ł���̓��͗� : b2\n");
    game();

    return 0;
}

//--------------------------------------------------------------------
//  �Ֆʂ̏�����
//--------------------------------------------------------------------

void init_board(int board[][SIZE])
{
    int i, j;

    //--- ���ׂẴ}�X��ՊO�l�ŏ���������
    for ( i = 0; i < SIZE; i++ ) {
        for ( j = 0; j < SIZE; j++ ) {
            board[i][j] = OUT;
        }
    }

    //--- ���ۂɎg�p����}�X����ɂ���
    for ( i = FIRST; i <= LAST; i++ ) {
        for ( j = FIRST; j <= LAST; j++ ) {
            board[i][j] = EMP;
        }
    }

    //--- �Ֆʂ̒��������Ɣ��ɂ���
    board[4][5] = board[5][4] = BLK;
    board[4][4] = board[5][5] = WHT;
}

//--------------------------------------------------------------------
//  �Ֆʂ̕\��
//--------------------------------------------------------------------

void print_board(int board[][SIZE])
{
    int i, j;

    puts("");
    puts("   ���������������� ");    // ��L���̕\��
    puts("  ------------------");

    //--- �s
    for ( i = FIRST; i <= LAST; i++ ) {
        printf("%d| ", i);           // �s�ԍ��̕\��
        //--- ��
        for ( j = FIRST; j <= LAST; j++ ) {
            switch( board[i][j] ) {
            case 0 :
                printf("�E");    // ��̕\��
                break;
            case BLK :
                printf("��");    // ���΂̕\��
                break;
            case WHT :
                printf("��");    // ���΂̕\��
                break;
            }
        }
        puts(" |");
    }

    puts("  ------------------");
}

//--------------------------------------------------------------------
//  ���@�I�Ȏ肪���邩���ׂ�(���ׂẴ}�X�𑖍�)
//--------------------------------------------------------------------

int search_legal_move(int board[][SIZE], int player)
{
    int i, j;

    //--- ���ׂẴ}�X�𑖍�
    for ( i = FIRST; i <= LAST; i++ ) {
        for ( j = FIRST; j <= LAST; j++ ) {
            if ( is_legal_move(board, player, i, j) == TRUE ) {    // ���@�I�Ȏ肪���݂���Ȃ��
                return TRUE;
            }
        }
    }
    return FALSE;
}

//--------------------------------------------------------------------
//  �ł���̓���(�l��)
//--------------------------------------------------------------------

void get_input_human(int board[][SIZE], int player, int *p, int *q)
{
    char str[10];    // ���͂��������񂪓���o�b�t�@

    printf("Player %d �̃^�[���ł�.\n", player);

    //--- �ł���̓���
    while ( 1 ) {
        printf("> ");                      // ���͂𑣂��v�����v�g
        fgets(str, sizeof(str), stdin);    // �ő�Ŕz��str�̑傫�����W�����͂��當�����1�s�ǂݍ���
        *q = str[0] - 'a' + 1;             // �Ⴆ��'a'��ǂݍ��ނƁA*q�ɓ���l��1�ƂȂ�悤�ɂ��Ă���
        *p = str[1] - '1' + 1;             // �Ⴆ��'1'��ǂݍ��ނƁA*p�ɓ���l��1�ƂȂ�悤�ɂ��Ă���
        if ( is_legal_move(board, player, *p, *q) == TRUE ) {    // �ł��肪���@�I�Ȃ��
            return;
        }
        puts("�����ɂ͒u���܂���.");
    }
}

//--------------------------------------------------------------------
//  �ł���̍��@�����m���߂�
//--------------------------------------------------------------------

int is_legal_move(int board[][SIZE], int player, int p, int q)
{
    if ( p < FIRST || p > LAST || q < FIRST || q > LAST ) { return FALSE; }    // �ՖʊO
    if ( board[p][q] != EMP ) { return FALSE; }                    // ���ɐ΂��u����Ă���
    if ( count_turn_over(board, player, p, q, DEC_RAW,     0   ) > 0 ) { return TRUE; }    // ��
    if ( count_turn_over(board, player, p, q, INC_RAW,     0   ) > 0 ) { return TRUE; }    // ��
    if ( count_turn_over(board, player, p, q,    0,     DEC_COL) > 0 ) { return TRUE; }    // ��
    if ( count_turn_over(board, player, p, q,    0,     INC_COL) > 0 ) { return TRUE; }    // �E
    if ( count_turn_over(board, player, p, q, DEC_RAW,  DEC_COL) > 0 ) { return TRUE; }    // ����
    if ( count_turn_over(board, player, p, q, DEC_RAW,  INC_COL) > 0 ) { return TRUE; }    // �E��
    if ( count_turn_over(board, player, p, q, INC_RAW,  DEC_COL) > 0 ) { return TRUE; }    // ����
    if ( count_turn_over(board, player, p, q, INC_RAW,  INC_COL) > 0 ) { return TRUE; }    // �E��
    return FALSE;    // ���Ԃ�΂�1���Ȃ�����(��@)
}

//--------------------------------------------------------------------
//  ���񂾐΂̐���Ԃ�(����������1�����̂�)
//--------------------------------------------------------------------

int count_turn_over(int board[][SIZE], int player, int p, int q, int d, int e)
{
    int i;

    //--- �΂�u�����}�X����w������ɑ���(�����}�X������̐΂ł����)
    for ( i = 1; board[p+(i*d)][q+(i*e)] == partner(player); i++ ) { ; }

    //--- ���񂾐΂̌���Ԃ�
    if ( board[p+(i*d)][q+(i*e)] == player ) {    // �����I���n�_�������̐΂Ȃ��
        return i-1;    // �Ԃɂ���������̐΂̌���Ԃ�
    } else {
        return 0;      // �����I���n�_�������̐΂łȂ��Ȃ��0��Ԃ�
    }
}

//--------------------------------------------------------------------
//  �΂�u�� �� ���񂾐΂𗠕Ԃ�
//--------------------------------------------------------------------

void set_and_turn_over(int board[][SIZE], int player, int p, int q)
{
    int count;    // 1�����������ɂ����鋲�񂾐΂̌�
    int d, e;     // ����������������߂�ϐ�
    int i;

    //--- (0,0)�����ȊO��8�����𑖍����ċ��񂾐΂𗠕Ԃ�
    for ( d = DEC_RAW; d <= INC_RAW; d++ ) {         // �㉺����
        for ( e = DEC_COL; e <= INC_COL; e++ ) {     // ���E����
            if ( d == 0 && e == 0 ) { continue; }    // �ǂ̕����ɂ��������Ȃ��̂Ŗ���
            count = count_turn_over(board, player, p, q, d, e);    // d,e�����̋��񂾐΂̌�
            for ( i = 1; i <= count; i++ ) {
                board[p+(i*d)][q+(i*e)] = player;    // d,e�����̋��񂾐�(count��)�𗠕Ԃ�
            }
        }
    }
    board[p][q] = player;    // �ł���̈ʒu�ɐ΂�u��
}

//--------------------------------------------------------------------
//  ���v���C���[�̐΂̌��𐔂���
//--------------------------------------------------------------------

void count_stone(int board[][SIZE], int *c1, int *c2)
{
    int i, j;

    //--- �΂̌��̏�����
    *c1 = *c2 = 0;

    //--- ���v���C���[�̐΂̌��̌v�Z
    for ( i = FIRST; i <= LAST; i++ ) {
        for ( j = FIRST; j <= LAST; j++ ) {
            switch ( board[i][j] ) {
            case BLK :      // ���΂Ȃ��
                (*c1)++;
                break;
            case WHT :      // ���΂Ȃ�� 
                (*c2)++;
                break;
            }
        }
    }
}

//--------------------------------------------------------------------
//  ���o�[�V�Q�[����1��s��
//--------------------------------------------------------------------

void game(void)
{
    int board[SIZE][SIZE];    // �Ֆ�
    int player;       // �v���C���[
    int p, q;         // �ł���̈ʒu
    int c1, c2;       // �Տ�̐΂̌�

    //--- ��������
    init_board(board);    // �Ֆʂ̏�����
    player = BLK;         // ���͍���

    //--- �Q�[���i�s����
    while ( 1 ) {
        print_board(board);    // �Ֆʂ̕\��
        if ( search_legal_move(board, player) == FALSE ) {        // ���@�I�Ȏ肪�Ȃ��Ȃ��
            printf("Player %d �ɂ͐΂�u����ꏊ������܂���. �p�X���܂�.\n", player);
            player = partner(player);                             // �v���C���[���
            if ( search_legal_move(board, player) == FALSE ) {    // ��サ�Ă����@�I�Ȏ肪�Ȃ��Ȃ��
                printf("Player %d �ɂ͐΂�u����ꏊ������܂���. �p�X���܂�.\n\n", player);
                break;                                            // ���v���C���[�Ƃ��p�X�������߃Q�[���I��
            }
        }
        get_input_human(board, player, &p, &q);    // �ł���̓���
        set_and_turn_over(board, player, p, q);    // �΂�u�� �� ���񂾐΂𗠕Ԃ�
        player = partner(player);           // �v���C���[���
    }
    
    //--- ���v���C���[�̐΂̌����v�Z
    count_stone(board, &c1, &c2);
    
    //--- ���ʕ\��
    puts("�Q�[���I��\n");
    puts("<���΂̌�>");
    printf("Player 1 : %d��\nPlayer 2 : %d��\n", c1, c2);    // ���v���C���[�̎��΂̌���\��
    if ( c1 - c2 > 0 ) {                 // ���΂̂ق��������Ȃ��
        puts("Player 1 �̏����ł�.");
    } else if ( c1 - c2 < 0 ) {          // ���΂̂ق������Ȃ��Ȃ��
        puts("Player 2 �̏����ł�.");
    } else {
        puts("���������ł�.");
    }
}
