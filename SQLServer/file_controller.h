#pragma once
#include <stdio.h>
#define BLOCKSIZE 100
#include <cstdint>
class FileController
{
public :
	const char* file_name;
	FILE* file;
	char* fl;
	char* pnt;

	FileController(const char* fn);

	void open_read();

	void open_write();

	void open_read_write();

	void open_add();

	void close();

	void go_to(long index);

	void read(char* pt, size_t count, size_t es = sizeof(char));
	//void read(void* pt, size_t count, size_t es = sizeof(char));

	int read_int();

	int8_t read_int8();

	void write(char* pt, size_t count, size_t es = sizeof(char));
	//void write(void* pt, size_t count, size_t es = sizeof(char));

	void write_int(int i);
	
	void write_int8(int8_t i);

	void go_to_block(long index, long offset = 0);

	char* read_block(long index);

	void write_block(char* data, long index);
};

struct SubBlock
{
	long block;
	int n_portions;
	int index;
};

class BlockController
{
public:
	FileController* fc;
	int n_block;
	int free_block;

	BlockController(FileController* fc, bool initial = false);

	long new_block();

	void erase_block(long index);

	~BlockController();
};

char* create_blk(char c = 0);