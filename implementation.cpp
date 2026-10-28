#include <iostream>
using namespace std;

template <typename Type, int Ord>
struct Node
{
    bool is_leaf;
    int num_keys;
    Type key[Ord];

    Node(bool leaf, int n) : is_leaf(leaf), num_keys(n) {}

    virtual ~Node() = default;
};

template <typename Type, int Ord>
struct LeafNode : public Node<Type, Ord>
{
    LeafNode(int n) : Node<Type, Ord>(true, n) {}
};

template <typename Type, int Ord>
struct InternalNode : public Node<Type, Ord>
{
    Node<Type, Ord> *child[Ord + 1] = {};

    InternalNode(int n) : Node<Type, Ord>(false, n) {}
};

template <typename Type, int Ord>
struct BTree
{
    Node<Type, Ord> *root = nullptr;

    void display_all_inorder(Node<Type, Ord> *node)
    {
        if (node == nullptr)
        {
            return;
        }
        for (int i = 0; i < node->num_keys; i++)
        {
            if (!node->is_leaf)
            {
                auto internal = static_cast<InternalNode<Type, Ord> *>(node);
                display_all_inorder(internal->child[i]);
            }
            cout << node->key[i] << " ";
        }
        if (!node->is_leaf)
        {
            auto internal = static_cast<InternalNode<Type, Ord> *>(node);
            display_all_inorder(internal->child[node->num_keys]);
        }
    }

    bool search(Type key)
    {
        return search(key, root);
    }

    bool search(Type key, Node<Type, Ord> *node)
    {
        if (node == nullptr)
        {
            return false;
        }

        int i = 0;
        while (i < node->num_keys)
        {
            if (key == node->key[i])
            {
                return true;
            }
            else if (key > node->key[i])
            {
                i++;
            }
            else
            {
                break;
            }
        }

        if (node->is_leaf)
        {
            return false;
        }

        auto internal = static_cast<InternalNode<Type, Ord> *>(node);
        return search(key, internal->child[i]);
    }

    // Recursively delete the tree
    void delete_tree(Node<Type, Ord> *node)
    {
        if (node == nullptr)
            return;

        if (!node->is_leaf)
        {
            auto internal =
                static_cast<InternalNode<Type, Ord> *>(node);

            for (int i = 0; i <= node->num_keys; i++)
            {
                delete_tree(internal->child[i]);
            }
        }

        delete node;
    }

    // Destructor to clean up the tree with the delete tree function
    ~BTree()
    {
        delete_tree(root);
    }
};

int main()
{
    BTree<int, 3> tree;

    // Create the root
    auto root = new InternalNode<int, 3>(2);
    root->key[0] = 20;
    root->key[1] = 40;

    // Create the leaf nodes
    auto left = new LeafNode<int, 3>(2);
    left->key[0] = 5;
    left->key[1] = 10;

    auto middle = new LeafNode<int, 3>(2);
    middle->key[0] = 25;
    middle->key[1] = 30;

    auto right = new LeafNode<int, 3>(2);
    right->key[0] = 50;
    right->key[1] = 60;

    // Connect the children
    root->child[0] = left;
    root->child[1] = middle;
    root->child[2] = right;

    // Set the root of the tree
    tree.root = root;

    // test search function
    cout << tree.search(25) << endl;   // 1 => found
    cout << tree.search(1000) << endl; // 0 => not found

    // Test the traversal
    cout << "Inorder traversal: ";
    tree.display_all_inorder(tree.root);
    cout << endl;

    return 0;
}