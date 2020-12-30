#pragma once

#define DEFAULT_OUTPUT
const char* standart_output = "asm_code.txt";

typedef struct Function {
    char* func_name;
    char** var_names;
    int arg_count;
    int var_count;
} Function;

typedef struct NamesTable {
    Function* functions;
    int func_count;
} NamesTable;

typedef struct CompileInfo {
    NamesTable* table;
    FILE* fp;
    int labels_count;
    int func_no;
} CompileInfo;

const char* standart_func_comp[] {
    "B.1.2",        //"printf
    "B.1.3",        //"scanf"
    "B.4_2",        //"cos"
    "B.4_1",        //"sin"
    "B.4_3",        //"sqrt"
    "K&R!K&R!K&R!"  //K&R!
};

const char* standart_func_asm[] {
    "out",
    "in",
    "cos",
    "sin",
    "sqrt",
    "kern"
};

enum STANDART_INDEX {
    IDX_OUT,
    IDX_IN,
    IDX_COS,
    IDX_SIN,
    IDX_SQRT,
    IND_KERN,

    IDX_NUM
};

void Assemble(Tree* tree);
void WriteAsmCode(Tree* tree, NamesTable* table, FILE* file);
void MakeNamesTable(Tree* tree, NamesTable* table);
int GetFuncCount(Tree* tree);
void NamesTableConstruct(NamesTable* table);
void FillNamesTable(Tree* tree, NamesTable* table);
void GetFunction(Node* node, Function* func);
int GetArgCount(Node* node);
int GetVarCount(Node* node);
void GetVarNames(Node* node, char** var_names);
void GetVarDecl(Node* node, char** var_names, int* ofs);
void DumpTable(NamesTable* table);
void MakeNamesTable(Tree* tree, NamesTable* table);
int GetFuncCount(Tree* tree);
void NamesTableConstruct(NamesTable* table);
void FillNamesTable(Tree* tree, NamesTable* table);
void GetFunction(Node* node, Function* func);
int GetArgCount(Node* node);
int GetVarCount(Node* node);
void GetVarNames(Node* node, char** var_names);
void GetVarDecl(Node* node, char** var_names, int* ofs);
int GetFunctionId(NamesTable* table, char* func_name);
void DumpTable(NamesTable* table);
void ConstructCompile(CompileInfo* compile, NamesTable* table);
void DestructCompile(CompileInfo* compile);
void MakeCompilation(Tree* tree);
void CompileFunctions(CompileInfo* compile, Node* node);
void CompileFuction(CompileInfo* compile, Node* node);
void CompileArgumets(CompileInfo* compile, Node* node);
void CompileCompound(CompileInfo* compile, Node* node);
void CompileStatement(CompileInfo* compile, Node* node);
void CompileDeclaration(CompileInfo* compile, Node* node);
void CompileAssignment(CompileInfo* compile, Node* node);
void CompileReturn(CompileInfo* compile, Node* node);
void CompileCondition(CompileInfo* compile, Node* node);
void CompileLoop(CompileInfo* compile, Node* node);
void CompileCall(CompileInfo* compile, Node* node);
void CompileExpression(CompileInfo* compile, Node* node);
void MakeCompare(CompileInfo* compile, Node* node);
void CompileSimpExpression(CompileInfo* compile, Node* node);
void CompilePrimExpression(CompileInfo* compile, Node* node);
void CompileStandartFunc(CompileInfo* compile, Node* node, int standart_index);
int GetStandartIndex(char* func_name);
void CompileArguments(CompileInfo* compile, Node* node);
int GetVarIndex(Function* func, char* id);
