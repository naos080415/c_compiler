#include "9cc.h"

Node *code[100];

// ローカル変数
LVar *locals;

char label_name[10];
int label_cnt[LV_END];      // ラベルの通し番号

void gen_lval(Node *node)
{
    if(node->kind == ND_LVAR){
        printf("    mov rax, rbp\n");
        printf("    sub rax, %d\n",node->offset);
        printf("    push rax\n");
    }else if(node->kind == ND_DEREF){
        gen(node->lhs);
        return;
    }else{
        error("代入の左辺値が変数ではありません");
    }
}

char *lavel_contorl(Label_keyword kind)
{
    int num = label_cnt[kind];
    sprintf(label_name,"%d",num);
    return label_name;
}


Vtype *get_type(Node *node) {
    while(node != NULL){
        node = node->lhs;
        if(node->type != NULL)
            return node->type;
    }
    return NULL;
}

void gen(Node *node)
{
    char *x86_ABI[6] = {"rdi","rsi","rdx","rcx","r8","r9"};
    if(!node) return;
    switch (node->kind){
        case ND_NUM:
            printf("#   ND_NUM\n");
            printf("    push %d\n",node->val);
            return;
        case ND_LVAR:   // 変数(int)
            printf("#   ND_LVAR\n");
            gen_lval(node);   
            printf("    pop rax\n");
            printf("    mov rax, [rax]\n");
            printf("    push rax\n");
            return;
        case ND_ASSIGN:     // =
            printf("#   ND_ASSIGN\n");
            Vtype *t = get_type(node);
            
            if( t && t->kind == ARRAY ){
                gen_lval(node->lhs);
                gen(node->rhs);
                printf("    pop rdi\n");
                printf("    pop rax\n");
                printf("    mov rax, rdi\n");
                printf("    push rdi\n");
                return;
            }else{
                gen_lval(node->lhs);
                gen(node->rhs);
                printf("    pop rdi\n");
                printf("    pop rax\n");
                printf("    mov [rax], rdi\n");
                printf("    push rdi\n");
                return;
            }
            return;
        case ND_ADDR:       // &
            printf("#   ND_ADDR\n");
            gen_lval(node->lhs);
            return;
        case ND_DEREF:      // *
            printf("#   ND_DEREF\n");
            gen(node->lhs);
            printf("    pop rax\n");
            printf("    mov rax, [rax]\n");
            printf("    push rax\n");
            return;
        case ND_FUNC:       // 関数 
            for(int i = 0;node->args[i];i++){
                gen(node->args[i]);
                printf("    pop %s\n",x86_ABI[i]);
            }
            printf("    call ");
            char *p = node->name_func;
            char q;
            for(;;){
                q = *p;
                if(q == '(')
                    break;
                printf("%c",q);
                p++;
            }
            printf("\n");
            return;
        case ND_IF:         // if
            printf("#   ND_IF\n");

            gen(node->lhs);
            printf("    pop rax\n");
            printf("    cmp rax, 0\n");
            
            if( node->rhs->kind == ND_ELSE ){
                printf("#   ND_ELSE\n");
                printf("    je .Lelse");
                printf("%s\n",lavel_contorl(LV_IF));
            }else{         
                printf("    je .Lend");            
                printf("%s\n",lavel_contorl(LV_IF));
            }

            if( node->rhs->kind == ND_ELSE ){
                gen(node->rhs->lhs);
                printf("    jmp .Lend");
                printf("%s\n",lavel_contorl(LV_IF));

                printf(".Lelse");
                printf("%s:\n",lavel_contorl(LV_IF));

                gen(node->rhs->rhs);
                printf(".Lend");
                printf("%s:\n",lavel_contorl(LV_IF));

            }else{
                gen(node->rhs);
                printf(".Lend");
                printf("%s:\n",lavel_contorl(LV_IF));
            }
            *(label_cnt+LV_IF) = *(label_cnt+LV_IF) + 1;
            return;
        case ND_WHILE:
            printf("#   ND_WHILE\n");
            printf(".Lbegin");
            printf("%s:\n",lavel_contorl(LV_WHILE));
            gen(node->lhs);
            printf("    pop rax\n");
            printf("    cmp rax, 0\n");
            printf("    je .Lend");
            printf("%s\n",lavel_contorl(LV_WHILE));
            gen(node->rhs);
            printf("    jmp .Lbegin");
            printf("%s\n",lavel_contorl(LV_WHILE));
            printf(".Lend");
            printf("%s:\n",lavel_contorl(LV_WHILE));
            *(label_cnt+LV_FOR) = *(label_cnt+LV_FOR) + 1;
            return;
        case ND_FOR:    // for
            printf("#   ND_FOR\n");
            gen(node->lhs->lhs);        // 初期条件
            printf(".Lbegin");
            printf("%s:\n",lavel_contorl(LV_FOR));
            gen(node->lhs->rhs);        // 継続条件
            printf("    pop rax\n");
            printf("    cmp rax, 0\n");
            printf("    je .Lend");
            printf("%s\n",lavel_contorl(LV_FOR));
            gen(node->rhs->rhs);         // 処理内容
            gen(node->rhs->lhs);        // 増分
            printf("    jmp .Lbegin");
            printf("%s\n",lavel_contorl(LV_FOR));
            printf(".Lend");
            printf("%s:\n",lavel_contorl(LV_FOR));
            *(label_cnt+LV_FOR) = *(label_cnt+LV_FOR) + 1;
            return;
        case ND_FUNC_DEF:
            printf("#   ND_FUNC_DEF\n");
            for(int i = 0;node->block[i];i++){
                gen(node->block[i]);
                printf("    pop rax\n");
            }
            printf("#   ND_FUNC_DEF_END\n");            
            return;
        case ND_BLOCK:      // { }
            printf("#   NL_BLOCK\n");
            for(int i = 0;node->block[i];i++){
                gen(node->block[i]);
                printf("    pop rax\n");
            }
            printf("#   NL_BLOCK_END\n");
            return;
        case ND_RETURN:     // return
            printf("#   ND_RETURN\n");
            gen(node->lhs);
            printf("    pop rax\n");
            printf("    mov rsp, rbp\n");
            printf("    pop rbp\n");
            printf("    ret\n");
            return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch(node->kind) {
        case ND_ADD:
            printf("    add rax, rdi\n");
            break;
        case ND_SUB:
            printf("    sub rax, rdi\n");
            break;
        case ND_MUL:
            printf("    imul rax, rdi\n");
            break;
        case ND_DIV:
            printf("    cqo\n");
            printf("    idiv rdi\n");
            break;
        case ND_EQ:
            printf("    cmp rax, rdi\n");
            printf("    sete al\n");
            printf("    movzb rax, al\n");
            break;
        case ND_NE:
            printf("    cmp rax, rdi\n");
            printf("    setne al\n");
            printf("    movzb rax, al\n");
            break;
        case ND_LT:
            printf("    cmp rax, rdi\n");
            printf("    setl al\n");
            printf("    movzb rax, al\n");
            break;
        case ND_LE:
            printf("    cmp rax, rdi\n");
            printf("    setle al\n");
            printf("    movzb rax, al\n");
            break;
    }
    printf("    push rax\n");
}
 
 
Node *new_node(NodeKind kind)
{
    Node *node = calloc(1,sizeof(Node));
    node->kind = kind;
    return node;
}

Node *new_binary(NodeKind kind,Node *lhs,Node *rhs)
{
    Node *node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val)
{
    Node *node = new_node(ND_NUM);
    node->val = val;
    return node;
}

// program    = func*
void program()
{
     int i = 0;
     while(!at_eof())
        code[i++] = func();
    code[i] = NULL;
}

// func     = "int"  ident "(" ")" "{" stmt* "}"
Node *func()
{
    Node *node;
    if(consume_keyword("int")){
        Token *tok = consume_ident();
        if(tok){
            node = new_node(ND_FUNC_DEF);
            node->block = calloc(100,sizeof(Node));
            node->args = calloc(6,sizeof(Node));
            expect("(");
            for(int i=0;i<6 && !consume(")");i++){
                if(consume_keyword("int")){
                    node->args[i] = variable_def();
                    if(!consume(")"))
                        expect(",");
                    else
                        break;
                }
            }
            expect("{");
            for(int i = 0;!consume("}");i++){       
                node->block[i] = stmt();
            }
        }
        return node;
    }else{
        error("関数の型が定義されていません.\n");
    }
}

/* stmt       =  expr ";" 
                | "if" "(" expr ")" stmt ("else" stmt)?
                | "while" "(" expr ")" stmt   
                | "for" "(" expr? ";" expr? ";" expr? ")" stmt 
                | return expr ";"
                | "{" stmt* "}"
                | "int"  variable_def ";"
                | return expr ";" */
Node *stmt()
{
    Node *node;
    if(consume_keyword("if")){
        node = new_node(ND_IF);
        expect("(");
        node->lhs = expr();
        expect(")");
        node->rhs = stmt();
        if(consume_keyword("else")){
            Node *els = new_node(ND_ELSE);
            /* elsを作成し,nodeにつなげる. */
            els->lhs = node->rhs;
            els->rhs = stmt();
            node->rhs = els;
        }
        return node;
    }else if(consume_keyword("while")){
        node = new_node(ND_WHILE);
        expect("(");
        node->lhs = expr();
        expect(")");
        node->rhs = stmt();
        return node;
    }else if(consume_keyword("for")){
        node = new_node(ND_FOR);
        Node *left = calloc(1,sizeof(Node));
        Node *right = calloc(1,sizeof(Node));
        expect("(");
        if(!consume(";")){
            left->lhs = expr();     // 初期条件
            expect(";");
        }
        if(!consume(";")){
            left->rhs = expr();     // 継続条件
            expect(";");
        }
        if(!consume(")")){
            right->lhs = expr();    // 増分
            expect(")");
        }
        right->rhs = stmt();    // 処理内容
        // ノードにつなげる.
        node->lhs = left;
        node->rhs = right;
        return node;
    }else if(consume_keyword("return")){
        node = new_node(ND_RETURN);
        node->lhs = expr();
        expect(";");
        return node;
    }else if(consume("{")){
        node = new_node(ND_BLOCK);
        node->block = calloc(100,sizeof(Node));
        for(int i = 0;!consume("}");i++){
            node->block[i] = stmt();
        }
        return node;
    }else if(consume_keyword("int")){
        node = calloc(1,sizeof(Node));
        node = variable_def();
        expect(";");
        return node;
    }else{
        node = expr();
        expect(";");
        return node;
    }
}

// expr       = assign
Node *expr()
{
    return assign();
}

// assign     = equality ("=" assign)?
Node *assign()
{
    Node *node = equality();
    if(consume("="))
        node = new_binary(ND_ASSIGN,node,assign());
    return node;  
}

// equality   = relational ("==" relational | "!=" relational)*
Node *equality()
{
    Node *node = relational(); 
    
    for(;;){
        if(consume("=="))
            node = new_binary(ND_EQ,node,relational());
        else if(consume("!="))
            node = new_binary(ND_NE,node,relational());
        else
            return node;
    }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational()
{
    Node *node = add();
    
    for(;;){
        if(consume("<"))
            node = new_binary(ND_LT,node,add());
        else if(consume("<="))
            node = new_binary(ND_LE,node,add());
        else if(consume(">"))
            node = new_binary(ND_LT,add(),node);
        else if(consume(">="))
            node = new_binary(ND_LE,add(),node);
        else
            return node;
    }
}

// add        = mul ("+" mul | "-" mul)*
Node *add()
{
    Node *node = mul();
    for(;;){
        if(consume("+")){
            Node *right = mul();
            if(node->type && node->type->kind == PTR){     // ポインタ変数だった場合
                int n = node->type->ptr_to->kind == INT ? 4 : 8;
                right = new_binary(ND_MUL,right,new_node_num(n));
            }
            node = new_binary(ND_ADD,node,right);
        }else if(consume("-")){
            Node *right = mul();
            if(node->type && node->type->kind == PTR){     // ポインタ変数だった場合
                int n = node->type->ptr_to->kind == INT ? 4 : 8;
                right = new_binary(ND_MUL,right,new_node_num(n));
            }
            node = new_binary(ND_SUB,node,right);
        }else{
            return node;
        }
    }
}

// mul        = unary ("*" unary | "/" unary)*
Node *mul()
{
    Node *node = unary();

    for(;;){
        if(consume("*"))
            node = new_binary(ND_MUL,node,unary());
        else if(consume("/"))
            node = new_binary(ND_DIV,node,unary());
        else
            return node;
    }
}

/* unary      = |("+" | "-")? primary 
                | ("*" | "&") unary
                | "sizeof" "(" unary ")" */
Node *unary()
{
    if(consume("+"))
        return primary();
    else if(consume("-"))
        return new_binary(ND_SUB,new_node_num(0),primary());
    else if(consume("*")){
        Node *node = new_node(ND_DEREF);
        node->lhs = unary();
        return node;
    }else if(consume("&")){
        Node *node = new_node(ND_ADDR);
        node->lhs = unary();
        return node;
    }else if(consume_keyword("sizeof")){
        expect("(");
        Node *buf = unary();
        int size;
        if(buf->kind != ND_NUM)
            size = buf->type->kind == PTR ? 8 : 4;
        else
            size = 4;
        expect(")");
        return new_node_num(size);
    }else
        return primary();
}


// pimary    = num | ident "(" stmt* ")"?  |  "(" expr ")"
Node *primary()
{
    Node *node;
    
    if(consume("(")){
        // 次のトークンが"("なら,"(" expr ")"のはず 
        node = expr();
        expect(")");
        return node;
    }
    
    Token *tok = consume_ident();
    if(tok){
        if(consume("(")){    // identのあとに ( があると関数とみなす.
            node = function_def(tok);
        }else{    // identのあとに ( がない場合,変数とみなす.
            node = variable(tok);
        }
        return node;
    }
    return new_node_num(expect_number());
}

Node *function_def(Token *tok)
{
    Node *node = new_node(ND_FUNC);
    node->args = calloc(6,sizeof(Node));
    // 関数名をnodeにわたす.
    node->name_func = tok->str;

    for(int i=0;i<6 && !consume(")");i++){
        node->args[i] = expr();
        if(!consume(")"))
            expect(",");
        else
            break;
    }
    return node;
}

Node *variable(Token *tok)
{
    Node *node = new_node(ND_LVAR);     
    LVar *lvar = find_lvar(tok);
    if(consume("[")){
        int size = expect_number();
        node->type = lvar->type;
        node->offset = lvar->offset - (size * 8);
        expect("]");
    }else if(lvar){
        node->type = lvar->type;
        node->offset = lvar->offset;
    }else{
        error("定義されていない変数です\n");
    }
    return node;
}

// variable_def     =  "*"? variable
Node *variable_def()
{
    Vtype *type = calloc(1,sizeof(Vtype));
    type->kind = INT;
    type->ptr_to = NULL;
    while(consume("*")){
        Vtype *p = calloc(1,sizeof(Vtype));
        p->kind = PTR;
        p->ptr_to = type;
        type = p;
    }

    Token *tok = consume_ident();
    if(tok){
        Node *node = new_node(ND_LVAR);
        LVar *lvar = find_lvar(tok);
        lvar = calloc(1,sizeof(LVar));
        lvar->name = calloc(32,sizeof(char));       // 確かCの変数名は32文字までやった気がする
        lvar->next = locals;
        memcpy(lvar->name,tok->str,tok->len);
        lvar->len = tok->len;

        int size = 1;
        if(consume("[")){
            type->kind = ARRAY;
            size = expect_number();
            expect("]");
        }
    
        if( locals == NULL )
            lvar->offset = 8 * size;
        else
            lvar->offset = locals->offset + (8 * size);

        lvar->type = type;
        node->type = type;
        node->offset = lvar->offset;
        locals = lvar;
        
        return node;
    }
}
