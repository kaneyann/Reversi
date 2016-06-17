//==========================================================
// 人間vs人間のリバーシゲーム
//==========================================================

#include <stdio.h>

//--------------------------------------------------------------------
//  マクロ定義
//--------------------------------------------------------------------

//--- オブジェクト形式マクロ
#define SIZE 10    // 盤のサイズ(8×8と上下左右両端の番兵各1つ分)
#define FIRST 1    // 1行目、1列目を表す
#define LAST 8     // 8行目、8列目を表す
#define TRUE  1    // 真は1
#define FALSE 0    // 偽は0

//--- 関数形式マクロ
#define partner(player) ((3) - (player))    // 1の相手は2、2の相手は1

//--------------------------------------------------------------------
//  大域宣言
//--------------------------------------------------------------------
    
//--------------------------------------------------------------------
//  大域定義
//--------------------------------------------------------------------

enum { EMP = 0, BLK = 1, WHT = 2, OUT = -1 };    // マス目の状態
enum { DEC_RAW = -1, INC_RAW = 1 };    // 行の増減
enum { DEC_COL = -1, INC_COL = 1 };    // 列の増減

//--------------------------------------------------------------------
//  関数原型宣言
//--------------------------------------------------------------------

void init_board(int board[][SIZE]);     // 盤面の初期化
void print_board(int board[][SIZE]);    // 盤面の表示
int search_legal_move(int board[][SIZE], int player);                              // 合法的な手があるか調べる(すべてのマスを走査)
void get_input_human(int board[][SIZE], int player, int *p, int *q);               // 打ち手の入力(人間)
int is_legal_move(int board[][SIZE], int player, int p, int q);                    // 打ち手の合法性を確かめる
int count_turn_over(int board[][SIZE], int player, int p, int q, int d, int e);    // 挟んだ石の数を返す(走査方向は1方向のみ)
void set_and_turn_over(int board[][SIZE], int player, int p, int q);               // 石を置く ＆ 挟んだ石を裏返す
void count_stone(int board[][SIZE], int *c1, int *c2);                             // 両プレイヤーの石の個数を数える
void game(void);           // オセロゲームを1回行う

//====================================================================
//  本体処理
//====================================================================

//--------------------------------------------------------------------
//  main関数
//--------------------------------------------------------------------

int main(void)
{
    puts("1人リバーシスタート！");
    puts("Player 1 : 黒石   Player 2 : 白石");
    puts("※打ち手の入力例 : b2\n");
    game();

    return 0;
}

//--------------------------------------------------------------------
//  盤面の初期化
//--------------------------------------------------------------------

void init_board(int board[][SIZE])
{
    int i, j;

    //--- すべてのマスを盤外値で初期化する
    for ( i = 0; i < SIZE; i++ ) {
        for ( j = 0; j < SIZE; j++ ) {
            board[i][j] = OUT;
        }
    }

    //--- 実際に使用するマスを空にする
    for ( i = FIRST; i <= LAST; i++ ) {
        for ( j = FIRST; j <= LAST; j++ ) {
            board[i][j] = EMP;
        }
    }

    //--- 盤面の中央を黒と白にする
    board[4][5] = board[5][4] = BLK;
    board[4][4] = board[5][5] = WHT;
}

//--------------------------------------------------------------------
//  盤面の表示
//--------------------------------------------------------------------

void print_board(int board[][SIZE])
{
    int i, j;

    puts("");
    puts("   ａｂｃｄｅｆｇｈ ");    // 列記号の表示
    puts("  ------------------");

    //--- 行
    for ( i = FIRST; i <= LAST; i++ ) {
        printf("%d| ", i);           // 行番号の表示
        //--- 列
        for ( j = FIRST; j <= LAST; j++ ) {
            switch( board[i][j] ) {
            case 0 :
                printf("・");    // 空の表示
                break;
            case BLK :
                printf("●");    // 黒石の表示
                break;
            case WHT :
                printf("○");    // 白石の表示
                break;
            }
        }
        puts(" |");
    }

    puts("  ------------------");
}

//--------------------------------------------------------------------
//  合法的な手があるか調べる(すべてのマスを走査)
//--------------------------------------------------------------------

int search_legal_move(int board[][SIZE], int player)
{
    int i, j;

    //--- すべてのマスを走査
    for ( i = FIRST; i <= LAST; i++ ) {
        for ( j = FIRST; j <= LAST; j++ ) {
            if ( is_legal_move(board, player, i, j) == TRUE ) {    // 合法的な手が存在するならば
                return TRUE;
            }
        }
    }
    return FALSE;
}

//--------------------------------------------------------------------
//  打ち手の入力(人間)
//--------------------------------------------------------------------

void get_input_human(int board[][SIZE], int player, int *p, int *q)
{
    char str[10];    // 入力した文字列が入るバッファ

    printf("Player %d のターンです.\n", player);

    //--- 打ち手の入力
    while ( 1 ) {
        printf("> ");                      // 入力を促すプロンプト
        fgets(str, sizeof(str), stdin);    // 最大で配列strの大きさ分標準入力から文字列を1行読み込む
        *q = str[0] - 'a' + 1;             // 例えば'a'を読み込むと、*qに入る値が1となるようにしている
        *p = str[1] - '1' + 1;             // 例えば'1'を読み込むと、*pに入る値が1となるようにしている
        if ( is_legal_move(board, player, *p, *q) == TRUE ) {    // 打ち手が合法的ならば
            return;
        }
        puts("そこには置けません.");
    }
}

//--------------------------------------------------------------------
//  打ち手の合法性を確かめる
//--------------------------------------------------------------------

int is_legal_move(int board[][SIZE], int player, int p, int q)
{
    if ( p < FIRST || p > LAST || q < FIRST || q > LAST ) { return FALSE; }    // 盤面外
    if ( board[p][q] != EMP ) { return FALSE; }                    // 既に石が置かれている
    if ( count_turn_over(board, player, p, q, DEC_RAW,     0   ) > 0 ) { return TRUE; }    // 上
    if ( count_turn_over(board, player, p, q, INC_RAW,     0   ) > 0 ) { return TRUE; }    // 下
    if ( count_turn_over(board, player, p, q,    0,     DEC_COL) > 0 ) { return TRUE; }    // 左
    if ( count_turn_over(board, player, p, q,    0,     INC_COL) > 0 ) { return TRUE; }    // 右
    if ( count_turn_over(board, player, p, q, DEC_RAW,  DEC_COL) > 0 ) { return TRUE; }    // 左上
    if ( count_turn_over(board, player, p, q, DEC_RAW,  INC_COL) > 0 ) { return TRUE; }    // 右上
    if ( count_turn_over(board, player, p, q, INC_RAW,  DEC_COL) > 0 ) { return TRUE; }    // 左下
    if ( count_turn_over(board, player, p, q, INC_RAW,  INC_COL) > 0 ) { return TRUE; }    // 右下
    return FALSE;    // 裏返る石が1つもなかった(違法)
}

//--------------------------------------------------------------------
//  挟んだ石の数を返す(走査方向は1方向のみ)
//--------------------------------------------------------------------

int count_turn_over(int board[][SIZE], int player, int p, int q, int d, int e)
{
    int i;

    //--- 石を置いたマスから指定方向に走査(走査マスが相手の石である間)
    for ( i = 1; board[p+(i*d)][q+(i*e)] == partner(player); i++ ) { ; }

    //--- 挟んだ石の個数を返す
    if ( board[p+(i*d)][q+(i*e)] == player ) {    // 走査終了地点が自分の石ならば
        return i-1;    // 間にあった相手の石の個数を返す
    } else {
        return 0;      // 走査終了地点が自分の石でないならば0を返す
    }
}

//--------------------------------------------------------------------
//  石を置く ＆ 挟んだ石を裏返す
//--------------------------------------------------------------------

void set_and_turn_over(int board[][SIZE], int player, int p, int q)
{
    int count;    // 1方向走査時における挟んだ石の個数
    int d, e;     // 走査する方向を決める変数
    int i;

    //--- (0,0)方向以外の8方向を走査して挟んだ石を裏返す
    for ( d = DEC_RAW; d <= INC_RAW; d++ ) {         // 上下方向
        for ( e = DEC_COL; e <= INC_COL; e++ ) {     // 左右方向
            if ( d == 0 && e == 0 ) { continue; }    // どの方向にも走査しないので無視
            count = count_turn_over(board, player, p, q, d, e);    // d,e方向の挟んだ石の個数
            for ( i = 1; i <= count; i++ ) {
                board[p+(i*d)][q+(i*e)] = player;    // d,e方向の挟んだ石(count個)を裏返す
            }
        }
    }
    board[p][q] = player;    // 打ち手の位置に石を置く
}

//--------------------------------------------------------------------
//  両プレイヤーの石の個数を数える
//--------------------------------------------------------------------

void count_stone(int board[][SIZE], int *c1, int *c2)
{
    int i, j;

    //--- 石の個数の初期化
    *c1 = *c2 = 0;

    //--- 両プレイヤーの石の個数の計算
    for ( i = FIRST; i <= LAST; i++ ) {
        for ( j = FIRST; j <= LAST; j++ ) {
            switch ( board[i][j] ) {
            case BLK :      // 黒石ならば
                (*c1)++;
                break;
            case WHT :      // 白石ならば 
                (*c2)++;
                break;
            }
        }
    }
}

//--------------------------------------------------------------------
//  リバーシゲームを1回行う
//--------------------------------------------------------------------

void game(void)
{
    int board[SIZE][SIZE];    // 盤面
    int player;       // プレイヤー
    int p, q;         // 打ち手の位置
    int c1, c2;       // 盤上の石の個数

    //--- 初期処理
    init_board(board);    // 盤面の初期化
    player = BLK;         // 先手は黒石

    //--- ゲーム進行部分
    while ( 1 ) {
        print_board(board);    // 盤面の表示
        if ( search_legal_move(board, player) == FALSE ) {        // 合法的な手がないならば
            printf("Player %d には石を置ける場所がありません. パスします.\n", player);
            player = partner(player);                             // プレイヤー交代
            if ( search_legal_move(board, player) == FALSE ) {    // 交代しても合法的な手がないならば
                printf("Player %d には石を置ける場所がありません. パスします.\n\n", player);
                break;                                            // 両プレイヤーともパスしたためゲーム終了
            }
        }
        get_input_human(board, player, &p, &q);    // 打ち手の入力
        set_and_turn_over(board, player, p, q);    // 石を置く ＆ 挟んだ石を裏返す
        player = partner(player);           // プレイヤー交代
    }
    
    //--- 両プレイヤーの石の個数を計算
    count_stone(board, &c1, &c2);
    
    //--- 結果表示
    puts("ゲーム終了\n");
    puts("<自石の個数>");
    printf("Player 1 : %d個\nPlayer 2 : %d個\n", c1, c2);    // 両プレイヤーの自石の個数を表示
    if ( c1 - c2 > 0 ) {                 // 黒石のほうが多いならば
        puts("Player 1 の勝ちです.");
    } else if ( c1 - c2 < 0 ) {          // 黒石のほうが少ないならば
        puts("Player 2 の勝ちです.");
    } else {
        puts("引き分けです.");
    }
}
