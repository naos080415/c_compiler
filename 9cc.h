#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// tokenize
typedef enum {
    TK_RESERVED,    // 記号
    TK_IDENT,       // 識別子
    TK_NUM,         // 整数トークン
    TK_CONSYS,      // 制御構文(if,else,while,for)
    TK_VAR,         // 変数の型(int)
    TK_RETURN,      // return    
    TK_EOF,         // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

// トークンの型
struct Token {
    TokenKind kind;     // トークンの型
    Token *next;        // 次の入力トークン
    int val;            // kindがTK_NUMの場合,その数値
    char *str;          // トークン文字列
    int len;            // トークンの長さ
};

// 抽象構文木のノートの種類
typedef enum {
    ND_ADD,     // +
    ND_SUB,     // -
    ND_MUL,     // *
    ND_DIV,     // /
    ND_ASSIGN,  // =(代入)
    ND_ADDR,    // &
    ND_DEREF,   // *
    ND_LVAR,    // ローカル変数
    ND_LVAR_DEF_INT,    // ローカル変数
    ND_EQ,      // ==
    ND_NE,      // !=
    ND_LT,      // <
    ND_LE,      // <=
    ND_BLOCK,   // { or }
    ND_IF,      // if
    ND_ELSE,    // else
    ND_WHILE,   // while
    ND_FOR,     // for
    ND_RETURN,  // return
    ND_FUNC,    // 関数(呼び出し)
    ND_FUNC_DEF,// 関数(定義)
    ND_NUM,     // 整数
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
    NodeKind kind;  // ノードの型
    Node *lhs;      // 左辺
    Node *rhs;      // 右辺
    Node **block;   // kindがND_BLOCKの場合のみ使う
    Node *argv;     // kindがND_FUNCの場合のみ使う(引数)
    char *name_func;    // kindがND_FUNCの場合のみ使う(一時的に作成する・後で削除予定???)
    int val;        // kindがND_NUMの場合のみ使う
    int offset;     // kindがND_LVARの場合のみ使う
};

typedef struct LVar LVar;

// ローカル変数の型
struct LVar {
    LVar *next;     // 次の変数,ない場合:NULL
    char *name;     // 変数名
    int len;        // 変数名の長さ
    int offset;     // RBPからのオフセット
};

// 予約語の定義
typedef enum{
    LV_IF,
    LV_WHILE,
    LV_FOR,
    LV_END,
} Label_keyword;

// 関数の引数の定義
typedef enum{
    FN_RDI,     // 第1引数(RDI)
    FN_RSI,     // 第2引数(RSI)
    FN_RDX,     // 第3引数(RDX)
    FN_RCX,     // 第4引数(RCX)
    FN_R8,      // 第5引数(R8)
    FN_R9,      // 第6引数(R9)
} Call_function;

// 現在着目しているトークン
extern Token *token;

// 入力プログラム
extern  char *user_input;

extern Node *code[100];

// ローカル変数
extern LVar *locals;

extern char label_name[10];
extern int label_cnt[LV_END];

void init();
// プロトタイプ宣言(parse.c)
void error(char *fmt, ...);
void error_at(char *loc,char *fmt, ...);
int is_alnum(char c);
// 制御構文であるかどうかの判定
int contorl_syntax(char *p);
// 変数の型であるかどうかの判定
int variable_syntax(char *p);
/* 次のトークンが期待している記号のときには,トークンを1つ読み進めて
    真を返す.それ以外の場合には偽をかえす */
bool consume(char *op);
bool consume_keyword(char *op);
Token *consume_ident();
/* 次のトークンが期待している記号のときには,トークンを1つ読み進める.
    それ以外の場合にはエラーを報告する */
void expect(char *op);
/* 次のトークンが数値の場合,トークンを1つ読み進めてその数値を返す.
    それ以外の場合にはエラーを報告する. */
int expect_number();
bool at_eof();
LVar *find_lvar(Token *tok);        // 変数名の検索(以前に定義されていないかどうか)
Token *new_token(TokenKind kind,Token *cur,char *str,int len);   // 新しいトークンを作成してcurにつなげる 
Token *tokenize();   // 入力文字列pをトークナイズしてそれを返す

// プロトタイプ宣言(codegen.c)
/* エラーを報告するための関数
    printfと同じ引数を取る */
void gen(Node *node);
void gen_lval(Node *node);      // 左辺値の評価(左辺がアドレスになっているか?)
char *lavel_contorl(Label_keyword kind);
void program();
Node *func();
Node *stmt();
Node *expr();
Node *assign();
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();
Node *variable_def();
