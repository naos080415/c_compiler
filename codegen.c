#include "9cc.h"

Node *code[100];

// ローカル変数
LVar *locals;

char label_name[10];
int label_cnt[LV_END];      // ラベルの通し番号

void gen_lval(Node *node)
{
    if(node->kind != ND_LVAR)
        error("代入の左辺値が変数ではありません");

    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n",node->offset);
    printf("    push rax\n");
}

char *lavel_contorl(Label_keyword kind)
{
    int num = label_cnt[kind];
    sprintf(label_name,"%d",num);
    return label_name;
}

void gen(Node *node)
{
    if(!node) return;
    switch (node->kind){
        case ND_NUM:
            printf("#   ND_NUM\n");
            printf("    push %d\n",node->val);
            return;
        case ND_LVAR:   // 変数
            printf("#   ND_LVAR\n");
            gen_lval(node);
            printf("    pop rax\n");
            printf("    mov rax, [rax]\n");
            printf("    push rax\n");
            return;
        case ND_ASSIGN:     // =
            printf("#   ND_ASSIGN\n");
            gen_lval(node->lhs);
            gen(node->rhs);
            printf("    pop rdi\n");
            printf("    pop rax\n");
            printf("    mov [rax], rdi\n");
            printf("    push rdi\n");
            return;
        case ND_FUNC:       // 関数 
            if(node->lhs->lhs != NULL){
                gen(node->lhs->lhs);
                printf("    pop rdi\n");
            }
            if(node->lhs->rhs != NULL){
                gen(node->lhs->rhs);
                printf("    pop rsi\n");
            }
            if(node->rhs->lhs != NULL){
                gen(node->rhs->lhs);
                printf("    pop rdx\n");
            }
            if(node->rhs->rhs != NULL){
                gen(node->rhs->rhs);
                printf("    pop rcx\n");
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
            // if(node->lhs->lhs != NULL)
                gen(node->lhs->lhs);        // 初期条件
            printf(".Lbegin");
            printf("%s:\n",lavel_contorl(LV_FOR));
            gen(node->lhs->rhs);        // 継続条件
            printf("    pop rax\n");
            printf("    cmp rax, 0\n");
            printf("    je .Lend");
            printf("%s\n",lavel_contorl(LV_FOR));
            gen(node->rhs->rhs);         // 処理内容
            // if(node->rhs->lhs != NULL)
                gen(node->rhs->lhs);        // 増分
            printf("    jmp .Lbegin");
            printf("%s\n",lavel_contorl(LV_FOR));
            printf(".Lend");
            printf("%s:\n",lavel_contorl(LV_FOR));
            *(label_cnt+LV_FOR) = *(label_cnt+LV_FOR) + 1;
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

// func     = ident "(" ")" "{" stmt* "}"
Node *func()
{
    Node *node;
    Token *tok = consume_ident();
    if(tok){
        node = new_node(ND_BLOCK);
        node->block = calloc(100,sizeof(Node));
        expect("(");
        expect(")");
        expect("{");
        for(int i = 0;!consume("}");i++){
            node->block[i] = stmt();
        }
    }
    return node;
}

/* stmt       = expr ";" | "if" "(" expr ")" stmt ("else" stmt)?
    "while" "(" expr ")" stmt   | "for" "(" expr? ";" expr? ";" expr? ")" stmt 
|   | "{" stmt* "}" | return expr ";" */
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
    }else if(consume("{")){
        node = new_node(ND_BLOCK);
        node->block = calloc(100,sizeof(Node));
        for(int i = 0;!consume("}");i++){
            node->block[i] = stmt();
        }
        return node;
    }else{
        node = expr();
    }

    // 文の終わりの判断
    expect(";");
    return node;
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
        if(consume("+"))
            node = new_binary(ND_ADD,node,mul());
        else if(consume("-"))
            node = new_binary(ND_SUB,node,mul());
        else
            return node;
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

// unary      = ("+" | "-")? primary
Node *unary()
{
    if(consume("+"))
        return primary();
    if(consume("-"))
        return new_binary(ND_SUB,new_node_num(0),primary());
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
    if(tok){    // 数字でなければ
        if(consume("(")){
            // identのあとに ( があると関数とみなす.
            Node *left = calloc(1,sizeof(Node));
            Node *right = calloc(1,sizeof(Node));
            node = new_node(ND_FUNC);
            // 関数名をnodeにわたす.
            node->name_func = tok->str;

            for(int i=0;!consume(")");i++){
                if(i == 0)
                    left->lhs = expr();
                else if(i == 1)
                    left->rhs = expr();
                else if(i == 2)
                    right->lhs = expr();
                else if(i == 3)
                    right->rhs = expr();
                if(consume(")"))
                    break;
                expect(",");
            }
            // 新規作成したノードを連結する.
            node->lhs = left;
            node->rhs = right;
            // 関数の引数を渡す処理
            return node;
        }else{
            // identのあとに ( がない場合,変数とみなす.
            node = new_node(ND_LVAR);     
            LVar *lvar = find_lvar(tok);

            if(lvar){
                node->offset = lvar->offset;
            }else{
                lvar = calloc(1,sizeof(LVar));
                lvar->next = locals;
                lvar->name = tok->str;
                lvar->len = tok->len;
                if( locals == NULL )
                    lvar->offset = 8;
                else
                    lvar->offset = locals->offset + 8;
                node->offset = lvar->offset;
                locals = lvar;
            }
            return node;
        }
    }

    // そうでなければ数値のはず
    return new_node_num(expect_number());
}
