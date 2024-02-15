#include "node.h"
#include "vec.h"
#include "str.h"
#include <stdio.h>

void copy_backward(char* st, char* en, char* ds) 
{
	ds = ds + (en - st) - 1;
	for (char* ps = en - 1; ps >= st; ps--)
	{
		*ds = *ps;
		ds--;
	}
}


Node::Node() : param(NodeParamter()), blck_indx(0), seq(nullptr), seq_size(0)
{
}

bool Node::can_join(Node* nd)
{
	if(nd == nullptr)
		return free() > occupy();
	return free() > nd->occupy();
}

bool Node::is_rec_node()
{
	return *seq == RecTy;
}

void Node::save()
{
	encode_in(&seq_size, seq + BlckTypSz);
	param.file->write_block(seq, blck_indx);
}

char* IndexNode::split()
{
	// split size
	int n1 = seq_size / 2;
	int n2 = seq_size - n1 - 1;

	this->seq_size = n1;

	// create new node;
	IndexNode newnode = create_indx_node(param);
	newnode.seq_size = n2;

	// copy data
	int key_size = param.key_size;

	char* st1 = seq + BlckTypSz + sizeof(int);
	char* st2 = newnode.seq + BlckTypSz + sizeof(int);
	char* i = st1 + (n1 * key_size + (n1 + 1) * BlckPtrSz);
	
	std::copy(i + key_size, seq + BLOCKSIZE, st2);

	// save
	newnode.save();

	// newkey
	char* newkey = new char[key_size + BlckPtrSz];
	std::copy_n(i, key_size, newkey);
	*(newkey + key_size) = newnode.blck_indx;

	// erase
	std::fill(i, seq + BLOCKSIZE, '!');
	this->save();

	return newkey;
}

void IndexNode::join_left(Node* nd, char* key)
{
	char* i = seq + BlckTypSz + sizeof(int) + occupy();

	std::copy_n(key, param.key_size, i);
	i += param.key_size;
	std::copy_n(nd->seq + BlckTypSz + sizeof(int), nd->occupy(), i);

	seq_size += nd->seq_size;
	this->save();

	param.block_cntr->erase_block(nd->blck_indx);
}

int IndexNode::free()
{
	return BLOCKSIZE - (seq_size * param.key_size + (seq_size + 1) * BlckPtrSz) - BlckTypSz - sizeof(int);
}

bool IndexNode::isfull()
{
	return free() < param.key_size + BlckPtrSz;
}

char* IndexNode::first_key()
{
	return seq + BlckTypSz + sizeof(int) + BlckPtrSz;
}

int IndexNode::occupy()
{
	return seq_size * param.key_size + (seq_size + 1) * BlckPtrSz;
}

void IndexNode::set_frst_indx(int indx)
{
	*(seq + BlckTypSz + sizeof(int)) = indx;
}

char* IndexNode::indx_at(int i)
{
	return seq + BlckTypSz + sizeof(int) + i * (param.key_size + BlckPtrSz);
}

char* IndexNode::key_at(int i)
{
	return indx_at(i) + BlckPtrSz;
}

void IndexNode::set_key(int i, char* key)
{
	std::copy_n(key, param.key_size, key_at(i));
}

void IndexNode::insert(char* key)
{
	char* st = first_key();
	char* end = st + occupy();

	int step = param.key_size + BlckPtrSz;

	for (size_t i = 0; i < seq_size; i++)
	{
		if (compare_char(key, st, param.key_size) < 0)
			break;
		st += step;
	}

	copy_backward(st, end, st + step);
	std::copy_n(key, step, st);

	seq_size++;
	save();
}

void IndexNode::remove(char* key)
{
	char* st = first_key();
	char* end = st + occupy();

	int step = param.key_size + BlckPtrSz;

	for (size_t i = 0; i < seq_size; i++)
	{
		if (compare_char(key, st, param.key_size) == 0)
			break;
		st += step;
	}

	if (st == end)
		return;

	std::copy(st + step, end, st);
	std::fill_n(end - step, step, '!');
	seq_size--;
	save();
}

int IndexNode::search(char* key)
{
	char* st = first_key();

	int step = param.key_size + BlckPtrSz;

	for (size_t i = 0; i < seq_size; i++)
	{
		if (compare_char(key, st, param.key_size) < 0)
			return i;
		st += step;
	}
	return seq_size;
}

void RecordNode::set_nxt(int nxt)
{
	next_node = nxt;
	*(seq + BlckTypSz + sizeof(int)) = nxt;
}

char* RecordNode::split()
{
	// split size
	int n1 = seq_size / 2;
	int n2 = seq_size - n1;

	this->seq_size = n1;

	// create new node;
	RecordNode newnode = create_rec_node(param, next_node);
	newnode.seq_size = n2;
	set_nxt(newnode.blck_indx);

	// copy data
	int rec_size = param.record_size;

	char* st1 = first_key();
	char* st2 = newnode.first_key();
	char* i = st1 + n1 * rec_size;

	std::copy(i, seq + BLOCKSIZE, st2);
	std::fill(i, seq + BLOCKSIZE, '!');

	// save
	this->save();
	newnode.save();
	
	//return
	int key_size = param.key_size;
	char* newkey = new char[key_size + BlckPtrSz];
	std::copy_n(st2, key_size, newkey);
	*(newkey + key_size) = newnode.blck_indx;

	return newkey;
}

void RecordNode::join_left(Node* nd, char* key)
{
	char* i = first_key() + occupy();

	std::copy_n(nd->first_key(), nd->occupy(), i);
	
	seq_size += nd->seq_size;
	set_nxt(static_cast<RecordNode*>(nd)->next_node);
	this->save();

	param.block_cntr->erase_block(nd->blck_indx);
}

int RecordNode::free()
{
	return BLOCKSIZE - seq_size * param.record_size - BlckTypSz - sizeof(int) - BlckPtrSz;
}

bool RecordNode::isfull()
{
	return free() < param.record_size;
}

char* RecordNode::first_key()
{
	return seq + BlckTypSz + sizeof(int) + BlckPtrSz;
}

int RecordNode::occupy()
{
	return seq_size * param.record_size;
}

char* RecordNode::record_at(int i)
{
	char* c = new char[param.record_size];
	char* at = first_key() + i * param.record_size;
	std::copy_n(at, param.record_size, c);
	return c;
}

void RecordNode::insert(char* rec)
{
	char* st = first_key();
	char* end = st + occupy();

	for (size_t i = 0; i < seq_size; i++)
	{
		if (compare_char(rec, st, param.key_size) < 0)
			break;
		st += param.record_size;
	}

	copy_backward(st, end, st + param.record_size);
	std::copy_n(rec, param.record_size, st);

	seq_size++;

	save();
}

void RecordNode::remove(char* key)
{
	char* st = first_key();
	char* end = st + occupy();

	for (size_t i = 0; i < seq_size; i++)
	{
		if (compare_char(key, st, param.key_size) == 0)
			break;
		st += param.record_size;
	}

	if (st == end)
		return;

	std::copy(st + param.record_size, end, st );
	std::fill_n(end - param.record_size, param.record_size, '!');
	seq_size--;
	save();
}

void RecordNode::remove(CharCondition cnd)
{
	char* st = first_key();
	char* end = st + occupy();
	int n = seq_size;
	for (size_t i = 0; i < n; i++)
	{
		if (cnd.operate_on(st))
		{
			std::copy(st + param.record_size, end, st);
			seq_size--;
			continue;
		}
		st += param.record_size;
	}
	
	std::fill(st, end, '!');
	save();
}

int RecordNode::search(char* key)
{
	char* st = first_key();

	for (size_t i = 0; i < seq_size; i++)
	{
		if (compare_char(key, st, param.key_size) == 0)
			return i;
		st += param.record_size;
	}
	return -1;
}

int RecordNode::search_upper(char* key)
{
	char* st = first_key();

	for (size_t i = 0; i < seq_size; i++)
	{
		if (compare_char(key, st, param.key_size) < 0)
			return i;
		st += param.record_size;
	}
	return -1;
}

Node* read_node(NodeParamter param, int blk)
{
	char* sq = param.file->read_block(blk);
	Node* nd;
	if (*sq == IndxTy)
	{
		nd = new IndexNode();
	}
	else
	{
		auto rnd = new RecordNode();
		rnd->next_node = *(sq + BlckTypSz + sizeof(int));
		nd = rnd;
	}

	nd->param = param;
	nd->blck_indx = blk;
	nd->seq = sq;
	nd->seq_size = decode_int(sq + BlckTypSz);
	return nd;
}

IndexNode create_indx_node(NodeParamter param)
{
	IndexNode newnode;
	newnode.param = param;
	newnode.blck_indx = param.block_cntr->new_block();
	newnode.seq = new char[BLOCKSIZE];
	*newnode.seq = IndxTy;
	std::fill(newnode.indx_at(0), newnode.seq + BLOCKSIZE, '!');
	return newnode;
}

RecordNode create_rec_node(NodeParamter param, int nxt_node)
{
	RecordNode newnode;
	newnode.param = param;
	newnode.blck_indx = param.block_cntr->new_block();
	newnode.seq = new char[BLOCKSIZE];
	std::fill(newnode.first_key(),newnode.seq + BLOCKSIZE, '!');
	*(newnode.seq) = RecTy;
	newnode.set_nxt(nxt_node);
	return newnode;
}

