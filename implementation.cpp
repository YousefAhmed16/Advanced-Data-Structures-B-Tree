#include <iostream>
using namespace std;

template <typename Type, int Ord>
struct Node{
    bool is_leaf;
    int num_keys;
    Type key[Ord];
};

template <typename Type, int Ord>
struct LeafNode:public Node<Type, Ord>{

};

template <typename Type, int Ord>
struct InternalNode:public Node<Type, Ord>{
    Node<Type, Ord>* child[Ord+1];
};

template <typename Type, int Ord>
struct BTree{
    InternalNode<Type, Ord>* root;

    void display_all_inorder(auto root){
        for (int i = 0; i < root.num_keys; i++){
            if (!root.is_leaf) display_all_inorder(root.child[i]);
            cout << root.key[i] << " ";
        }
        if (!root.is_leaf) display_all_inorder(root.child[root.num_keys]);
    }
};

int main() {
    
    return 0;
}