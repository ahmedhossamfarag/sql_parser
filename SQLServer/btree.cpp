#include "btree.h"
#include "str.h"
long InternalNode::insert(char* key, Node** node)
{
    Node* n;
    int i = 0;
    while (i < keys.size() && compare_char(key, keys[i], key_size) > 0)
        i++;
    long p = nodes[i]->insert(key, &n);
    if (n != nullptr) {
        keys.insert(keys.begin() + i, n->keys.front());
        nodes.insert(nodes.begin() + i + 1, n);
        if (keys.size() == max) {
            InternalNode* newn = new InternalNode();
            newn->keys.resize(max / 2);
            std::copy(keys.begin() + max / 2 + 1, keys.end(), newn->keys.begin());
            newn->nodes.resize(max / 2);
            std::copy(nodes.begin() + max / 2, nodes.end(), newn->nodes.begin());
            *node = newn;
            keys.resize(max / 2);
            nodes.resize(keys.size() + 1);
        }
    }
    return p;
}

long InternalNode::remove(char* key)
{
    return 0;
}

long InternalNode::select(char* key)
{
    return 0;
}

void InternalNode::remove()
{
}

void InternalNode::join(Node* node)
{
}

long LeafNode::insert(char* key, Node** node)
{
    int i = 0;
    while (i < keys.size() && compare_char(key, keys[i], key_size) > 0)
        i++;
    keys.insert(keys.begin() + i, key);
    if (i != keys.size())
    {
        char* c = read_from(i);
        file->go_to_block(data_block, record_size * i);
    }
    if (keys.size() == max) {
        LeafNode* newn = new LeafNode();
        newn->block_index = block_controller->new_block();
        newn->block_controller = block_controller;
        newn->data_block = block_controller->new_block();
        newn->file = file;
        newn->key_size = key_size;
        newn->record_size = record_size;
        newn->max = max;
        newn->next = next;
        newn->keys.resize(max / 2);
        std::copy(keys.begin() + max / 2, keys.end(), newn->keys.begin());
        keys.resize(max / 2);
        char* c = read_from(max / 2);
        file->go_to_block(newn->data_block);
        file->write(c, (max - (max / 2)) * record_size);
        *node = newn;
        if (i >= max / 2)
            return  newn->data_block * BLOCKSIZE + ((i - (max / 2)) * record_size);
    }

    return data_block * BLOCKSIZE + (i * record_size);
}

long LeafNode::remove(char* key)
{
    return 0;
}

long LeafNode::select(char* key)
{
    return 0;
}

void LeafNode::remove()
{
}

void LeafNode::join(Node* node)
{
}

long LeafNode::get_pointer(int indx)
{
    return record_size * indx;
}

long LeafNode::get_key_p(int indx)
{
    return (key_size * indx) + (2 * sizeof(int));
}

char* LeafNode::read_from(int i)
{
    int l = (keys.size() - i) * record_size;
    char* c = new char[l];
    file->go_to_block(data_block, get_pointer(i));
    file->read(c, l);
    return c;
}

void BTree::insert(char* key, char* data)
{
    Node* node;
    long pntr =  root->insert(key, &node);
    file->go_to(pntr);
    file->write(data, record_size);
    if (node != nullptr) {
        InternalNode* n = new InternalNode();
        n->keys.push_back(node->keys.front());
        n->nodes.push_back(root);
        n->nodes.push_back(node);
        root = n;
    }
}

void BTree::remove(char* key)
{
}

char* BTree::select(char* key)
{
    return nullptr;
}

void BTree::update(char* key, char* data)
{
}
