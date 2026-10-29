#include <iostream>
#include <cmath>

using namespace std;

template <typename Type, int Ord>
struct Node
{
  bool is_leaf;
  int num_keys;
  Type keys[Ord];
  Node() : is_leaf(false), num_keys(0) {}
};

template <typename Type, int Ord>
struct LeafNode : public Node<Type, Ord>
{
  LeafNode()
  {
    this->is_leaf = true;
    this->num_keys = 0;
  }
};

template <typename Type, int Ord>
struct InternalNode : public Node<Type, Ord>
{
  Node<Type, Ord> *child[Ord + 1];

  InternalNode()
  {
    this->is_leaf = false;
    this->num_keys = 0;
    for (int i = 0; i < Ord + 1; ++i)
    {
      child[i] = nullptr;
    }
  }
};

template <typename Type, int Ord>
struct BTree
{
  Node<Type, Ord> *root = nullptr;

  void insert(const Type &key)
  {
    if (root == nullptr)
    {
      root = new LeafNode<Type, Ord>();
      insert_into_leaf(static_cast<LeafNode<Type, Ord> *>(root), key);
      return;
    }

    if (root->num_keys == Ord)
    {
      InternalNode<Type, Ord> *new_root = new InternalNode<Type, Ord>();
      new_root->child[0] = root;
      split_child(new_root, 0);
      root = new_root;
    }

    insert_non_full(root, key);
  }

  void insert_into_leaf(LeafNode<Type, Ord> *leaf, const Type &key)
  {
    int index = leaf->num_keys;

    while (index > 0 && leaf->keys[index - 1] > key)
    {
      leaf->keys[index] = leaf->keys[index - 1];
      index--;
    }

    leaf->keys[index] = key;
    leaf->num_keys++;
  }

  void insert_non_full(Node<Type, Ord> *node, const Type &key)
  {
    if (node->is_leaf)
    {
      LeafNode<Type, Ord> *leaf = static_cast<LeafNode<Type, Ord> *>(node);
      insert_into_leaf(leaf, key);
      return;
    }

    InternalNode<Type, Ord> *internal = static_cast<InternalNode<Type, Ord> *>(node);

    int child_index = internal->num_keys - 1;

    while (child_index >= 0 && key < internal->keys[child_index])
    {
      child_index--;
    }

    child_index++;

    if (internal->child[child_index]->num_keys == Ord)
    {
      split_child(internal, child_index);

      if (key >= internal->keys[child_index])
      {
        child_index++;
      }
    }

    insert_non_full(internal->child[child_index], key);
  }

  void split_child(InternalNode<Type, Ord> *parent, int index)
  {
    Node<Type, Ord> *child = parent->child[index];

    int mid_index = Ord / 2;

    if (child->is_leaf)
    {
      LeafNode<Type, Ord> *old_leaf =
          static_cast<LeafNode<Type, Ord> *>(child);

      LeafNode<Type, Ord> *new_leaf =
          new LeafNode<Type, Ord>();

      Type mid_key = old_leaf->keys[mid_index];

      new_leaf->num_keys =
          old_leaf->num_keys - mid_index - 1;

      for (int i = 0; i < new_leaf->num_keys; i++)
      {
        new_leaf->keys[i] =
            old_leaf->keys[mid_index + 1 + i];
      }

      old_leaf->num_keys = mid_index;

      for (int i = parent->num_keys; i > index; i--)
      {
        parent->child[i + 1] = parent->child[i];
      }

      parent->child[index + 1] = new_leaf;

      for (int i = parent->num_keys; i > index; i--)
      {
        parent->keys[i] = parent->keys[i - 1];
      }

      parent->keys[index] = mid_key;
      parent->num_keys++;
    }
  }

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
      cout << node->keys[i] << " ";
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
      if (key == node->keys[i])
      {
        return true;
      }
      else if (key > node->keys[i])
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

  // deletion
  int min_keys()
  {
    return ceil((Ord / 2.0))-1;
  }

  Type get_predecessor(Node<Type, Ord>* node)
  {
    while (!node->is_leaf)
    {
      auto internal =
          static_cast<InternalNode<Type, Ord>*>(node);

      node = internal->child[node->num_keys];
    }

    return node->keys[node->num_keys - 1];
  }

  void borrow_from_left(
    InternalNode<Type, Ord>* parent,
    int index)
  {
    Node<Type, Ord>* child = parent->child[index];
    Node<Type, Ord>* left = parent->child[index - 1];

    for (int i = child->num_keys; i > 0; i--)
      child->keys[i] = child->keys[i - 1];

    if (!child->is_leaf)
    {
      auto c = static_cast<InternalNode<Type, Ord>*>(child);
      auto l = static_cast<InternalNode<Type, Ord>*>(left);

      for (int i = child->num_keys + 1; i > 0; i--)
        c->child[i] = c->child[i - 1];

      c->child[0] = l->child[left->num_keys];
      l->child[left->num_keys] = nullptr;
    }

    child->keys[0] = parent->keys[index - 1];

    parent->keys[index - 1] =
        left->keys[left->num_keys - 1];

    left->num_keys--;
    child->num_keys++;
  }

  void borrow_from_right(
    InternalNode<Type, Ord>* parent,
    int index)
  {
    Node<Type, Ord>* child = parent->child[index];
    Node<Type, Ord>* right = parent->child[index + 1];

    child->keys[child->num_keys] = parent->keys[index];

    if (!child->is_leaf)
    {
      auto c = static_cast<InternalNode<Type, Ord>*>(child);
      auto r = static_cast<InternalNode<Type, Ord>*>(right);

      c->child[child->num_keys + 1] = r->child[0];

      for (int i = 0; i < right->num_keys; i++)
        r->child[i] = r->child[i + 1];

      r->child[right->num_keys] = nullptr;
    }

    parent->keys[index] = right->keys[0];

    for (int i = 0; i < right->num_keys - 1; i++)
      right->keys[i] = right->keys[i + 1];

    right->num_keys--;
    child->num_keys++;
  }

  void merge_children(
    InternalNode<Type, Ord>* parent,
    int index)
  {
    Node<Type, Ord>* left = parent->child[index];
    Node<Type, Ord>* right = parent->child[index + 1];

    int left_size = left->num_keys;

    left->keys[left_size] = parent->keys[index];

    for (int i = 0; i < right->num_keys; i++)
      left->keys[left_size + 1 + i] = right->keys[i];

    if (!left->is_leaf)
    {
      auto l = static_cast<InternalNode<Type, Ord>*>(left);
      auto r = static_cast<InternalNode<Type, Ord>*>(right);

      for (int i = 0; i <= right->num_keys; i++)
      {
        l->child[left_size + 1 + i] = r->child[i];
        r->child[i] = nullptr;
      }
    }

    left->num_keys += right->num_keys + 1;

    for (int i = index; i < parent->num_keys - 1; i++)
      parent->keys[i] = parent->keys[i + 1];

    for (int i = index + 1; i < parent->num_keys; i++)
      parent->child[i] = parent->child[i + 1];

    parent->child[parent->num_keys] = nullptr;
    parent->num_keys--;

    delete right;
  }
  
  void fix_underflow(
    InternalNode<Type, Ord>* parent,
    int index)
  {
    Node<Type, Ord>* child = parent->child[index];
    int minimum = min_keys();

    if (child->num_keys >= minimum)
      return;

    if (index > 0 &&
        parent->child[index - 1]->num_keys > minimum)
    {
      borrow_from_left(parent, index);
      return;
    }

    if (index < parent->num_keys &&
        parent->child[index + 1]->num_keys > minimum)
    {
      borrow_from_right(parent, index);
      return;
    }

    if (index > 0)
      merge_children(parent, index - 1);
    else
      merge_children(parent, index);
  }

  bool delete_recursive(
    Node<Type, Ord>* node,
    const Type& key)
  {
    int index = 0;

    while (index < node->num_keys &&
           node->keys[index] < key)
    {
      index++;
    }

    if (index < node->num_keys &&
        node->keys[index] == key)
    {
      if (node->is_leaf)
      {
        for (int i = index; i < node->num_keys - 1; i++)
          node->keys[i] = node->keys[i + 1];

        node->num_keys--;
        return true;
      }

      auto internal =
          static_cast<InternalNode<Type, Ord>*>(node);

      Type predecessor =
          get_predecessor(internal->child[index]);

      node->keys[index] = predecessor;

      bool deleted = delete_recursive(
          internal->child[index], predecessor);

      if (deleted &&
          internal->child[index]->num_keys < min_keys())
      {
        fix_underflow(internal, index);
      }

      return deleted;
    }

    if (node->is_leaf)
      return false;

    auto internal =
        static_cast<InternalNode<Type, Ord>*>(node);

    bool deleted = delete_recursive(
        internal->child[index], key);

    if (deleted &&
        internal->child[index]->num_keys < min_keys())
    {
      fix_underflow(internal, index);
    }

    return deleted;
  }

  bool delete_key(const Type& key)
  {
    if (root == nullptr)
      return false;

    bool deleted = delete_recursive(root, key);

    if (!deleted)
      return false;

    if (root->num_keys == 0)
    {
      Node<Type, Ord>* old_root = root;

      if (root->is_leaf)
      {
        root = nullptr;
      }
      else
      {
        auto internal =
            static_cast<InternalNode<Type, Ord>*>(root);

        root = internal->child[0];
        internal->child[0] = nullptr;
      }

      delete old_root;
    }

    return true;
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
  BTree<int, 5> tree;
  tree.insert(10);
  tree.insert(20);
  tree.insert(5);
  tree.insert(6);
  tree.insert(12);
  tree.insert(11);
  tree.insert(14);

  cout << tree.search(20) << endl;
  cout << tree.search(1000) << endl;

  cout << "In-order traversal of the B-tree: ";
  tree.display_all_inorder(tree.root);

  cout<<endl;

  tree.delete_key(20);
  tree.delete_key(14);
  tree.delete_key(5);
  cout << tree.search(20) << endl;
  cout << "In-order traversal of the B-tree: ";
  tree.display_all_inorder(tree.root);

  cout << endl;


  return 0;
}