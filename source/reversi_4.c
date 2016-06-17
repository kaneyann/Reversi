//==========================================================
// �l��vsCOM�̃��o�[�V�Q�[��
// AI : �~�j�}�b�N�X�@
// �Ֆʕ]���̕��@ : �e�}�X�ւ̏d�ݕt��
//==========================================================

#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <Windows.h>

//--------------------------------------------------------------------
//  �}�N����`
//--------------------------------------------------------------------

//--- �I�u�W�F�N�g�`���}�N��
#define SIZE 10    // �Ղ̃T�C�Y(8�~8�Ə㉺���E���[�̔ԕ��e1��)
#define FIRST 1    // 1�s�ځA1��ڂ�\��
#define LAST 8     // 8�s�ځA8��ڂ�\��
#define SEARCH_LEVEL 6    // ��ǂ݂��郌�x��

//--------------------------------------------------------------------
//  ���錾
//--------------------------------------------------------------------

int copy[SEARCH_LEVEL][SIZE][SIZE];            // �ՖʋL���p�̔z��
int value_of_place[LAST][LAST] = {             // �Ֆʂ̊e�ꏊ�̉��l
    { 100, -20, 20,  5,  5, 20, -20, 100 },
    { -20, -40, -5, -5, -5, -5, -40, -20 },
    {  20,  -5, 15,  3,  3, 15,  -5,  20 },
    {   5,  -5,  3,  3,  3,  3,  -5,   5 },
    {   5,  -5,  3,  3,  3,  3,  -5,   5 },
    {  20,  -5, 15,  3,  3, 15,  -5,  20 },
    { -20, -40, -5, -5, -5, -5, -40, -20 },
    { 100, -20, 20,  5,  5, 20, -20, 100 }
};

//--------------------------------------------------------------------
//  ����`
//--------------------------------------------------------------------

enum { EMP = 0, BLK = 1, WHT = -1, OUTSIDE = INT_MIN };    // �}�X�ڂ̏��
enum { DEC_RAW = -1, INC_RAW = 1 };    // �s�̑���
enum { DEC_COL = -1, INC_COL = 1 };    // ��̑���

//--------------------------------------------------------------------
//  �֐����^�錾
//--------------------------------------------------------------------

void init_board(int board[][SIZE]);     // �Ֆʂ̏�����
void print_board(int board[][SIZE]);    // �Ֆʂ̕\��
bool search_legal_move(int board[][SIZE], int player);                             // ���@�I�Ȏ肪���邩���ׂ�(���ׂẴ}�X�𑖍�)
void get_input_human(int board[][SIZE], int player, int *y, int *x);               // �ł���̓���(�l��)
void ai(int board[][SIZE], int player, int *y, int *x);                            // AI�ɂ��ł���̌���
int min_max(int board[][SIZE], int player, bool flag, int level);                  // �~�j�}�b�N�X�@
void record_board(int board[][SIZE], int level);                                   // ���݂̔Ֆʂ��L�^����
void undo_board(int board[][SIZE], int *player, int level);                        // �łO(1��O)�ɖ߂�
int value_board(int board[][SIZE]);                                                // �]���֐�
bool is_legal_move(int board[][SIZE], int player, int y, int x);                   // �ł���̍��@�����m���߂�
int count_turn_over(int board[][SIZE], int player, int y, int x, int d, int e);    // ���񂾐΂̐���Ԃ�(����������1�����̂�)
void set_and_turn_over(int board[][SIZE], int player, int y, int x);               // �΂�u�� �� ���񂾐΂𗠕Ԃ�
void count_stone(int board[][SIZE], int *c1, int *c2);                             // ���v���C���[�̐΂̌��𐔂���
void print_pass(int player);    // �p�X��\������
void game(void);                // �I�Z���Q�[����1��s��

//====================================================================
//  �{�̏���
//====================================================================

//--------------------------------------------------------------------
//  main�֐�
//--------------------------------------------------------------------

int main(void)
{
    puts("======================");
    puts("���o�[�V�Q�[���X�^�[�g�I");
    puts("======================");
    puts("�v���C���[ : ����   �R���s���[�^ : ����");
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
            board[i][j] = OUTSIDE;
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

bool search_legal_move(int board[][SIZE], int player)
{
    int i, j;

    //--- ���ׂẴ}�X�𑖍�
    for ( i = FIRST; i <= LAST; i++ ) {
        for ( j = FIRST; j <= LAST; j++ ) {
            if ( is_legal_move(board, player, i, j) == true ) {    // ���@�I�Ȏ肪���݂���Ȃ��
                return true;
            }
        }
    }
    return false;
}

//--------------------------------------------------------------------
//  �ł���̓���(�l��)
//--------------------------------------------------------------------

void get_input_human(int board[][SIZE], int player, int *y, int *x)
{
    char str[10];    // ���͂��������񂪓���o�b�t�@

    //Sleep(1000);(1000);
    puts("�v���C���[�̃^�[���ł�.");
    //--- �ł���̓���
    while ( 1 ) {
        printf("> ");                      // ���͂𑣂��v�����v�g
        fgets(str, sizeof(str), stdin);    // �ő�Ŕz��str�̑傫�����W�����͂��當�����1�s�ǂݍ���
        *x = str[0] - 'a' + 1;             // �Ⴆ��'a'��ǂݍ��ނƁA*x�ɓ���l��1�ƂȂ�悤�ɂ��Ă���
        *y = str[1] - '1' + 1;             // �Ⴆ��'1'��ǂݍ��ނƁA*y�ɓ���l��1�ƂȂ�悤�ɂ��Ă���
        if ( is_legal_move(board, player, *y, *x) == true ) {    // �ł��肪���@�I�Ȃ��
            printf("%c%c�ɑł��܂���.\n", str[0], str[1]);
            return;
        }
        puts("�����ɂ͑łĂ܂���.");
    }
}

//--------------------------------------------------------------------
//  AI�ɂ��ł���̌���
//--------------------------------------------------------------------

void ai(int board[][SIZE], int player, int *y, int *x)
{
    Sleep(2000);
    puts("�R���s���[�^�̃^�[���ł�.");
    Sleep(1000);
    puts("�v�l�����");
    //--- �~�j�}�b�N�X�@�Ő΂�łꏊ�����߂�
    // �߂��Ă���l�́AbestX + bestY * SIZE
    int tmp = min_max(board, player, true, SEARCH_LEVEL);

    //--- �łꏊ�����߂�
    *x = tmp % SIZE;
    *y = tmp / SIZE;
    
    Sleep(2000);    // ���x�~������
    printf("�R���s���[�^��%c%c�ɑł��܂���.\n", (*x)+'a'-1, (*y)+'1'-1);
    Sleep(2000);
}

//--------------------------------------------------------------------
//  �~�j�}�b�N�X�@(flag : AI�̎�Ԃ̂Ƃ�true�A�v���C���[�̎�Ԃ̂Ƃ�false)
//--------------------------------------------------------------------

int min_max(int board[][SIZE], int player, bool flag, int level)
{
    int value;         // �m�[�h�̕]���l
    int childValue;    // �q�m�[�h����`�d���Ă����]���l
    int bestX = 0;     // �~�j�}�b�N�X�@�ŋ��߂��A�ő�̕]���l�����ꏊ
    int bestY = 0;
    int y, x;
    
    //--- �Q�[���؂̖��[�ł͔Ֆʕ]��
    // ���̑��̃m�[�h�ł�MIN or MAX�œ`�d����
    if ( level == 0 ) {
        return value_board(board);    // �Ֆʂ�]�����ĕ]���l�����߂�
    }

    if ( flag = TRUE ) {
        value = INT_MIN;    // AI�̎�Ԃł͍ő�̕]���l�����������̂ŁA�ŏ��ɍŏ��l���Z�b�g���Ă���
    } else {
        value = INT_MAX;    // �v���C���[�̎�Ԃł͍ŏ��̕]���l�����������̂ŁA�ŏ��ɍő�l���Z�b�g���Ă���
    }

    //--- �����p�X�̏ꍇ�͂��̂܂ܔՖʕ]���l��Ԃ�
    if ( search_legal_move(board, player) == false ) {
        return value_board(board);
    }

    //--- �łĂ�Ƃ���͂��ׂĎ���
    for ( y = FIRST; y <= LAST; y++ ) {
        for ( x = FIRST; x <= LAST; x++ ) {
            if ( is_legal_move(board, player, y, x) == true ) {
                //printf("%d %d\n", y, x);
                //--- ���݂̔Ֆʂ��L�^���Ă���
                record_board(board, level);
                set_and_turn_over(board, player, y, x);   // �����ɑł��Ă݂� & �Ђ�����Ԃ�
                player *= -1;                             // �v���C���[���
                //--- �q�m�[�h�̕]���l���v�Z(�ċA)
                childValue = min_max(board, player, !flag, level-1);    // ���x�͑���̔�
                //--- �q�m�[�h�Ƃ��̃m�[�h�̕]���l���r����
                if ( flag == true ) {
                    //--- AI�̃m�[�h�Ȃ�q�m�[�h�̒��ōő�̕]���l��I��
                    if ( childValue > value ) {
                        value = childValue;
                        bestX = x;
                        bestY = y;
                    }
                } else {
                    //--- �v���C���[�̃m�[�h�Ȃ�q�m�[�h�̒��ōŏ��̕]���l��I��
                    if ( childValue < value ) {
                        value = childValue;
                        //bestX = x;
                        //bestY = y;
                    }
                }
                undo_board(board, &player, level);    // �łO�ɖ߂�
                //printf("bX = %d  bY = %d\n", bestX, bestY);
            }
        }
    }

    if ( level == SEARCH_LEVEL ) {
        //printf("%d\n", value);
        return (bestX + bestY * SIZE);    // ���[�g�m�[�h�Ȃ�ő�]���l�����ꏊ��Ԃ�
    } else {
        return value;    // �q�m�[�h�Ȃ�m�[�h�̕]���l��Ԃ�
    }
}

//--------------------------------------------------------------------
//  ���݂̔Ֆʂ��L�^����
//--------------------------------------------------------------------

void record_board(int board[][SIZE], int level)
{
    int y, x;
    
    for ( y = FIRST; y <= LAST; y++ ) {
        for ( x = FIRST; x <= LAST; x++ ) {
            copy[SEARCH_LEVEL-level][y][x] = board[y][x];
        }
    }
}

//--------------------------------------------------------------------
//  �łO(1��O)�ɖ߂�
//--------------------------------------------------------------------

void undo_board(int board[][SIZE], int *player, int level)
{
    int y, x;
    
    for ( y = FIRST; y <= LAST; y++ ) {
        for ( x = FIRST; x <= LAST; x++ ) {
            board[y][x] = copy[SEARCH_LEVEL-level][y][x];
        }
    }
    (*player) *= -1;    // �v���C���[�����ɖ߂�
}

//--------------------------------------------------------------------
//  �]���֐�
//--------------------------------------------------------------------

int value_board(int board[][SIZE])
{
    int value = 0;
    int y, x;
    
    for ( y = FIRST; y <= LAST; y++ ) {
        for ( x = FIRST; x <= LAST; x++ ) {
            value += board[y][x] * value_of_place[y-1][x-1];    // �ł��ꂽ�΂Ƃ��̏ꏊ�̉��l�������đ����Ă���
        }
    }
    
    return -value;    // ����(AI)���L���ȂƂ��͕]���l�����ɂȂ�(WHT=-1�ɂ��)�̂ŕ����𔽓]����
}

//--------------------------------------------------------------------
//  �ł���̍��@�����m���߂�
//--------------------------------------------------------------------

bool is_legal_move(int board[][SIZE], int player, int y, int x)
{
    if ( y < FIRST || y > LAST || x < FIRST || x > LAST ) { return false; }    // �ՖʊO
    if ( board[y][x] != EMP ) { return false; }                    // ���ɐ΂��u����Ă���
    if ( count_turn_over(board, player, y, x, DEC_RAW,     0   ) > 0 ) { return true; }    // ��
    if ( count_turn_over(board, player, y, x, INC_RAW,     0   ) > 0 ) { return true; }    // ��
    if ( count_turn_over(board, player, y, x,    0,     DEC_COL) > 0 ) { return true; }    // ��
    if ( count_turn_over(board, player, y, x,    0,     INC_COL) > 0 ) { return true; }    // �E
    if ( count_turn_over(board, player, y, x, DEC_RAW,  DEC_COL) > 0 ) { return true; }    // ����
    if ( count_turn_over(board, player, y, x, DEC_RAW,  INC_COL) > 0 ) { return true; }    // �E��
    if ( count_turn_over(board, player, y, x, INC_RAW,  DEC_COL) > 0 ) { return true; }    // ����
    if ( count_turn_over(board, player, y, x, INC_RAW,  INC_COL) > 0 ) { return true; }    // �E��
    return false;    // ���Ԃ�΂�1���Ȃ�����(��@)
}

//--------------------------------------------------------------------
//  ���񂾐΂̐���Ԃ�(����������1�����̂�)
//--------------------------------------------------------------------

int count_turn_over(int board[][SIZE], int player, int y, int x, int d, int e)
{
    int i;

    //--- �΂�u�����}�X����w������ɑ���(�����}�X������̐΂ł����)
    for ( i = 1; board[y+(i*d)][x+(i*e)] == player * -1; i++ ) { ; }

    //--- ���񂾐΂̌���Ԃ�
    if ( board[y+(i*d)][x+(i*e)] == player ) {    // �����I���n�_�������̐΂Ȃ��
        return i-1;    // �Ԃɂ���������̐΂̌���Ԃ�
    } else {
        return 0;      // �����I���n�_�������̐΂łȂ��Ȃ��0��Ԃ�
    }
}

//--------------------------------------------------------------------
//  �΂�u�� �� ���񂾐΂𗠕Ԃ�
//--------------------------------------------------------------------

void set_and_turn_over(int board[][SIZE], int player, int y, int x)
{
    int count;    // 1�����������ɂ����鋲�񂾐΂̌�
    int d, e;     // ����������������߂�ϐ�
    int i;

    //--- (0,0)�����ȊO��8�����𑖍����ċ��񂾐΂𗠕Ԃ�
    for ( d = DEC_RAW; d <= INC_RAW; d++ ) {         // �㉺����
        for ( e = DEC_COL; e <= INC_COL; e++ ) {     // ���E����
            if ( d == 0 && e == 0 ) { continue; }    // �ǂ̕����ɂ��������Ȃ��̂Ŗ���
            count = count_turn_over(board, player, y, x, d, e);    // d,e�����̋��񂾐΂̌�
            for ( i = 1; i <= count; i++ ) {
                board[y+(i*d)][x+(i*e)] = player;    // d,e�����̋��񂾐�(count��)�𗠕Ԃ�
            }
        }
    }
    board[y][x] = player;    // �ł���̈ʒu�ɐ΂�u��
}

//--------------------------------------------------------------------
//  ���v���C���[�̐΂̌��𐔂���
//--------------------------------------------------------------------

void count_stone(int board[][SIZE], int *c1, int *c2)
{
    int x, y;

    //--- �΂̌��̏�����
    *c1 = *c2 = 0;

    //--- ���v���C���[�̐΂̌��̌v�Z
    for ( y = FIRST; y <= LAST; y++ ) {
        for ( x = FIRST; x <= LAST; x++ ) {
            switch ( board[y][x] ) {
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
//  �p�X��\������
//--------------------------------------------------------------------

void print_pass(int player)
{
    switch ( player ) {
        case BLK :
            puts("�v���C���[�ɂ͐΂�u����ꏊ������܂���. �p�X���܂�.");
            break;
        case WHT :
            puts("�R���s���[�^�ɂ͐΂�u����ꏊ������܂���. �p�X���܂�.");
            break;                
    }
}

//--------------------------------------------------------------------
//  ���o�[�V�Q�[����1��s��
//--------------------------------------------------------------------

void game(void)
{
    int board[SIZE][SIZE];    // �Ֆ�
    int player;       // �v���C���[
    int x, y;         // �ł���̈ʒu
    int c1, c2;       // �Տ�̐΂̌�

    //--- ��������
    init_board(board);    // �Ֆʂ̏�����
    player = BLK;         // ���͍���

    //--- �Q�[���i�s����
    while ( 1 ) {
        print_board(board);    // �Ֆʂ̕\��
        if ( search_legal_move(board, player) == false ) {        // ���@�I�Ȏ肪�Ȃ��Ȃ��
            //Sleep(1000);(1000);
            print_pass(player);    // �p�X��\������
            player *= -1;                                         // �v���C���[���
            if ( search_legal_move(board, player) == false ) {    // ��サ�Ă����@�I�Ȏ肪�Ȃ��Ȃ��
                //Sleep(1000);(1000);
                print_pass(player);    // �p�X��\������
                break;                                            // ���v���C���[�Ƃ��p�X�������߃Q�[���I��
            }
        }
        if ( player == BLK ) {
            get_input_human(board, player, &y, &x);    // �ł���̓���
        } else {
            ai(board, player, &y, &x);             // �R���s���[�^�̑ł���
        }
        set_and_turn_over(board, player, y, x);    // �΂�u�� �� ���񂾐΂𗠕Ԃ�
        player *= -1;                              // �v���C���[���
    }
    
    //--- ���v���C���[�̐΂̌����v�Z
    count_stone(board, &c1, &c2);
    
    //--- ���ʕ\��
    //Sleep(1000);(1000);
    puts("\n�Q�[���I��\n");
    //Sleep(1000);(1000);
    puts("<���΂̌�>");
    //Sleep(1000);(2000);
    printf("�v���C���[   : %d��\n�R���s���[�^ : %d��\n", c1, c2);    // ���v���C���[�̎��΂̌���\��
    if ( c1 - c2 > 0 ) {                 // ���΂̂ق��������Ȃ��
        puts("\n�v���C���[�̏����ł�.");
    } else if ( c1 - c2 < 0 ) {          // ���΂̂ق������Ȃ��Ȃ��
        puts("\n�R���s���[�^�̏����ł�.");
    } else {
        puts("\n���������ł�.");
    }
}
