#include "file_controller.h"
#include <algorithm>
#include <iostream>
#include "vec.h"

FileController::FileController(const char* fn) : file(nullptr)
{
	this->file_name = fn;
	pnt = fl = new char[1000];
	std::fill_n(fl, 1000, 0);
}

void FileController::open_read()
{
	fopen_s(&file, file_name, "r");
}

void FileController::open_write()
{
	fopen_s(&file, file_name, "w");
}

void FileController::open_read_write()
{
	fopen_s(&file, file_name, "r+");
}

void FileController::open_add()
{
	fopen_s(&file, file_name, "a");
}

void FileController::close()
{
	fclose(file);
	file = nullptr;
}

void FileController::go_to(long index)
{
	//fseek(file, index, 0);
	pnt = fl + index;
}

void FileController::read(char* pt, size_t count, size_t es)
{
	std::copy_n(pnt, count * es, pt);
}
//
//void FileController::read(void* pt, size_t count, size_t es)
//{
//	fread(pt, es, count, file);
//}

int FileController::read_int()
{
	int* i = new int();
	read((char*)i, 1, sizeof(int));
	return *i;
}

int8_t FileController::read_int8()
{
	char* c = new char();
	read(c, 1, sizeof(int8_t));
	return *c;
}

void FileController::write(char* pt, size_t count, size_t es)
{
	std::copy_n(pt, count * es, pnt);
}
//void FileController::write(void* pt, size_t count, size_t es)
//{
//
//	fwrite(pt, es, count, file);
//}

void FileController::write_int(int i)
{
	write((char*)&i, 1, sizeof(int));
}

void FileController::write_int8(int8_t i)
{
	char c = i;
	write(&c, sizeof(int8_t));
}

void FileController::go_to_block(long index, long offset)
{
	go_to((index * BLOCKSIZE) + offset);
}

char* FileController::read_block(long index)
{
	go_to_block(index);
	char* data = new char[BLOCKSIZE];
	read(data, BLOCKSIZE, sizeof(char));
	return data;
}

void FileController::write_block(char* data, long index)
{
	go_to_block(index);
	write(data, BLOCKSIZE, sizeof(char));
}

BlockController::BlockController(FileController* fc, bool initial)
{
	this->fc = fc;
	if (!initial)
	{
		fc->go_to(0);
		n_block = fc->read_int8();
		free_block = fc->read_int8();
	}
	else
	{
		fc->go_to(0);
		fc->write_int8(0);
		fc->write_int8(0);
		n_block = 0;
		free_block = 0;
	}
}

long BlockController::new_block()
{
	if(!free_block)
		return ++n_block;
	int nb = free_block;
	fc->go_to_block(free_block);
	free_block = fc->read_int8();
	return nb;
}

void BlockController::erase_block(long index)
{
	char* er = new char[BLOCKSIZE];
	std::fill(er, er + BLOCKSIZE, '0');
	*er = free_block;
	fc->write_block(er, index);
	free_block = index;
	delete[] er;
}


BlockController::~BlockController()
{
	fc->go_to(0);
	fc->write_int8(n_block);
	fc->write_int8(free_block);
}

char* create_blk(char c)
{
	char* ar = new char[BLOCKSIZE];
	std::fill_n(ar, BLOCKSIZE, c);
	return ar;
}
