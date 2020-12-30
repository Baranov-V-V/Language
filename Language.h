#pragma once

#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <locale.h>
#include <sys\stat.h>
#include <ctype.h>

const char* standart_input = "input.txt";
const int MAX_FILENAME_LEN = 128;
const int MAX_VAR_LEN = 32;
const int MAX_TOKENS_COUNT = 9026;
const int MAX_TOKEN_LEN = 32;

//#define DEFAULT_INPUT

enum TOKENS_TYPE {
    TOKEN_NUM,
    TOKEN_ID,
    TOKEN_OP
};

typedef union Data {
    char* id;
    double num;
    int op;
} Data;

typedef struct Token {
    Data data;
    TOKENS_TYPE type;
    char* name;
} Token;

typedef struct Tokens {
    Token* data;
    int ofs;
    int count;
} Tokens;

typedef struct Input {
    char* data;
    int ofs;
    int len;
} Input;

enum TYPE {
    TYPE_FICT,
    TYPE_DECL,
    TYPE_VAR,
    TYPE_ARG,

    TYPE_BRACE,
    TYPE_STAT,

    TYPE_IF,
    TYPE_IF_ELSE,
    TYPE_WHILE,
    TYPE_ASSGN,

    TYPE_CALL,
    TYPE_RETURN,

    TYPE_OP,
    TYPE_CONST,

    TYPES_COUNT
};

enum OP_DATA {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,

    OP_EQUAL,
    OP_NOT_EQUAL,
    OP_LESS,
    OP_GREATER,
    OP_LESS_EQUAL,
    OP_GREATER_EQUAL,

    NUM_OP
};

enum OperatorType {
    DECLARATOR,
    LOOP,
    RET,
    IF,
    ELSE,
    EQUAL,          //  5
    ASSG,
    ADD,
    SUB,
    MUL,
    DIV,
    EQ_ABOVE,       //  11
    EQ_BELOW,
    ABOVE,
    BELOW,
    NOT_EQ,         //  15
    BRACE_OPEN,
    BRACE_CLOSE,
    ROUND_OPEN,
    ROUND_CLOSE,
    SEMICOLON,
    COMMA,

    NUM_OPERATORS
};

static char* data_op[] {
    "A.8",                 //"decl",
    "Page_74",             //"while",
    "Page_81",             //"return",
    "Page_69",             //"if",
    "Page_70",             //"else",
    "A.7.10_1",            //"==",
    "A.7.17",              //"=",
    "A.7.7_1",             //"+",
    "A.7.7_2",             //"-",
    "A.7.6_1",             //"*",
    "A.7.6_2",             //"/",
    "A.7.9_4",             //">=",
    "A.7.9_3",             //"<=",
    "A.7.9_2",             //">",
    "A.7.9_1",             //"<",
    "A.7.10_2",            //"!=",
    "Read",                //"{",
    "Till_End",            //"}",
    "K&R",                 //"(",
    "K&R!",                //")",
    "A.8.2",               //";",
    "A.7.18"               //",",
};

static char* reference_op[] {
    "double",
    "while",
    "return",
    "if",
    "else",
    "==",
    "=",
    "+",
    "-",
    "*",
    "/",
    ">=",
    "<=",
    ">",
    "<",
    "!=",
    "{",
    "}",
    "(",
    ")",
    ";",
    ",",
};

static char* math_op[] {
    "A.7.7_1",
    "A.7.7_2",
    "A.7.6_1",
    "A.7.6_2",
    "A.7.10_1",
    "A.7.10_2",
    "A.7.9_1",
    "A.7.9_2",
    "A.7.9_3",
    "A.7.9_4"
};

static char* reference_math_op[] {
    "+",
    "-",
    "*",
    "/",
    "==",
    "!=",
    "<",
    ">",
    "<=",
    ">="
};

typedef struct Node {
    Node* parent;
    Node* left;
    Node* right;
    Data data;
    TYPE type;
} Node;

typedef struct Tree {
    Node* root;
} Tree;

int FileLength(FILE* fp);
char* ReadFile(FILE* fp);
void ClearBuffer(Input* input);
Tree* BuildTree(Tokens* tokens);
void ConstructTokens(Tokens* toxens);
void DestructToxens(Tokens* toxens);
void ParseIntoToxens(Tokens* tokens, Input* input);
bool GetToken(Tokens* tokens, Input* input);
bool IsVar(char* token);
void DumpTokens(Tokens* tokens);

int GetOpData(Tokens* tokens);
Data AssignConst(double number);
Data AssignId(char* identificator);
Data AssignData(int op_type);

Node* NodeConstruct(TYPE type, Data data, Node* parent, Node* left, Node* right);
void NodeDestruct(Node* node);
void DestructNodes(Node* node);
void TreeClear(Tree* tree);

void* SyntaxError(Tokens* tokens, int pos, char* message);
char* GetVar(Tokens* tokens);
Node* GetNatural(Tokens* tokens);
Node* GetGram(Tokens* tokens);
Node* GetProgram(Tokens* tokens);
Node* GetDefi(Tokens* tokens);
Node* GetFDec(Tokens* tokens);
Node* GetComp(Tokens* tokens);
Node* GetStat(Tokens* tokens);
Node* GetExpr(Tokens* tokens);
Node* GetInit(Tokens* tokens);
Node* GetAssgn(Tokens* tokens);
Node* GetRet(Tokens* tokens);
Node* GetCond(Tokens* tokens);
Node* GetLoop(Tokens* tokens);
Node* GetSimp(Tokens* tokens);
Node* GetTerm(Tokens* tokens);
Node* GetPrim(Tokens* tokens);
Node* GetCall(Tokens* tokens);

void TreeVertexPrint(Node* node, FILE* fp);
void TreeEdgesPrint(Node* node, FILE* fp);
void TreeVisualDump(Node* root, const char* file_name);
