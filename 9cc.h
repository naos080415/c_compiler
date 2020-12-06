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
    ND_LVAR,    // ローカル変数
    ND_EQ,      // ==
    ND_NE,      // !=
    ND_LT,      // <
    ND_LE,      // <=
    ND_NUM,     // 整数
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
    NodeKind kind;  // ノードの型
    Node *lhs;      // 左辺
    Node *rhs;      // 右辺
    int val;        // kindがND_NUMの場合のみ使う
    int offset;     // kindがND_LVARの場合のみ使う
};

// 現在着目しているトークン
extern Token *token;

// 入力プログラム
extern  char *user_input;

extern Node *code[100];

// プロトタイプ宣言(parse.c)
void error(char *fmt, ...);
void error_at(char *loc,char *fmt, ...);
/* 次のトークンが期待している記号のときには,トークンを1つ読み進めて
    真を返す.それ以外の場合には偽をかえす */
bool consume(char *op);
Token *consume_ident();
/* 次のトークンが期待している記号のときには,トークンを1つ読み進める.
    それ以外の場合にはエラーを報告する */
void expect(char *op);
/* 次のトークンが数値の場合,トークンを1つ読み進めてその数値を返す.
    それ以外の場合にはエラーを報告する. */
int expect_number();
bool at_eof();
Token *new_token(TokenKind kind,Token *cur,char *str,int len);   // 新しいトークンを作成してcurにつなげる 
Token *tokenize();   // 入力文字列pをトークナイズしてそれを返す


// プロトタイプ宣言(codegen.c)
/* エラーを報告するための関数
    printfと同じ引数を取る */
void gen(Node *node);
void program();
void gen_lval(Node *node);      // 左辺値の評価(左辺がアドレスになっているか?)
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