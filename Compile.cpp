#include "Language.h"
#include "Compile.h"

#define COMP_FP compile->fp
#define FUNCTION compile->table->functions[compile->func_no]

void MakeNamesTable(Tree* tree, NamesTable* table) {
    assert(tree);

    int func_count = GetFuncCount(tree);
    NamesTableConstruct(table);

    table->functions = (Function*) calloc(func_count, sizeof(Function));
    table->func_count = func_count;

    FillNamesTable(tree, table);

    Node* node = tree->root;

    for (int i = 0; i < table->func_count; ++i) {
        GetFunction(node->right, table->functions + i);
        node = node->left;
    }
}

int GetFuncCount(Tree* tree) {
    assert(tree);

    int count = 0;
    Node* node = tree->root;

    while (node != nullptr) {
        node = node->left;
        count++;
    }

    return count;
}

void NamesTableConstruct(NamesTable* table) {
    assert(table);

    table->func_count = 0;
    table->functions = nullptr;
}

void NamesTableDestruct(NamesTable* table) {
    assert(table);

    table->func_count = -1;
    free(table->functions);
}

void GetFunction(Node* node, Function* func) {
    assert(node);
    assert(func);

    func->func_name = node->data.id;

    func->arg_count = GetArgCount(node->right);
    func->var_count = GetVarCount(node->left->right);

    func->var_names = (char**) calloc(func->arg_count + func->var_count, sizeof(char*));

    GetVarNames(node, func->var_names);
}

int GetArgCount(Node* node) {
    int count = 0;

    while (node != nullptr) {
        node = node->right;
        ++count;
    }

    return count;
}

int GetVarCount(Node* node) {
    if (node == nullptr) {
        return 0;
    }

    if (node->type == TYPE_DECL) {
        return 1 + GetVarCount(node->left) + GetVarCount(node->right);
    }

    return GetVarCount(node->left) + GetVarCount(node->right);
}

void GetVarNames(Node* node, char** var_names) {
    assert(var_names);
    assert(node);

    Node* node_tmp = node->right;
    int ofs = 0;

    while (node_tmp != nullptr) {
        var_names[ofs] = node_tmp->data.id;
        node_tmp = node_tmp->right;
        ++ofs;
    }

    GetVarDecl(node->left->right, var_names, &ofs);
}

void GetVarDecl(Node* node, char** var_names, int* ofs) {
    assert(var_names);
    assert(ofs);

    if (node == nullptr) {
        return;
    }

    GetVarDecl(node->left, var_names, ofs);

    if (node->type == TYPE_DECL) {
        var_names[*ofs] = node->left->data.id;
        (*ofs)++;
        return;
    }

    GetVarDecl(node->right, var_names, ofs);
}

int GetFunctionId(NamesTable* table, char* func_name) {
    assert(table);
    assert(func_name);

    for (int i = 0; i < table->func_count; ++i) {
        if (strcmp(table->functions[i].func_name, func_name) == 0) {
            return i;
        }
    }

    return -1;
}

void DumpTable(NamesTable* table) {
    assert(table);

    printf("\nfunc count: %d\n", table->func_count);

    for (int i = 0; i < table->func_count; ++i) {
        printf("name: <%s>\n", table->functions[i].func_name);
        printf("var_count: %d, arg_count: %d\n", table->functions[i].var_count, table->functions[i].arg_count);

        printf("vars: ");
        for (int j = 0; j < table->functions[i].arg_count + table->functions[i].var_count; ++j) {
            printf("<%s> ", table->functions[i].var_names[j]);
        }
        printf("\n");
    }
}

void ConstructCompile(CompileInfo* compile, NamesTable* table) {
    assert(compile);
    assert(table);

    compile->table = table;
    compile->func_no = 0;
    compile->labels_count = 0;

    #ifdef DEFAULT_OUTPUT
    compile->fp = fopen(standart_output, "w");
    #else
    fprintf(stderr, "Enter file name\n");
    char* file_name = (char*) calloc(30, sizeof(char));
    scanf("%s", file_name);

    compile->fp = fopen(file_name, "w");
    if (compile->fp == nullptr) {
        fprintf(stderr, "File is not found\n");
        return;
    }
    #endif
}

void DestructCompile(CompileInfo* compile) {
    assert(compile);

    NamesTableDestruct(compile->table);
    compile->func_no = -1;
    compile->labels_count = -1;
    fclose(compile->fp);
}

void MakeCompilation(Tree* tree) {
    assert(tree);

    NamesTable table = {};
    NamesTableConstruct(&table);
    MakeNamesTable(tree, &table);

    CompileInfo compile = {};
    ConstructCompile(&compile, &table);

    CompileFunctions(&compile, tree->root);

    DestructCompile(&compile);

    fprintf(stderr, "Done compilation!\n");
}

void CompileFunctions(CompileInfo* compile, Node* node) {
    assert(compile);
    assert(node);

    fprintf(COMP_FP, "push 0\n"
                     "pop rax\n"
                     "push 0\n"
                     "pop rbx\n"
                     "push 0\n"
                     "pop [0]\n"
                     "call main\n"
                     "hlt\n");

    while (node != nullptr) {
        CompileFuction(compile, node->right);
        node = node->left;
        compile->func_no++;
    }

    printf("Done functions compilation\n");
}

void CompileFuction(CompileInfo* compile, Node* node) {
    assert(compile);
    assert(node);

    fprintf(COMP_FP, "          \n"
                     "%s:       \n"
                     "push rax  \n"
                     "pop  rbx  \n"
                     "push rax  \n"
                     "push rax  \n"
                     "push %d   \n"
                     "add       \n"
                     "pop rax   \n"
                     "pop [rax] \n", node->data.id, FUNCTION.arg_count + FUNCTION.var_count + 1);

    if (node->right != nullptr) {
        CompileArguments(compile, node->right);
    }

    CompileCompound(compile, node->left);
}

void CompileArgumets(CompileInfo* compile, Node* node) {
    assert(compile);
    assert(node);

    for (int i = 0; i < FUNCTION.arg_count; ++i) {
        fprintf(COMP_FP, "pop [rbx + %d]\n", GetVarIndex(&(FUNCTION), node->data.id));
        node = node->right;
    }
}

void CompileCompound(CompileInfo* compile, Node* node) {
    assert(compile);
    assert(node);

    while (node->right != nullptr) {
        node = node->right;
        if (node->left != nullptr) {
            CompileStatement(compile, node->left);
        }
    }
}

void CompileStatement(CompileInfo* compile, Node* node) {
    assert(compile);
    assert(node);

    switch (node->type) {
        case TYPE_DECL:
            CompileDeclaration(compile, node);
            break;

        case TYPE_ASSGN:
            CompileAssignment(compile, node);
            break;

        case TYPE_RETURN:
            CompileReturn(compile, node);
            break;

        case TYPE_IF:
            CompileCondition(compile, node);
            break;

        case TYPE_WHILE:
            CompileLoop(compile, node);
            break;

        case TYPE_CALL:
            CompileCall(compile, node);
            break;

        default :
            printf("Unknown node type %d", node->type);
            break;

    }
}

void CompileDeclaration(CompileInfo* compile, Node* node) {
    assert(compile);
    assert(node);

    CompileExpression(compile, node->right);
    fprintf(COMP_FP, "pop [rbx + %d]\n", GetVarIndex(&(FUNCTION), node->left->data.id));
}

void CompileAssignment(CompileInfo* compile, Node* node) {
    assert(compile);
    assert(node);

    CompileExpression(compile, node->right);
    fprintf(COMP_FP, "pop [rbx + %d]\n", GetVarIndex(&(FUNCTION), node->left->data.id));
}

void CompileReturn(CompileInfo* compile, Node* node) {
    assert(compile);
    assert(node);

    if (node->right != nullptr) {
        CompileExpression(compile, node->right);
    }

    fprintf(COMP_FP, "push [rax]  \n"
                     "pop rax     \n"
                     "push [rax]  \n"
                     "pop rbx     \n"
                     "ret         \n");

}

void CompileCondition(CompileInfo* compile, Node* node) {
    assert(compile);
    assert(node);

    CompileExpression(compile, node->left);
    int current_label = compile->labels_count++;

    fprintf(COMP_FP, "push 1       \n"
                     "je  true_%d  \n"
                     "jmp false_%d \n"
                     "true_%d:     \n", current_label, current_label, current_label);
    CompileCompound(compile, node->right->left);

    fprintf(COMP_FP, "jmp end_cond_%d  \n"
                     "false_%d:        \n", current_label, current_label);

    if (node->right->right != nullptr) {
        CompileCompound(compile, node->right->right);
    }

    fprintf(COMP_FP, "end_cond_%d:     \n", current_label);
}

void CompileLoop(CompileInfo* compile, Node* node) {
    assert(compile);
    assert(node);

    int current_label = compile->labels_count++;
    fprintf(COMP_FP, "while_%d:  \n", current_label);

    CompileExpression(compile, node->left);

    fprintf(COMP_FP, "push 0          \n"
                     "je end_while_%d \n", current_label);

    CompileCompound(compile, node->right);

    fprintf(COMP_FP, "jmp while_%d   \n"
                     "end_while_%d:  \n", current_label, current_label);
};

void CompileCall(CompileInfo* compile, Node* node) {
    assert(compile);
    assert(node);


    Node* tmp_node = node;
    while (tmp_node->right != nullptr) {
        tmp_node = tmp_node->right;
    }

    int standart_index = GetStandartIndex(node->left->data.id);
    if (standart_index != -1) {
        CompileStandartFunc(compile, node, standart_index);
        return;
    }

    int func_id = GetFunctionId(compile->table, node->left->data.id);

    if (func_id == -1) {
        fprintf(stderr, "No such function found <%s>\n", node->left->data.id);
        return;
    }

    for (int i = 0; i < compile->table->functions[func_id].arg_count; ++i) {
        CompileExpression(compile, tmp_node->left);
        tmp_node = tmp_node->parent;
    }

    fprintf(COMP_FP, "call %s\n", compile->table->functions[func_id].func_name);

}

void CompileExpression(CompileInfo* compile, Node* node) {
    assert(node);
    assert(compile);

    if (node->type == TYPE_OP && (node->data.op >= OP_EQUAL && node->data.op <= OP_GREATER_EQUAL)) {
        CompileSimpExpression(compile, node->left);
        CompileSimpExpression(compile, node->right);
        MakeCompare(compile, node);
    }
    else {
        CompileSimpExpression(compile, node);
    }
}

void MakeCompare(CompileInfo* compile, Node* node) {
    assert(compile);
    assert(node);

    switch (node->data.op) {
        case OP_EQUAL:
            fprintf(COMP_FP, "je ");
            break;

        case OP_NOT_EQUAL:
            fprintf(COMP_FP, "jne ");
            break;

        case OP_GREATER:
            fprintf(COMP_FP, "ja ");
            break;

        case OP_LESS:
            fprintf(COMP_FP, "jb ");
            break;

        case OP_GREATER_EQUAL:
            fprintf(COMP_FP, "jae ");
            break;

        case OP_LESS_EQUAL:
            fprintf(COMP_FP, "jbe ");
            break;

        default:
            fprintf(stderr, "Unknown op type: %d", node->data.op);
            break;
    }

    int current_label = compile->labels_count++;
    fprintf(COMP_FP, "true_%d           \n"
                     "push 0            \n"
                     "jmp end_comp_%d   \n"
                     "true_%d:          \n"
                     "push 1            \n"
                     "jmp end_comp_%d   \n"
                     "end_comp_%d:      \n", current_label, current_label, current_label, current_label, current_label);

}

void CompileSimpExpression(CompileInfo* compile, Node* node) {
    assert(compile);
    assert(node);

    if (node->type == TYPE_OP) {
            CompileSimpExpression(compile, node->left);
            CompileSimpExpression(compile, node->right);

        switch (node->data.op) {
            case OP_ADD:
                fprintf(COMP_FP, "add\n");
                break;

            case OP_SUB:
                fprintf(COMP_FP, "sub\n");
                break;

            case OP_MUL:
                fprintf(COMP_FP, "mul\n");
                break;

            case OP_DIV:
                fprintf(COMP_FP, "div\n");
                break;

            default:
                printf("Unknown math type %d", node->data.op);
        }

    }
    else {
        CompilePrimExpression(compile, node);
    }
}

void CompilePrimExpression(CompileInfo* compile, Node* node) {
    assert(compile);
    assert(node);

    switch (node->type) {
        case TYPE_CONST:
            fprintf(COMP_FP, "push %lg\n", node->data.num);
            break;

        case TYPE_VAR:
            fprintf(COMP_FP, "push [rbx + %d]\n", GetVarIndex(&(FUNCTION), node->data.id));
            break;

        case TYPE_CALL:
            CompileCall(compile, node);
            break;

        default:
            printf("Unknown node type %d", node->type);
    }
}

void CompileStandartFunc(CompileInfo* compile, Node* node, int standart_index) {
    assert(compile);

    if (standart_index == IDX_IN) {
        fprintf(COMP_FP, "in              \n"
                         "pop [rbx + %d]  \n", GetVarIndex(&(FUNCTION), node->right->left->data.id));
    }
    else {
        if (node->right != nullptr) {
            CompileExpression(compile, node->right->left);
        }
        fprintf(COMP_FP, "%s\n", standart_func_asm[standart_index]);
    }

}

int GetStandartIndex(char* func_name) {
    assert(func_name);

    for (int i = 0; i < IDX_NUM; ++i) {
        if (strcmp(standart_func_comp[i], func_name) == 0) {
            return i;
        }
    }

    return -1;
}

void CompileArguments(CompileInfo* compile, Node* node) {
    assert(compile);
    assert(node);

    for (int i = 0; i < FUNCTION.arg_count; ++i) {
        int pos = GetVarIndex(&(FUNCTION), node->data.id);
        if (pos == -1) {
            fprintf(stderr, "Cant find Argument with name <%s>", node->data.id);
        }
        fprintf(COMP_FP, "pop [rbx + %d]\n", pos);
        node = node->right;
    }
}

int GetVarIndex(Function* func, char* id) {
    assert(func);
    assert(id);

    for (int i = 0; i < func->arg_count + func->var_count; ++i) {
        if (strcmp(func->var_names[i], id) == 0) {
            return i + 1;
        }
    }

    fprintf(stderr, "Couldn't find var with name <%s>", id);
    return -1;
}
