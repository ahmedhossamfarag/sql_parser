#pragma once
#include "node.h"
#include <functional>
#include "cmp_char.h"

class Iterator
{
public:
	RecordNode* nd;
	int i;
	char* at;
	CharCondition cnd;
	int end_nd_indx;
	int end_indx;

	Iterator(RecordNode* nd);

	char* _next();

	char* next();

	void set(int i);
};

class BpTree
{
public:
	NodeParamter param;
	int root;
	int root_ps;

	void set_root(int ndx);

	RecordNode* get_first_rec_blck();
	void insert(char* rec);
	void remove_all();
	void remove(char* key);
	void remove(CharCondition cnd);
	void update_all(char* rec, bool* at);
	void update(CharCondition cnd, char* rec, bool* at);
	char* select(char* key);
	Iterator select_all();
	Iterator select(CharCondition cnd);
};



