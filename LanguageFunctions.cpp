#include "Language.h"

Node* NodeConstruct(TYPE type, Data data, Node* parent, Node* left, Node* right) {

    Node* new_node = (Node*) calloc(1, sizeof(Node));
    if (new_node == nullptr) {
        fprintf(stderr, "Can not allocate new Node\n");
        return nullptr;
    }

    new_node->left = left;
    new_node->right = right;
    new_node->parent = parent;

    new_node->type = type;
    new_node->data = data;

    return new_node;
}

void NodeDestruct(Node* node) {
    assert(node != nullptr);

    free(node);
}

void TreeConstruct(Tree* tree) {
    assert(tree != nullptr);

    //tree->variables = (int*) calloc(MAX_VARIABLES_COUNT, sizeof(int));
    tree->root = nullptr;

}

void DestructNodes(Node* node) {
    assert(node != nullptr);

    if (node->left != nullptr) {
        return DestructNodes(node->left);
    }
    if (node->right != nullptr) {
        return DestructNodes(node->right);
    }
    free(node);

}

void TreeDestruct(Tree* tree) {
    assert(tree != nullptr);

    TreeClear(tree);

}

void TreeClear(Tree* tree) {
    assert(tree != nullptr);

    if (tree->root != nullptr) {
        DestructNodes(tree->root);
    }

    tree->root = nullptr;

}

