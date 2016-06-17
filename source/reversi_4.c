//==========================================================
// 人間vsCOMのリバーシゲーム
// AI : ミニマックス法
// 盤面評価の方法 : 各マスへの重み付け
//==========================================================

#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <Windows.h>

//--------------------------------------------------------------------
//  マクロ定義
//--------------------------------------------------------------------

//--- オブジェクト形式マクロ
#define SIZE 10    // 盤のサイズ(8×8と上下左右両端の番兵各1つ分)
#define FIRST 1    // 1行目、1列目を表す
#define LAST 8     // 8行目、8列目を表す
#define SEARCH_LEVEL 6    // 先読みするレベル

//--------------------------------------------------------------------
//  大域宣言
//--------------------------------------------------------------------

int copy[SEARCH_LEVEL][SIZE][SIZE];            // 盤面記憶用の配列
int value_of_place[LAST][LAST] = {             // 盤面の各場所の価値
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
//  大域定義
//--------------------------------------------------------------------

enum { EMP = 0, BLK = 1, WHT = -1, OUTSIDE = INT_MIN };    // マス目の状態
enum { DEC_RAW = -1, INC_RAW = 1 };    // 行の増減
enum { DEC_COL = -1, INC_COL = 1 };    // 列の増減

//--------------------------------------------------------------------
//  関数原型宣言
//--------------------------------------------------------------------

void init_board(int board[][SIZE]);     // 盤面の初期化
void print_board(int board[][SIZE]);    // 盤面の表示
bool search_legal_move(int board[][SIZE], int player);                             // 合法的な手があるか調べる(すべてのマスを走査)
void get_input_human(int board[][SIZE], int player, int *y, int *x);               // 打ち手の入力(人間)
void ai(int board[][SIZE], int player, int *y, int *x);                            // AIによる打ち手の決定
int min_max(int board[][SIZE], int player, bool flag, int level);                  // ミニマックス法
void record_board(int board[][SIZE], int level);                                   // 現在の盤面を記録する
void undo_board(int board[][SIZE], int *player, int level);                        // 打つ前(1手前)に戻す
int value_board(int board[][SIZE]);                                                // 評価関数
bool is_legal_move(int board[][SIZE], int player, int y, int x);                   // 打ち手の合法性を確かめる
int count_turn_over(int board[][SIZE], int player, int y, int x, int d, int e);    // 挟んだ石の数を返す(走査方向は1方向のみ)
void set_and_turn_over(int board[][SIZE], int player, int y, int x);               // 石を置く ＆ 挟んだ石を裏返す
void count_stone(int board[][SIZE], int *c1, int *c2);                             // 両プレイヤーの石の個数を数える
void print_pass(int player);    // パスを表示する
void game(void);                // オセロゲームを1回行う

//====================================================================
//  本体処理
//====================================================================

//--------------------------------------------------------------------
//  main関数
//--------------------------------------------------------------------

int main(void)
{
    puts("======================");
    puts("リバーシゲームスタート！");
    puts("======================");
    puts("プレイヤー : 黒石   コンピュータ : 白石");
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
            board[i][j] = OUTSIDE;
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

bool search_legal_move(int board[][SIZE], int player)
{
    int i, j;

    //--- すべてのマスを走査
    for ( i = FIRST; i <= LAST; i++ ) {
        for ( j = FIRST; j <= LAST; j++ ) {
            if ( is_legal_move(board, player, i, j) == true ) {    // 合法的な手が存在するならば
                return true;
            }
        }
    }
    return false;
}

//--------------------------------------------------------------------
//  打ち手の入力(人間)
//--------------------------------------------------------------------

void get_input_human(int board[][SIZE], int player, int *y, int *x)
{
    char str[10];    // 入力した文字列が入るバッファ

    //Sleep(1000);(1000);
    puts("プレイヤーのターンです.");
    //--- 打ち手の入力
    while ( 1 ) {
        printf("> ");                      // 入力を促すプロンプト
        fgets(str, sizeof(str), stdin);    // 最大で配列strの大きさ分標準入力から文字列を1行読み込む
        *x = str[0] - 'a' + 1;             // 例えば'a'を読み込むと、*xに入る値が1となるようにしている
        *y = str[1] - '1' + 1;             // 例えば'1'を読み込むと、*yに入る値が1となるようにしている
        if ( is_legal_move(board, player, *y, *x) == true ) {    // 打ち手が合法的ならば
            printf("%c%cに打ちました.\n", str[0], str[1]);
            return;
        }
        puts("そこには打てません.");
    }
}

//--------------------------------------------------------------------
//  AIによる打ち手の決定
//--------------------------------------------------------------------

void ai(int board[][SIZE], int player, int *y, int *x)
{
    Sleep(2000);
    puts("コンピュータのターンです.");
    Sleep(1000);
    puts("思考中･･･");
    //--- ミニマックス法で石を打つ場所を決める
    // 戻ってくる値は、bestX + bestY * SIZE
    int tmp = min_max(board, player, true, SEARCH_LEVEL);

    //--- 打つ場所を求める
    *x = tmp % SIZE;
    *y = tmp / SIZE;
    
    Sleep(2000);    // 小休止を入れる
    printf("コンピュータは%c%cに打ちました.\n", (*x)+'a'-1, (*y)+'1'-1);
    Sleep(2000);
}

//--------------------------------------------------------------------
//  ミニマックス法(flag : AIの手番のときtrue、プレイヤーの手番のときfalse)
//--------------------------------------------------------------------

int min_max(int board[][SIZE], int player, bool flag, int level)
{
    int value;         // ノードの評価値
    int childValue;    // 子ノードから伝播してきた評価値
    int bestX = 0;     // ミニマックス法で求めた、最大の評価値を持つ場所
    int bestY = 0;
    int y, x;
    
    //--- ゲーム木の末端では盤面評価
    // その他のノードではMIN or MAXで伝播する
    if ( level == 0 ) {
        return value_board(board);    // 盤面を評価して評価値を決める
    }

    if ( flag = TRUE ) {
        value = INT_MIN;    // AIの手番では最大の評価値を見つけたいので、最初に最小値をセットしておく
    } else {
        value = INT_MAX;    // プレイヤーの手番では最小の評価値を見つけたいので、最初に最大値をセットしておく
    }

    //--- もしパスの場合はそのまま盤面評価値を返す
    if ( search_legal_move(board, player) == false ) {
        return value_board(board);
    }

    //--- 打てるところはすべて試す
    for ( y = FIRST; y <= LAST; y++ ) {
        for ( x = FIRST; x <= LAST; x++ ) {
            if ( is_legal_move(board, player, y, x) == true ) {
                //printf("%d %d\n", y, x);
                //--- 現在の盤面を記録しておく
                record_board(board, level);
                set_and_turn_over(board, player, y, x);   // 試しに打ってみる & ひっくり返す
                player *= -1;                             // プレイヤー交代
                //--- 子ノードの評価値を計算(再帰)
                childValue = min_max(board, player, !flag, level-1);    // 今度は相手の番
                //--- 子ノードとこのノードの評価値を比較する
                if ( flag == true ) {
                    //--- AIのノードなら子ノードの中で最大の評価値を選ぶ
                    if ( childValue > value ) {
                        value = childValue;
                        bestX = x;
                        bestY = y;
                    }
                } else {
                    //--- プレイヤーのノードなら子ノードの中で最小の評価値を選ぶ
                    if ( childValue < value ) {
                        value = childValue;
                        //bestX = x;
                        //bestY = y;
                    }
                }
                undo_board(board, &player, level);    // 打つ前に戻す
                //printf("bX = %d  bY = %d\n", bestX, bestY);
            }
        }
    }

    if ( level == SEARCH_LEVEL ) {
        //printf("%d\n", value);
        return (bestX + bestY * SIZE);    // ルートノードなら最大評価値を持つ場所を返す
    } else {
        return value;    // 子ノードならノードの評価値を返す
    }
}

//--------------------------------------------------------------------
//  現在の盤面を記録する
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
//  打つ前(1手前)に戻す
//--------------------------------------------------------------------

void undo_board(int board[][SIZE], int *player, int level)
{
    int y, x;
    
    for ( y = FIRST; y <= LAST; y++ ) {
        for ( x = FIRST; x <= LAST; x++ ) {
            board[y][x] = copy[SEARCH_LEVEL-level][y][x];
        }
    }
    (*player) *= -1;    // プレイヤーも元に戻す
}

//--------------------------------------------------------------------
//  評価関数
//--------------------------------------------------------------------

int value_board(int board[][SIZE])
{
    int value = 0;
    int y, x;
    
    for ( y = FIRST; y <= LAST; y++ ) {
        for ( x = FIRST; x <= LAST; x++ ) {
            value += board[y][x] * value_of_place[y-1][x-1];    // 打たれた石とその場所の価値をかけて足していく
        }
    }
    
    return -value;    // 白石(AI)が有利なときは評価値が負になる(WHT=-1による)ので符号を反転する
}

//--------------------------------------------------------------------
//  打ち手の合法性を確かめる
//--------------------------------------------------------------------

bool is_legal_move(int board[][SIZE], int player, int y, int x)
{
    if ( y < FIRST || y > LAST || x < FIRST || x > LAST ) { return false; }    // 盤面外
    if ( board[y][x] != EMP ) { return false; }                    // 既に石が置かれている
    if ( count_turn_over(board, player, y, x, DEC_RAW,     0   ) > 0 ) { return true; }    // 上
    if ( count_turn_over(board, player, y, x, INC_RAW,     0   ) > 0 ) { return true; }    // 下
    if ( count_turn_over(board, player, y, x,    0,     DEC_COL) > 0 ) { return true; }    // 左
    if ( count_turn_over(board, player, y, x,    0,     INC_COL) > 0 ) { return true; }    // 右
    if ( count_turn_over(board, player, y, x, DEC_RAW,  DEC_COL) > 0 ) { return true; }    // 左上
    if ( count_turn_over(board, player, y, x, DEC_RAW,  INC_COL) > 0 ) { return true; }    // 右上
    if ( count_turn_over(board, player, y, x, INC_RAW,  DEC_COL) > 0 ) { return true; }    // 左下
    if ( count_turn_over(board, player, y, x, INC_RAW,  INC_COL) > 0 ) { return true; }    // 右下
    return false;    // 裏返る石が1つもなかった(違法)
}

//--------------------------------------------------------------------
//  挟んだ石の数を返す(走査方向は1方向のみ)
//--------------------------------------------------------------------

int count_turn_over(int board[][SIZE], int player, int y, int x, int d, int e)
{
    int i;

    //--- 石を置いたマスから指定方向に走査(走査マスが相手の石である間)
    for ( i = 1; board[y+(i*d)][x+(i*e)] == player * -1; i++ ) { ; }

    //--- 挟んだ石の個数を返す
    if ( board[y+(i*d)][x+(i*e)] == player ) {    // 走査終了地点が自分の石ならば
        return i-1;    // 間にあった相手の石の個数を返す
    } else {
        return 0;      // 走査終了地点が自分の石でないならば0を返す
    }
}

//--------------------------------------------------------------------
//  石を置く ＆ 挟んだ石を裏返す
//--------------------------------------------------------------------

void set_and_turn_over(int board[][SIZE], int player, int y, int x)
{
    int count;    // 1方向走査時における挟んだ石の個数
    int d, e;     // 走査する方向を決める変数
    int i;

    //--- (0,0)方向以外の8方向を走査して挟んだ石を裏返す
    for ( d = DEC_RAW; d <= INC_RAW; d++ ) {         // 上下方向
        for ( e = DEC_COL; e <= INC_COL; e++ ) {     // 左右方向
            if ( d == 0 && e == 0 ) { continue; }    // どの方向にも走査しないので無視
            count = count_turn_over(board, player, y, x, d, e);    // d,e方向の挟んだ石の個数
            for ( i = 1; i <= count; i++ ) {
                board[y+(i*d)][x+(i*e)] = player;    // d,e方向の挟んだ石(count個)を裏返す
            }
        }
    }
    board[y][x] = player;    // 打ち手の位置に石を置く
}

//--------------------------------------------------------------------
//  両プレイヤーの石の個数を数える
//--------------------------------------------------------------------

void count_stone(int board[][SIZE], int *c1, int *c2)
{
    int x, y;

    //--- 石の個数の初期化
    *c1 = *c2 = 0;

    //--- 両プレイヤーの石の個数の計算
    for ( y = FIRST; y <= LAST; y++ ) {
        for ( x = FIRST; x <= LAST; x++ ) {
            switch ( board[y][x] ) {
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
//  パスを表示する
//--------------------------------------------------------------------

void print_pass(int player)
{
    switch ( player ) {
        case BLK :
            puts("プレイヤーには石を置ける場所がありません. パスします.");
            break;
        case WHT :
            puts("コンピュータには石を置ける場所がありません. パスします.");
            break;                
    }
}

//--------------------------------------------------------------------
//  リバーシゲームを1回行う
//--------------------------------------------------------------------

void game(void)
{
    int board[SIZE][SIZE];    // 盤面
    int player;       // プレイヤー
    int x, y;         // 打ち手の位置
    int c1, c2;       // 盤上の石の個数

    //--- 初期処理
    init_board(board);    // 盤面の初期化
    player = BLK;         // 先手は黒石

    //--- ゲーム進行部分
    while ( 1 ) {
        print_board(board);    // 盤面の表示
        if ( search_legal_move(board, player) == false ) {        // 合法的な手がないならば
            //Sleep(1000);(1000);
            print_pass(player);    // パスを表示する
            player *= -1;                                         // プレイヤー交代
            if ( search_legal_move(board, player) == false ) {    // 交代しても合法的な手がないならば
                //Sleep(1000);(1000);
                print_pass(player);    // パスを表示する
                break;                                            // 両プレイヤーともパスしたためゲーム終了
            }
        }
        if ( player == BLK ) {
            get_input_human(board, player, &y, &x);    // 打ち手の入力
        } else {
            ai(board, player, &y, &x);             // コンピュータの打ち手
        }
        set_and_turn_over(board, player, y, x);    // 石を置く ＆ 挟んだ石を裏返す
        player *= -1;                              // プレイヤー交代
    }
    
    //--- 両プレイヤーの石の個数を計算
    count_stone(board, &c1, &c2);
    
    //--- 結果表示
    //Sleep(1000);(1000);
    puts("\nゲーム終了\n");
    //Sleep(1000);(1000);
    puts("<自石の個数>");
    //Sleep(1000);(2000);
    printf("プレイヤー   : %d個\nコンピュータ : %d個\n", c1, c2);    // 両プレイヤーの自石の個数を表示
    if ( c1 - c2 > 0 ) {                 // 黒石のほうが多いならば
        puts("\nプレイヤーの勝ちです.");
    } else if ( c1 - c2 < 0 ) {          // 黒石のほうが少ないならば
        puts("\nコンピュータの勝ちです.");
    } else {
        puts("\n引き分けです.");
    }
}
