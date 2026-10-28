#include <iostream>

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
    else
    {
      if (root->num_keys == Ord)
      {
        InternalNode<Type, Ord> *new_root = new InternalNode<Type, Ord>();
        new_root->child[0] = root;
        split_child(new_root, 0);
        root = new_root;
      }

      insert_non_full(root, key);
    }

    void insert_into_leaf(LeafNode<Type, Ord> * leaf, const Type &key)
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

    void insert_non_full(Node<Type, Ord> * node, const Type &key)
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

    void split_child(InternalNode<Type, Ord> * parent, int index)
    {
      Node<Type, Ord> *child = parent->child[index];

      int mid_index = Ord / 2;

      if (child->is_leaf)
      {
        LeafNode<Type, Ord> *old_leaf = static_cast<LeafNode<Type, Ord> *>(child);
        LeafNode<Type, Ord> *new_leaf = new LeafNode<Type, Ord>();

        new_leaf->num_keys = old_leaf->num_keys - mid_index;

        for (int i = 0; i < new_leaf->num_keys; i++)
        {
          new_leaf->keys[i] = old_leaf->keys[mid_index + i];
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

        parent->keys[index] = new_leaf->keys[0];
        parent->num_keys++;
      }
    }
  }
};