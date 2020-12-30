#include "Language.h"

#define TOKEN tokens->data[tokens->ofs]
#define TOKEN_NEXT tokens->data[tokens->ofs + 1]

#define ASSERT_NULL(data)                                     \
    if (data == nullptr) {                                    \
        return nullptr;                                       \
    }

#define ASSERT_SYNTAX(data, pos, message)                     \
    if (data == nullptr) {                                    \
        return SyntaxError(tokens, pos, message);             \
    }

#define ASSERT_FALSE(data)                                    \
    if (data == false) {                                      \
        return nullptr;                                       \
    }

#define ASSERT_FALSE_SYNT(data, pos, message)                 \
    if (data == false) {                                      \
        return SyntaxError(tokens, pos, message);             \
    }

#define CHECK_OPERATOR(tokens, oper, message)                 \
    if ((TOKEN.type == TOKEN_OP && TOKEN.data.op != oper) ||  \
         TOKEN.type != TOKEN_OP) {                            \
        return nullptr;                                       \
    }                                                         \
    tokens->ofs++;

#define CHECK_TYPE(tokens, type_op)                           \
    if (TOKEN.type != type_op) {                              \
        return nullptr;                                       \
    }

int FileLength(FILE* fp) {
    assert(fp != nullptr);

    fseek(fp, 0, SEEK_END);
    int pos = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    return pos;
}

char* ReadFile(FILE* fp) {
    assert(fp != nullptr);

    int text_size = FileLength(fp);
    char* buffer = (char*) calloc(text_size, sizeof(char));
    assert(buffer != nullptr);

    int real_text_size = fread(buffer, sizeof(char), text_size, fp);
    buffer[real_text_size] = '\0';

    //printf("<<<\n%s\n>>>\n", buffer);

    return buffer;
}

void ClearBuffer(Input* input) {
    assert(input != nullptr);

    while (input->data[input->ofs] == ' ' || input->data[input->ofs] == '\n' || input->data[input->ofs] == '\t' || input->data[input->ofs] == '\0') {
        ++input->ofs;
    }

}

bool GetToken(Tokens* tokens, Input* input) {
    ClearBuffer(input);

    int pos = 0;
    char* token_name = (char*) calloc(MAX_TOKEN_LEN, sizeof(char));
    do {
        input->ofs++;
        token_name[pos++] = input->data[input->ofs - 1];
    } while (input->data[input->ofs - 1] != '\n' && input->data[input->ofs - 1] != '\t' && input->data[input->ofs - 1] != ' ');

    //printf("current: %s\n", token_name);

    if (pos > 1) {
        token_name[pos - 1] = '\0';
        input->ofs--;
    }
    else {
        token_name[pos] = '\0';
    }

    //printf("name: <%s>\n", token_name);

    tokens->data[tokens->count].name = token_name;

    for (int i = 0; i < NUM_OPERATORS; ++i) {
        if (strcmp(token_name, data_op[i]) == 0) {
            tokens->data[tokens->count].type = TOKEN_OP;
            tokens->data[tokens->count].data.op = i;
            ++tokens->count;
            return true;
        }
    }

    if (isdigit(token_name[0])) {
        tokens->data[tokens->count].type = TOKEN_NUM;
        tokens->data[tokens->count].data.num = strtod(token_name, nullptr);
        ++tokens->count;
        return true;
    }

    if (IsVar(token_name)) {
        tokens->data[tokens->count].type = TOKEN_ID;
        tokens->data[tokens->count].data.id = token_name;
        ++tokens->count;
        return true;
    }

    return false;
}

bool IsVar(char* token) {
    assert(token);

    int pos = 0;

    if (('a' <= token[pos] && 'z' >= token[pos]) ||
        ('A' <= token[pos] && 'Z' >= token[pos]) ||
        token[pos] == '_') {
        pos++;
    }
    else {
        return false;
    }

    while (('a' <= token[pos] && 'z' >= token[pos]) ||
           ('A' <= token[pos] && 'Z' >= token[pos]) ||
           ('0' <= token[pos] && '9' >= token[pos]) ||
           token[pos] == '_' || token[pos] == '.'   ||
           token[pos] == '!' || token[pos] == '&') {
        pos++;
    }

    if (token[pos] != '\0') {
        return false;
    }

    return true;
}

void DeleteComments(Input* input) {
    assert(input != nullptr);

    int i = 0;
    int j = 0;

    while (isalpha(input->data[i]) != '/0') {
        printf("%c", input->data[i]);
        if (input->data[i] == '/' && input->data[i + 1] == '/') {
            ++i;
            while (input->data[i] != '\n' && input->data[i] != '\0') {
                i++;
            }
        }
        if (input->data[i] != '\0') {
            input->data[j] = input->data[i];
            ++j;
        }
        ++i;
    }
    input->data[i] = '\0';
}

void SetParents(Node* node) {
    assert(node);

    if (node->left != nullptr) {
        node->left->parent = node;
        SetParents(node->left);
    }
    if (node->right != nullptr) {
        node->right->parent = node;
        SetParents(node->right);
    }

}

Tree* BuildTree(Tokens* tokens) {

    char* file_name = (char*) calloc(MAX_FILENAME_LEN, sizeof(char));
    assert(file_name != nullptr);

    #ifdef DEFAULT_INPUT
    FILE* fp = fopen(standart_input, "r");
    #else
    fprintf(stderr, "Please enter file name\n");
    scanf("%s", file_name);

    FILE* fp = fopen(file_name, "r");
    if (fp == nullptr) {
        fprintf(stderr, "Could find file with this name\n");
        return nullptr;
    }
    #endif

    Input input = {};
    input.ofs = 0;
    input.data = ReadFile(fp);
    input.len = strlen(input.data);

    ParseIntoToxens(tokens, &input);
    DumpTokens(tokens);

    Node* root = GetGram(tokens);
    TreeVisualDump(root, "Tree_visual_dump");

    Tree* tree = (Tree*) calloc(1, sizeof(Tree));
    tree->root = root;
    SetParents(tree->root);

    return tree;
}

void ConstructTokens(Tokens* toxens) {
    assert(toxens != nullptr);

    toxens->data = (Token*) calloc(MAX_TOKENS_COUNT, sizeof(Token));
    assert(toxens->data);

    toxens->ofs = 0;
    toxens->count = 0;
}

void DestructToxens(Tokens* toxens) {
    assert(toxens != nullptr);

    free(toxens->data);
    toxens->count = -1;
    toxens->ofs = -1;
}

void ParseIntoToxens(Tokens* tokens, Input* input) {
    assert(tokens != nullptr);
    assert(input != nullptr);

    while (input->ofs < input->len) {
        //printf ("ofs: %d len %d\n", input->ofs, input->len);
        if(!GetToken(tokens, input)) {
            fprintf(stderr, "Wrong token name <%s> pos %d\n", tokens->data[tokens->count].name, tokens->ofs);
        }
    }
}

void DumpTokens(Tokens* tokens) {
    assert(tokens != nullptr);

    fprintf(stderr, "tokens ofs: %d\ntokens count: %d\n", tokens->ofs, tokens->count);

    for (int i = 0; i < tokens->count; ++i) {
        if (tokens->data[i].type == TOKEN_NUM) {
            fprintf(stderr, "[%d] TYPE_NUM name: <%s> data: <%lg>\n", i, tokens->data[i].name, tokens->data[i].data.num);
        }
        else if (tokens->data[i].type == TOKEN_OP) {
            fprintf(stderr, "[%d] TYPE_OP name: <%s> real name: <%s> data: <%d>\n", i, tokens->data[i].name, reference_op[tokens->data[i].data.op], tokens->data[i].data.op);
        }
        else if (tokens->data[i].type == TOKEN_ID) {
            fprintf(stderr, "[%d] TYPE_ID name: <%s> data: <%s>\n", i, tokens->data[i].name, tokens->data[i].data.id);
        }
    }

}

int GetOpData(Tokens* tokens) {
    assert(tokens);
    assert(TOKEN.name);

    for (int i = 0; i < NUM_OP; ++i) {
        if (strcmp(TOKEN.name, math_op[i]) == 0) {
            return i;
        }
    }

    fprintf(stderr, "Could find math op <%s>\n", TOKEN.name);
    return -1;
}

Data AssignConst(double number) {
  Data data = {.num = number};
  return data;
};
Data AssignId(char* identificator) {
  Data data = {.id = identificator};
  return data;
};
Data AssignData(int op_type) {
  Data data = {.op = op_type};
  return data;
};

bool CheckBraceOpen(Tokens* tokens) {
    if (TOKEN.type == TOKEN_OP && TOKEN.data.op != BRACE_OPEN) {
        SyntaxError(tokens, 0, "Expected '{'\n");
        return false;
    }
    tokens->ofs++;
    return true;
}
bool CheckBraceClose(Tokens* tokens) {
    if (TOKEN.type == TOKEN_OP && TOKEN.data.op != BRACE_CLOSE) {
        SyntaxError(tokens, 0, "Expected '}'\n");
        return false;
    }
    tokens->ofs++;
    return true;
}
bool CheckRoundOpen(Tokens* tokens) {
    if (TOKEN.type == TOKEN_OP && TOKEN.data.op != ROUND_OPEN) {
        SyntaxError(tokens, 0, "Expected '('\n");
        return false;
    }
    tokens->ofs++;
    return true;
}
bool CheckRoundClose(Tokens* tokens) {
    if (TOKEN.type == TOKEN_OP && TOKEN.data.op != ROUND_CLOSE) {
        SyntaxError(tokens, 0, "Expected ')'\n");
        return false;
    }
    tokens->ofs++;
    return true;
}

void* SyntaxError(Tokens* tokens, int pos, char* message) {
    assert(tokens != nullptr);
    return nullptr;
}

Node* GetGram(Tokens* tokens) {
    assert(tokens);

    Node* root = GetProgram(tokens);

    if (tokens->ofs != tokens->count) {   //strcmp(TOKEN.name, "EOF") != 0
        DumpTokens(tokens);
        fprintf(stderr, "\nSyntax error in pos %d\n", tokens->ofs);
        return nullptr;
    }

    return root;
}

Node* GetProgram(Tokens* tokens) {
    assert(tokens);

    Node* root = NodeConstruct(TYPE_FICT, AssignId("fict"), nullptr, nullptr, GetDefi(tokens));

    Node* new_def = GetDefi(tokens);

    Node* fict_node = root;
    while (new_def != nullptr) {
        fict_node->left = NodeConstruct(TYPE_FICT, AssignId("fict"), nullptr, nullptr, new_def);
        fict_node = fict_node->left;
        new_def = GetDefi(tokens);
    }

    return root;
}

Node* GetDefi(Tokens* tokens) {
    assert(tokens);

    Node* def_node = GetFDec(tokens);
    ASSERT_NULL(def_node)

    def_node->left = GetComp(tokens);

    ASSERT_NULL(def_node->left);

    return def_node;
}

Node* GetFDec(Tokens* tokens) {
    assert(tokens);

    CHECK_OPERATOR(tokens, DECLARATOR, "Expected function declarator <double>\n")
    CHECK_TYPE(tokens, TOKEN_ID)

    char* func_name = TOKEN.data.id;
    tokens->ofs++;

    CHECK_OPERATOR(tokens, ROUND_OPEN, "Expected '('\n")

    if (TOKEN.type == TOKEN_OP && TOKEN.data.op == ROUND_CLOSE) {
        tokens->ofs++;
        return NodeConstruct(TYPE_VAR, AssignId(func_name), nullptr, nullptr, nullptr);
    }
    else {
        CHECK_TYPE(token, TOKEN_ID)
        char* var_name = TOKEN.data.id;
        tokens->ofs++;

        Node* root = NodeConstruct(TYPE_VAR, AssignId(func_name), nullptr, nullptr, nullptr);
        root->right = NodeConstruct(TYPE_VAR, AssignId(var_name), nullptr, nullptr, nullptr);
        Node* tmp_node = root->right;

        while(TOKEN.type == TOKEN_OP && TOKEN.data.op == COMMA) {
            tokens->ofs++;

            CHECK_TYPE(token, TOKEN_ID)
            var_name = TOKEN.data.id;
            tokens->ofs++;

            tmp_node->right = NodeConstruct(TYPE_VAR, AssignId(var_name), nullptr, nullptr, nullptr);
            tmp_node = tmp_node->right;
        }

        CHECK_OPERATOR(tokens, ROUND_CLOSE, "")

        return root;
    }

    return (Node*) SyntaxError(tokens, 0, "Expected ')' or list of variables\n");
}

Node* GetComp(Tokens* tokens) {
    assert(tokens);

    CHECK_OPERATOR(tokens, BRACE_OPEN, "Expected '{'\n")

    Node* root = NodeConstruct(TYPE_BRACE, AssignId("{"), nullptr, nullptr, nullptr);
    root->right = NodeConstruct(TYPE_STAT, AssignId(";"), nullptr, nullptr, nullptr);
    Node* node = root->right;
    node->left = GetStat(tokens);
    Node* tmp_stat = GetStat(tokens);

    while (tmp_stat != nullptr) {
        node->right = NodeConstruct(TYPE_STAT, AssignId(";"), nullptr, nullptr, nullptr);
        node = node->right;
        node->left = tmp_stat;
        tmp_stat = GetStat(tokens);
    }

    CHECK_OPERATOR(tokens, BRACE_CLOSE, "Expected '}'\n")

    return root;
}

Node* GetStat(Tokens* tokens) {
    assert(tokens);

    int ofs_old = tokens->ofs;

    if (TOKEN.type == TOKEN_OP && TOKEN.data.op == DECLARATOR) {
        Node* tmp = GetInit(tokens);
        CHECK_OPERATOR(tokens, SEMICOLON, "")
        return tmp;
    }

    if (TOKEN.type == TOKEN_ID && TOKEN_NEXT.type == TOKEN_OP && TOKEN_NEXT.data.op == ASSG) {
        Node* tmp = GetAssgn(tokens);
        CHECK_OPERATOR(tokens, SEMICOLON, "")
        return tmp;
    }

    if (TOKEN.type == TOKEN_OP && TOKEN.data.op == RET) {
        Node* tmp = GetRet(tokens);
        CHECK_OPERATOR(tokens, SEMICOLON, "")
        return tmp;
    }

    if (TOKEN.type == TOKEN_OP && TOKEN.data.op == IF) {
        return GetCond(tokens);
    }

    if (TOKEN.type == TOKEN_OP && TOKEN.data.op == LOOP) {
        return GetLoop(tokens);
    }

    Node* tmp = GetExpr(tokens);
    CHECK_OPERATOR(tokens, SEMICOLON, "")
    return tmp;
};

Node* GetExpr(Tokens* tokens) {
    assert(tokens);

    Node* root = nullptr;
    Node* node_tmp = GetSimp(tokens);
    ASSERT_NULL(node_tmp)

    if (TOKEN.type == TOKEN_OP && (TOKEN.data.op == 5 || (TOKEN.data.op >= 11 && TOKEN.data.op <= 15))) {
        int op = GetOpData(tokens);
        tokens->ofs++;
        Node* node_tmp2 = GetSimp(tokens);
        ASSERT_NULL(node_tmp2)
        root = NodeConstruct(TYPE_OP, AssignData(op), nullptr, node_tmp, node_tmp2);
        return root;
    }

    return node_tmp;
};

Node* GetInit(Tokens* tokens) {
    assert(tokens);

    Node* node = NodeConstruct(TYPE_DECL, AssignId("="), nullptr, nullptr, nullptr);

    CHECK_OPERATOR(tokens, DECLARATOR, "Expected decl\n")

    CHECK_TYPE(tokens, TOKEN_ID)

    char* var_name = TOKEN.data.id;
    tokens->ofs++;

    node->left = NodeConstruct(TYPE_VAR, AssignId(var_name), nullptr, nullptr, nullptr);

    CHECK_OPERATOR(tokens, ASSG, "Expected '='\n")

    node->right = GetExpr(tokens);

    ASSERT_NULL(node->right)

    return node;
};

Node* GetAssgn(Tokens* tokens) {
    assert(tokens);

    char* var_name = TOKEN.data.id;
    tokens->ofs++;
    Node* node_left = NodeConstruct(TYPE_VAR, AssignId(var_name), nullptr, nullptr, nullptr);

    Node* root = NodeConstruct(TYPE_ASSGN, AssignId("="), nullptr, node_left, nullptr);
    tokens->ofs++;

    root->right = GetExpr(tokens);
    ASSERT_NULL(root->right)

    return root;
};

Node* GetRet(Tokens* tokens) {
    assert(tokens);

    CHECK_OPERATOR(tokens, RET, "")

    Node* root = NodeConstruct(TYPE_RETURN, AssignId("return"), nullptr, nullptr, nullptr);
    root->right = GetExpr(tokens);

    return root;
};

Node* GetCond(Tokens* tokens) {
    assert(tokens);

    CHECK_OPERATOR(tokens, IF,"")
    CHECK_OPERATOR(tokens, ROUND_OPEN,"")

    Node* root = NodeConstruct(TYPE_IF, AssignId("if"), nullptr, nullptr, nullptr);
    root->left = GetExpr(tokens);

    ASSERT_NULL(root->left)

    CHECK_OPERATOR(tokens, ROUND_CLOSE,"")

    Node* tmp_node = GetComp(tokens);

    ASSERT_NULL(tmp_node)

    Node* if_else = NodeConstruct(TYPE_IF_ELSE, AssignId("if-else"), nullptr, nullptr, nullptr);
    root->right = if_else;
    if_else->left = tmp_node;

    if (TOKEN.type == TOKEN_OP && TOKEN.data.op == ELSE) {
        tokens->ofs++;
        if_else->right = GetComp(tokens);
        ASSERT_NULL(if_else->right);
    }

    return root;
};

Node* GetLoop(Tokens* tokens) {
    assert(tokens);

    Node* root = NodeConstruct(TYPE_WHILE, AssignId("while"), nullptr, nullptr, nullptr);
    Node* node = root;

    CHECK_OPERATOR(tokens, LOOP, "")
    CHECK_OPERATOR(tokens, ROUND_OPEN, "")

    node->left = GetExpr(tokens);
    ASSERT_NULL(node->left)

    CHECK_OPERATOR(tokens, ROUND_CLOSE, "")

    node->right = GetComp(tokens);
    ASSERT_NULL(node->right)

    return root;
};

Node* GetSimp(Tokens* tokens) {
    assert(tokens);

    Node* root = GetTerm(tokens);
    ASSERT_NULL(root)

    while (TOKEN.type == TOKEN_OP && (TOKEN.data.op == ADD || TOKEN.data.op == SUB)) {
        int op = GetOpData(tokens);
        ++tokens->ofs;
        Node* tmp_node = GetTerm(tokens);
        ASSERT_NULL(tmp_node)

        if (op == OP_ADD) {
            root = NodeConstruct(TYPE_OP, AssignData(OP_ADD), nullptr, root, tmp_node);
        }
        else if (op == OP_SUB) {
            root = NodeConstruct(TYPE_OP, AssignData(OP_SUB), nullptr, root, tmp_node);
        }
        else {
            printf("LOL WHAT THIS IS OPERATION ADD OR SUB\n");
        }
    }

    return root;
};

Node* GetTerm(Tokens* tokens) {
    assert(tokens);

    Node* root = GetPrim(tokens);
    ASSERT_NULL(root)

    while (TOKEN.type == TOKEN_OP && (TOKEN.data.op == MUL || TOKEN.data.op == DIV)) {
        int op = GetOpData(tokens);
        ++tokens->ofs;
        Node* tmp_node = GetPrim(tokens);
        ASSERT_NULL(tmp_node)

        if (op == OP_MUL) {
            root = NodeConstruct(TYPE_OP, AssignData(OP_MUL), nullptr, root, tmp_node);
        }
        else if (op == OP_DIV) {
            root = NodeConstruct(TYPE_OP, AssignData(OP_DIV), nullptr, root, tmp_node);
        }
        else {
            printf("LOL WHAT THIS IS OPERATION MUL OR DIV\n");
        }
    }

    return root;

};

Node* GetPrim(Tokens* tokens) {
    assert(tokens);

    Node* root = nullptr;

    if (TOKEN.type == TOKEN_OP && TOKEN.data.op == ROUND_OPEN) {
        tokens->ofs++;

        root = GetExpr(tokens);

        CHECK_OPERATOR(tokens, ROUND_CLOSE, "")
        return root;
    }

    if (TOKEN.type == TOKEN_NUM) {
        Node* root = NodeConstruct(TYPE_CONST, AssignConst(TOKEN.data.num), nullptr, nullptr, nullptr);
        tokens->ofs++;
        return root;
    }

    if (TOKEN.type == TOKEN_ID) {
        if (TOKEN_NEXT.type == TOKEN_OP && TOKEN_NEXT.data.op == ROUND_OPEN) {
            Node* tmp = GetCall(tokens);
            return tmp;
        }
        else {
            Node* root = NodeConstruct(TYPE_VAR, AssignId(TOKEN.data.id), nullptr, nullptr, nullptr);
            tokens->ofs++;
            return root;
        }
    }

    return (Node*) SyntaxError(tokens, 0, "END OF Primary\n");
}

Node* GetCall(Tokens* tokens) {
    assert(tokens);

    CHECK_TYPE(token, TOKEN_ID)
    char* func_name = TOKEN.data.id;
    tokens->ofs++;

    CHECK_OPERATOR(tokens, ROUND_OPEN, "Expected '('\n")
    Node* root = NodeConstruct(TYPE_CALL, AssignId("call"), nullptr, nullptr, nullptr);
    root->left = NodeConstruct(TYPE_VAR, AssignId(func_name), nullptr, nullptr, nullptr);

    if (TOKEN.type == TOKEN_OP && TOKEN.data.op == ROUND_CLOSE) {
        tokens->ofs++;
        return root;
    }
    else {
        Node* node_tmp = NodeConstruct(TYPE_ARG, AssignId("arg"), nullptr, nullptr, nullptr);
        root->right = node_tmp;
        node_tmp->left = GetExpr(tokens);
        ASSERT_NULL(node_tmp->left)

        while(TOKEN.type == TOKEN_OP && TOKEN.data.op == COMMA) {
            tokens->ofs++;

            node_tmp->right = NodeConstruct(TYPE_ARG, AssignId("arg"), nullptr, nullptr, nullptr);
            node_tmp = node_tmp->right;
            node_tmp->left = GetExpr(tokens);

            ASSERT_NULL(node_tmp->left)
        }

        CHECK_OPERATOR(tokens, ROUND_CLOSE, "")

        return root;
    }

    return (Node*) SyntaxError(tokens, 0, "Expected ')' or list of variables\n");
};
