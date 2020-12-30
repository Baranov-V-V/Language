#include "TXLib.h"

#include "Language.h"
#include "Compile.h"
#include "BuildTree.cpp"
#include "LanguageFunctions.cpp"
#include "LanguageDump.cpp"
#include "Compile.cpp"

int main() {
    Tokens tokens;
    ConstructTokens(&tokens);

    Tree* tree = BuildTree(&tokens);

    MakeCompilation(tree);

    DestructToxens(&tokens);
    TreeDestruct(tree);
}
