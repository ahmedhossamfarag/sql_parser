#pragma once
#include "file_controller.h"
#define BlckPtrSz 1
#define BlckTypSz 1
#define IndxTy '#'
#define RecTy '$'
#include "cmp_char.h"

struct NodeParamter
{
	FileController* file;
	BlockController* block_cntr;
	int key_size;
	int record_size;
};

class Node
{
public:
	NodeParamter param;
	int blck_indx;
	char* seq;
	int seq_size;

	Node();

	~Node()
	{
		delete[] seq;
	}

	virtual char* split() = 0;
	virtual void join_left(Node* nd, char* key) = 0;
	virtual int free() = 0;
	virtual bool isfull() = 0;
	virtual char* first_key() = 0;
	virtual int occupy() = 0;

	bool can_join(Node* nd = nullptr);

	bool is_rec_node();
	void save();
}; 

class IndexNode : public Node
{
public:

	char* split();
	void join_left(Node* nd, char* key);
	int free();
	bool isfull();
	char* first_key();
	int occupy();

	void set_frst_indx(int indx);
	char* indx_at(int i);
	char* key_at(int i);
	void set_key(int i, char* key);

	void insert(char* key);
	void remove(char* key);
	int search(char* key);
};

class RecordNode : public Node
{
public:
	int next_node = 0;

	void set_nxt(int nxt);

	char* split();
	void join_left(Node* nd, char* key);
	int free();
	bool isfull();
	char* first_key();
	int occupy();

	char* record_at(int i);

	void insert(char* rec);
	void remove(char* key);
	void remove(CharCondition cnd);
	int search(char* key);
	int search_upper(char* key);
};

Node* read_node(NodeParamter param, int blk);

IndexNode create_indx_node(NodeParamter param);

RecordNode create_rec_node(NodeParamter param, int nxt_node);