// tokenizeの実装
#include "9cc.h"

// 現在着目しているトークン
Token *token;

// 入力プログラム
char *user_input;

/* エラーを報告するための関数
    printfと同じ引数を取る */
void error_at(char *loc,char *fmt, ...)
{
    va_list ap;
    va_start(ap,fmt);

    int pos = loc - user_input;
    fprintf(stderr,"%s\n",user_input);
    fprintf(stderr,"%*s",pos," ");      // pos個の空白を出力
    fprintf(stderr,"^ ");
    vfprintf(stderr,fmt,ap);
    fprintf(stderr,"\n");
    exit(1);
}

/* エラーを報告するための関数
    printfと同じ引数を取る */
void error(char *fmt, ...)
{
    va_list ap;
    va_start(ap,fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr,"\n");
    exit(1);
}

/* 次のトークンが期待している記号のときには,トークンを1つ読み進めて
    真を返す.それ以外の場合には偽をかえす */
bool consume(char *op)
{
    if(token->kind != TK_RESERVED || strlen(op) != token->len 
        || memcmp(token->str,op,token->len))
        return false;
    token = token->next;
    return true;
}

bool consume_word(char *op)
{
    if(memcmp(token->str,op,token->len))
        return false;
    token = token->next;
    return true;
}

/* トークンがidentであるかどうかを調べる */
Token *consume_ident()
{
    if(token->kind != TK_IDENT)
        return NULL;
    
    Token *tok = token;
    token = token->next;
    return tok;
}

/* 次のトークンが期待している記号のときには,トークンを1つ読み進める.
    それ以外の場合にはエラーを報告する */
void expect(char *op)
{
    if(token->kind != TK_RESERVED || strlen(op) != token->len 
        || memcmp(token->str,op,token->len))
        error_at(token->str,"'%c'ではありません",op);
    token = token->next;
}

/* 次のトークンが数値の場合,トークンを1つ読み進めてその数値を返す.
    それ以外の場合にはエラーを報告する. */
int expect_number()
{
    if(token->kind != TK_NUM)
        error_at(token->str,"数ではありません");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof()
{
    return token->kind == TK_EOF;
}

// 変数を名前で検索する.(以前に定義されていないかの確認)
LVar *find_lvar(Token *tok)
{
    for(LVar *var = locals;var;var = var->next){
        if(var->len == tok->len && !memcmp(tok->str,var->name,var->len))
            return var;
    }
    return NULL;
}

// 新しいトークンを作成してcurにつなげる 
Token *new_token(TokenKind kind,Token *cur,char *str,int len)
{
    Token *tok = calloc(1,sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize()
{
    char *p = user_input;
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while(*p){
        
        // 空白文字をスキップ
        if(isspace(*p)){
            p++;
            continue;
        }

        // 2つの記号の演算子
        if(!memcmp(p,"==",2) || !memcmp(p,"!=",2)
            || !memcmp(p,"<=",2) || !memcmp(p,">=",2)){
            cur = new_token(TK_RESERVED,cur,p,2);
            p += 2;
            continue;
        }

        // 1つの記号の演算子
        if(strchr("+-*/=()<>;",*p)){
            cur = new_token(TK_RESERVED,cur,p,1);
            p++;
            continue;
        }

        // 数字である場合
        if(isdigit(*p)){
            cur = new_token(TK_NUM,cur,p,0);        // 文字列の長さを一旦0で代入
            char *q = p;
            cur->val = strtol(p,&p,10);
            cur->len = p-q;                         // ポインタの進み具合で文字数を把握する.
            continue;
        }

        // 変数があったとき
        if('a' <= *p && *p <= 'z'){
            char *q = p;
            while('a' <= *q && *q <= 'z')
                q++;
            int len = q - p;
            cur = new_token(TK_IDENT,cur,p,len);
            p = q;
            continue;
        }
        
        error_at(token->str,"トークナイズできません");
    }
    new_token(TK_EOF,cur,p,0);
    return head.next;
}
