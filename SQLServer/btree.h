#pragma once
#include "file_controller.h"
#include <vector>
class Node
{
public:
	int max;
	int key_size;
	std::vector<char*> keys;
	virtual long insert(char* key, Node** node) = 0;

	virtual long remove(char* key) = 0;

	virtual long select(char* key) = 0;

	virtual void remove() = 0;

	virtual void join(Node* node) = 0;
};

class InternalNode : public Node
{
public:
	std::vector<Node*> nodes;

	long insert(char* key, Node** node) override;

	long remove(char* key) override;

	long select(char* key) override;

	void remove() override;

	void join(Node* node) override;
};

class LeafNode : public Node
{
public:
	FileController* file;
	BlockController* block_controller;
	int record_size;
	long block_index;
	long data_block;
	LeafNode* next;

	long insert(char* key, Node** node) override;

	long remove(char* key) override;

	long select(char* key) override;

	void remove() override;

	void join(Node* node) override;

	long get_pointer(int indx);

	long get_key_p(int indx);

	char* read_from(int indx);
};

class BTree
{
public:
	FileController* file;
	BlockController* block_controller;
	int record_size;
	Node* root;

	void insert(char* key, char* data);

	void remove(char* key);

	char* select(char* key);

	void update(char* key, char* data);
};

