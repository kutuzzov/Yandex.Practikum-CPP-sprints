#include <cassert>
#include <iostream>
#include <memory>

template <typename T>
struct TreeNode;

template <typename T>
using TreeNodePtr = std::unique_ptr<TreeNode<T>>;

template <typename T>
struct TreeNode {
    TreeNode(T val, TreeNodePtr<T>&& left, TreeNodePtr<T>&& right)
        : value(std::move(val))
        , left(std::move(left))
        , right(std::move(right)) {
    }

    T value;
    TreeNodePtr<T> left;
    TreeNodePtr<T> right;
    TreeNode* parent = nullptr;
};

template <typename T>
bool CheckTreeProperty(const TreeNode<T>* node) noexcept {
    if (node->left.get()) {
        if (node->left.get()->value > node->value || !CheckTreeProperty(node->left.get())) {
            return false;
        }
    }
    if (node->right.get()) {
        if (node->right.get()->value < node->value || !CheckTreeProperty(node->right.get())) {
            return false;
        }
    }
    return true;
}

template <typename T>
TreeNode<T>* begin(TreeNode<T>* node) noexcept {
    while (node->left.get()) {
        node = node->left.get();
    }
    return node;
}

template <typename T>
TreeNode<T>* next(TreeNode<T>* node) noexcept {
    if (node->right != nullptr) {
        node = node->right.get();
        while (node->left != nullptr) {
            node = node->left.get();
        }
    }
    else {
        if (node->parent == nullptr) return nullptr;
        while (node->parent != nullptr) {
            if (node->parent->right.get() == node) {
                node = node->parent;
                if (node->parent == nullptr) return nullptr;
            }
            return node->parent;
        }
    }

    return node;
}

// функция создаёт новый узел с заданным значением и потомками
TreeNodePtr<int> N(int val, TreeNodePtr<int>&& left = {}, TreeNodePtr<int>&& right = {}) {
    auto new_node = std::make_unique<TreeNode<int>>(std::move(val), std::move(left), std::move(right));

    if (new_node->left) {
        auto l = new_node.get()->left.get();
        l->parent = new_node.get();
    }
    if (new_node->right) {
        auto r = new_node.get()->right.get();
        r->parent = new_node.get();
    }
    return new_node;
}

int main() {
    using namespace std;
    using T = TreeNode<int>;
    auto root1 = N(6, N(4, N(3), N(5)), N(7));
    assert(CheckTreeProperty(root1.get()));

    T* iter = begin(root1.get());
    while (iter) {
        cout << iter->value << " "s;
        iter = next(iter);
    }
    cout << endl;

    auto root2 = N(6, N(4, N(3), N(5)), N(7, N(8)));
    assert(!CheckTreeProperty(root2.get()));

    // Функция DeleteTree не нужна. Узлы дерева будут рекурсивно удалены
    // благодаря деструкторам unique_ptr
}
